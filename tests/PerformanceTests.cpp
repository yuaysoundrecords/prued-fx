#include "PluginProcessor.h"
#include "reference/RecoveredPluginEditor.h"
#include "StressChecks.h"
#include "ControlsChecks.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

static void check(bool ok, const char* message)
{
    if (!ok) throw std::runtime_error(message);
}
static void set(YuayStutterAudioProcessor& p, const char* id, float v)
{
    auto* parameter = p.apvts.getParameter(id);
    check(parameter != nullptr, "missing parameter");
    parameter->setValueNotifyingHost(parameter->convertTo0to1(v));
}
static void fill(juce::AudioBuffer<float>& b, float value)
{
    for (int c = 0; c < b.getNumChannels(); ++c)
        for (int s = 0; s < b.getNumSamples(); ++s) b.setSample(c, s, value);
}

// Exercise the complete processor, including both internal and external mix
// stages. Fresh live input must not leak into an already frozen full-wet loop.
static void checkFrozenWetMix()
{
    for (double rate : {44100.0, 48000.0, 96000.0})
        for (float amount : {0.03f, 0.15f, 0.42f, 0.75f, 1.0f})
            for (float mix : {0.0f, 0.5f, 1.0f})
            {
                YuayStutterAudioProcessor p;
                set(p,"OUTPUT_LEVEL",0);
                set(p,"DRY_WET",mix);
                set(p,"REFRESH_MODE",0);
                p.prepareToPlay(rate,512);
                juce::AudioBuffer<float> audio(2,512);
                juce::MidiBuffer midi;
                auto process=[&](float input,int blocks)
                {
                    for(int b=0;b<blocks;++b)
                    { fill(audio,input); p.processBlock(audio,midi); }
                };
                process(0.5f,200);
                set(p,"STUTTER",amount);
                process(0.5f,40);
                process(-0.5f,40);
                const float expected=-0.5f+mix;
                for(int ch=0;ch<2;++ch)
                    for(int s=0;s<512;++s)
                        check(std::abs(audio.getSample(ch,s)-expected)<0.00001f,
                              "live dry input leaked into frozen loop: Stutter amount changed wet mix");
                set(p,"STUTTER",0);
                process(-0.5f,4);
                check(std::abs(audio.getSample(0,511)+0.5f)<0.00001f,
                      "Stutter zero did not restore live input");
            }
    std::cout<<"PASS: complete processor frozen wet isolation at five Stutter amounts, three mix values and three sample rates\n";
}

