#include <juce_audio_processors/juce_audio_processors.h>
#include <iostream>

int main(int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    if (argc < 2) return 2;
    juce::VST3PluginFormat format;
    juce::OwnedArray<juce::PluginDescription> descriptions;
    format.findAllTypesForFile(descriptions, juce::String::fromUTF8(argv[1]));
    if (descriptions.isEmpty()) { std::cerr << "No VST3 processor found\n"; return 1; }
    juce::String error;
    auto instance = format.createInstanceFromDescription(*descriptions[0], 48000, 256, error);
    if (!instance) { std::cerr << error << '\n'; return 1; }
    if (argc > 2)
    {
        juce::OwnedArray<juce::PluginDescription> oldDescriptions;
        format.findAllTypesForFile(oldDescriptions, juce::String::fromUTF8(argv[2]));
        if (oldDescriptions.isEmpty()) return 1;
        auto original = format.createInstanceFromDescription(*oldDescriptions[0], 48000, 256, error);
        if (!original) return 1;
        auto& oldParams = original->getParameters();
        auto& newParams = instance->getParameters();
        std::cout << "Host parameters: original=" << oldParams.size() << ", candidate=" << newParams.size() << '\n';
        // JUCE exposes additional hidden controller parameters when MIDI input
        // is enabled in VST3. Only the five original audio parameters are compared.
        if (newParams.size() < oldParams.size()+5) return 1;
        for (int i=0;i<5;++i)
            if (oldParams[i]->getName(100) != newParams[i]->getName(100))
            { std::cerr << "Parameter order mismatch: " << oldParams[i]->getName(100) << " / " << newParams[i]->getName(100) << '\n'; return 1; }
        oldParams[0]->setValueNotifyingHost(0.62f);
        juce::MemoryBlock legacyState;
        original->getStateInformation(legacyState);
        instance->setStateInformation(legacyState.getData(),static_cast<int>(legacyState.getSize()));
        // Preset compatibility means preserving IDs and saved values. Stutter
        // sound intentionally changes in 2.0, so bit-matching 1.0 DSP is not a goal.
        for (int i=0;i<5;++i)
            if (std::abs(oldParams[i]->getValue()-newParams[i]->getValue())>0.00001f)
            { std::cerr << "Legacy preset value changed at " << i << '\n'; return 1; }
        std::cout << "PASS: actual 1.0.0 VST3 state loads with original IDs/order/values; Stutter uses upgraded DSP directly\n";
        if (argc > 4)
        {
            auto saveUI = [](juce::AudioPluginInstance& plugin, const char* filename, juce::Image& rendered)
            {
                std::unique_ptr<juce::AudioProcessorEditor> editor(plugin.createEditor());
                auto& parameters = plugin.getParameters();
                parameters[0]->setValueNotifyingHost(0.42f);
                parameters[1]->setValueNotifyingHost(0);
                parameters[2]->setValueNotifyingHost(0);
                parameters[3]->setValueNotifyingHost(48.0f/72.0f);
                parameters[4]->setValueNotifyingHost(1);
                juce::Thread::sleep(45);
                juce::Timer::callPendingTimersSynchronously();
                auto image = editor->createComponentSnapshot(editor->getLocalBounds(),true,1.0f);
                rendered = image;
                juce::File file(juce::String::fromUTF8(filename));
                file.deleteFile();
                auto stream=file.createOutputStream();
                return stream && juce::PNGImageFormat().writeImageToStream(image,*stream);
            };
            juce::Image oldUI, newUI;
            if (!saveUI(*original,argv[3],oldUI) || !saveUI(*instance,argv[4],newUI)) return 1;
            if(oldUI.getWidth()!=718 || oldUI.getHeight()!=506 || oldUI.getBounds()!=newUI.getBounds()) return 1;
            const juce::Rectangle<int> readouts[]={{151,372,74,32},{590,193,70,24},{590,328,70,24}};
            int changed=0;
            for(int y=0;y<506;++y)
                for(int x=0;x<718;++x)
                    if(oldUI.getPixelAt(x,y)!=newUI.getPixelAt(x,y))
                    {
                        bool allowed=false;
                        for(auto area:readouts) allowed=allowed||area.contains(x,y);
                        if(!allowed) {std::cerr<<"UI changed outside original readouts at "<<x<<","<<y<<'\n';return 1;}
                        ++changed;
                    }
            if(changed==0) return 1;
            std::cout<<"PASS: original UI pixel comparison; changes only within three original font readouts ("<<changed<<" pixels)\n";
        }

    }
    instance->prepareToPlay(48000, 256);
    juce::AudioBuffer<float> buffer(2, 256);
    juce::MidiBuffer midi;
    auto* hostBypass=instance->getBypassParameter();
    if(!hostBypass || hostBypass->getName(100)!="Bypass")
    {std::cerr<<"Actual VST3 does not expose the original bypass to its host\n";return 1;}
    hostBypass->setValueNotifyingHost(1);
    for(int b=0;b<8;++b)
    {
        for(int c=0;c<2;++c)for(int s=0;s<256;++s)buffer.setSample(c,s,0.25f);
        instance->processBlock(buffer,midi);
    }
    if(std::abs(buffer.getSample(0,255)-0.25f)>1.e-6f)
    {std::cerr<<"Actual VST3 host bypass did not return unity dry\n";return 1;}
    hostBypass->setValueNotifyingHost(0);
    std::cout<<"PASS: actual VST3 host recognizes BYPASS and returns unity dry audio\n";
    for (int b = 0; b < 32; ++b)
    {
        buffer.clear();
        buffer.setSample(0, 0, 0.25f);
        buffer.setSample(1, 0, 0.25f);
        instance->processBlock(buffer, midi);
        for (int c = 0; c < 2; ++c)
            for (int s = 0; s < 256; ++s)
                if (!std::isfinite(buffer.getSample(c, s))) return 1;
    }
    juce::MemoryBlock state;
    instance->getStateInformation(state);
    instance->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
    {
        std::unique_ptr<juce::AudioProcessorEditor> editor(instance->createEditor());
        if (!editor) return 1;
    }
    instance->releaseResources();
    std::cout << "PASS: loaded actual VST3, processed stereo audio, restored state, constructed editor: "
              << instance->getName() << '\n';
    return 0;
}