int main(int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    try
    {
        if(argc>1 && juce::String(argv[1])=="--controls-only"){runControlsChecks();return 0;}
        if(argc>1 && juce::String(argv[1])=="--stress-only"){runStressChecks();return 0;}
        if(argc>1 && juce::String(argv[1])=="--wet-mix-only"){checkFrozenWetMix();return 0;}
        // Old APVTS XML state, with the original IDs and no 2.0 parameters.
        YuayStutterAudioProcessor p;
        const auto legacy = juce::parseXML(
            "<PARAMETERS><PARAM id='STUTTER' value='0.73'/><PARAM id='BYPASS' value='1'/>"
            "<PARAM id='REFRESH_MODE' value='1'/><PARAM id='OUTPUT_LEVEL' value='-5'/>"
            "<PARAM id='DRY_WET' value='0.37'/></PARAMETERS>");
        juce::MemoryBlock state;
        juce::AudioProcessor::copyXmlToBinary(*legacy, state);
        set(p, "INTENSITY", 0.2f);
        p.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        check(std::abs(p.getStutterAmount() - 0.73f) < 0.001f, "legacy stutter state");
        check(p.apvts.getParameter("PERFORMANCE_MODE") == nullptr, "separate looper mode must not exist");
        check(p.apvts.getRawParameterValue("INTENSITY")->load() == 1, "missing parameter defaults");
        {
            std::unique_ptr<juce::AudioProcessorEditor> editor(p.createEditor());
            check(editor->getWidth() == 718 && editor->getHeight() == 506, "original editor dimensions changed");
            check(editor->getNumChildComponents() == 9, "original main UI controls changed");
            check(std::abs(p.getStutterAmount() - 0.73f) < 0.001f, "opening editor changed stutter");
            check(p.apvts.getRawParameterValue("OUTPUT_LEVEL")->load() == -5, "editor changed output");
            if (argc > 1)
            {
                YuayStutterAudioProcessor preview;
                set(preview, "STUTTER", 0.42f);
                std::unique_ptr<juce::AudioProcessorEditor> previewEditor(preview.createEditor());
                auto image = previewEditor->createComponentSnapshot(previewEditor->getLocalBounds(), true, 1.0f);
                const bool uiOnly = juce::String(argv[1]) == "--ui-only";
                juce::File outputFile(juce::String::fromUTF8(argv[uiOnly ? 2 : 1]));
                outputFile.deleteFile();
                auto stream = outputFile.createOutputStream();
                check(stream != nullptr && juce::PNGImageFormat().writeImageToStream(image, *stream), "UI snapshot");
                YuayStutterAudioProcessor baselineProcessor;
                RecoveredPruedEditor baselineEditor(baselineProcessor);
                set(baselineProcessor,"STUTTER",0.42f);
                set(baselineProcessor,"OUTPUT_LEVEL",0);
                baselineEditor.timerCallback();
                auto reference=baselineEditor.createComponentSnapshot(baselineEditor.getLocalBounds(),true,1.0f);
                const juce::Rectangle<int> readouts[]={{151,372,74,32},{590,193,70,24},{590,328,70,24},{646,38,58,26}};
                int changed=0;
                check(reference.getBounds()==image.getBounds(),"original UI size mismatch");
                for(int y=0;y<506;++y)
                    for(int x=0;x<718;++x)
                        if(reference.getPixelAt(x,y)!=image.getPixelAt(x,y))
                        {
                            bool allowed=false;
                            for(auto area:readouts)allowed=allowed||area.contains(x,y);
                            check(allowed,"UI changed outside the original three readouts");
                            ++changed;
                        }
                check(changed>0,"font change not visible");
                auto save=[&](const char* name,const juce::Image& rendered)
                {
                    auto file=outputFile.getSiblingFile(name);file.deleteFile();
                    auto output=file.createOutputStream();
                    check(output && juce::PNGImageFormat().writeImageToStream(rendered,*output),"comparison snapshot");
                };
                save("ui-original.png",reference);save("ui-corrected.png",image);
                std::cout<<"PASS: original UI; "<<changed<<" pixels changed only in three existing font readouts\n";
                if(uiOnly)return 0;
            }
        }
        juce::MemoryBlock roundTrip;
        p.getStateInformation(roundTrip);
        YuayStutterAudioProcessor restored;
        restored.setStateInformation(roundTrip.getData(), static_cast<int>(roundTrip.getSize()));
        check(std::abs(restored.getStutterAmount() - 0.73f) < 0.001f, "state roundtrip");

        // New engine remains finite at different rates and host block sizes.
        for (double rate : { 44100.0, 48000.0, 96000.0 })
        {
            PerformanceLooper looper;
            looper.prepare(rate, 2);
            looper.setTransport(120, 0, false, false);
            juce::AudioBuffer<float> audio(2, 127);
            for (int block = 0; block < 600; ++block)
            {
                fill(audio, block < 250 ? 0.7f : -0.7f);
                if (block == 350) looper.requestRecapture();
                looper.process(audio, 0, 127, block > 100 ? 0.8f : 0.0f,
                    1, true, 0.25f, false);
                for (int s = 0; s < 127; ++s)
                {
                    check(std::isfinite(audio.getSample(0, s)), "nonfinite sample");
                    check(std::abs(audio.getSample(0, s)) <= 0.701f, "unexpected gain");
                    check(audio.getSample(0, s) == audio.getSample(1, s), "stereo phase drift");
                }
            }
            check(looper.getCaptureCount() == 2, "recapture count");
        }

        // Frozen loop must survive history wrap, beyond the legacy ten-second ring.
        PerformanceLooper frozen;
        frozen.prepare(48000, 2);
        frozen.setTransport(120, 0, false, false);
        juce::AudioBuffer<float> block(2, 512);
        for (int i = 0; i < 1800; ++i)
        {
            fill(block, i < 100 ? 0.5f : -0.5f);
            frozen.process(block, 0, 512, i >= 50 ? 0.5f : 0, 1, true, 0.25f, false);
        }
        check(std::abs(block.getSample(0, 500) - 0.5f) < 0.001f, "frozen loop overwritten");
        fill(block, -0.5f);
        frozen.process(block, 0, 512, 0.5f, 0, true, 0.25f, false);
        check(std::abs(block.getSample(0, 500) + 0.5f) < 0.001f, "intensity zero is not dry");

        // Input events must take effect at their actual sample offsets.
        YuayStutterAudioProcessor midi;
        set(midi, "OUTPUT_LEVEL", 0);
        midi.prepareToPlay(48000, 64);
        juce::AudioBuffer<float> large(2, 511);
        juce::MidiBuffer events;
        fill(large, 0.25f);
        events.addEvent(juce::MidiMessage::controllerEvent(1, 1, 127), 300);
        midi.processBlock(large, events);
        check(midi.getStutterAmount() == 1, "CC1 mapping");
        for (int i = 0; i < 300; ++i)
            check(std::abs(large.getSample(0, i) - 0.25f) < 0.001f, "MIDI applied before timestamp");
        check(midi.acceptsMidi(), "MIDI input disabled");
        // The ORIGINAL Stutter parameter must engage the upgraded frozen loop
        // directly, without enabling a second mode or opening another control.
        YuayStutterAudioProcessor direct;
        set(direct,"OUTPUT_LEVEL",0);
        direct.prepareToPlay(48000,512);
        juce::AudioBuffer<float> directAudio(2,512);
        juce::MidiBuffer noMidi;
        for(int b=0;b<100;++b) {fill(directAudio,0.5f);direct.processBlock(directAudio,noMidi);}
        set(direct,"STUTTER",1);
        for(int b=0;b<8;++b) {fill(directAudio,0.5f);direct.processBlock(directAudio,noMidi);}
        for(int b=0;b<1200;++b) {fill(directAudio,-0.5f);direct.processBlock(directAudio,noMidi);}
        check(std::abs(directAudio.getSample(0,500)-0.5f)<0.001f,"original Stutter did not hold upgraded loop directly");
        set(direct,"STUTTER",0);
        fill(directAudio,-0.5f);direct.processBlock(directAudio,noMidi);
        check(std::abs(directAudio.getSample(0,500)+0.5f)<0.001f,"original Stutter zero did not release loop");
        // Host-grid capture commits only on a sixteenth boundary (6000 samples).
        PerformanceLooper grid;
        grid.prepare(48000, 2);
        juce::AudioBuffer<float> one(2, 1);
        int commit = -1;
        for (int i=0; i<24000; ++i)
        {
            grid.setTransport(120, i/24000.0, true, true);
            fill(one, 0.5f);
            grid.process(one,0,1,i>=2000?0.4f:0,1,true,0.0625f,false);
            if (grid.getCaptureCount()>0) { commit=i;break; }
        }
        check(commit == 6000, "capture did not align with host sixteenth");

        // A deliberately discontinuous captured waveform must not jump at wraps,
        // capture transitions, or release when the live input is continuous.
        PerformanceLooper seams;
        seams.prepare(48000,2);seams.setTransport(120,0,false,false);
        float previous=0, maximumJump=0;
        for(int i=0;i<40000;++i)
        {
            const float input=static_cast<float>(std::sin(i*0.015));
            fill(one,input);
            if(i==18000)seams.requestRecapture();
            seams.process(one,0,1,(i>=3000&&i<30000)?0.7f:0,1,true,0.0625f,false);
            const float value=one.getSample(0,0);
            if(i>0)maximumJump=juce::jmax(maximumJump,std::abs(value-previous));
            previous=value;
        }
        check(maximumJump<0.06f,"crossfade left an audible step in the seam fixture");
        // Knob/controller noise around a rate boundary must not flutter between
        // repeat lengths. Both engines receive exactly the same audio history.
        PerformanceLooper steady,jitter;
        steady.prepare(48000,2);jitter.prepare(48000,2);
        steady.setTransport(120,0,false,false);jitter.setTransport(120,0,false,false);
        juce::AudioBuffer<float> a(2,64),b(2,64);
        for(int blockIndex=0;blockIndex<600;++blockIndex)
        {
            for(int sample=0;sample<64;++sample)
                for(int ch=0;ch<2;++ch)
                {
                    const float value=static_cast<float>(std::sin((blockIndex*64+sample)*0.021));
                    a.setSample(ch,sample,value);b.setSample(ch,sample,value);
                }
            const float base=blockIndex<100?0.0f:0.49f;
            const float motion=blockIndex<200?base:(blockIndex%2?0.499f:0.501f);
            steady.process(a,0,64,base,1,true,0.25f,false);
            jitter.process(b,0,64,motion,1,true,0.25f,false);
            for(int sample=0;sample<64;++sample)
                check(std::abs(a.getSample(0,sample)-b.getSample(0,sample))<0.000001f,"rate flutter near knob boundary");
        }
        std::cout << "PASS: knob boundary jitter does not change repeat rate\n";
        std::cout << "PASS: host grid at sample " << commit << ", maximum seam-fixture step " << maximumJump << '\n';
        std::cout << "PASS: legacy presets, editor state, roundtrip, rates, stereo, recapture, freeze, intensity, MIDI offsets and oversized blocks\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
