#pragma once
#include "PluginProcessor.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

inline void runStressChecks()
{
    struct Head : juce::AudioPlayHead
    {
        double bpm=120,ppq=0;bool playing=true;
        juce::Optional<PositionInfo> getPosition() const override
        {PositionInfo p;p.setBpm(bpm);p.setPpqPosition(ppq);p.setIsPlaying(playing);return p;}
    };
    int scenarios=0;
    for(double rate:{44100.0,48000.0,96000.0})
        for(int size:{1,16,64,511,4096})
        {
            YuayStutterAudioProcessor processor;Head head;processor.setPlayHead(&head);
            processor.prepareToPlay(rate,size);
            juce::AudioBuffer<float> block(2,size);juce::MidiBuffer midi;midi.ensureSize(256);
            int previousEvent=-1;float peak=0;
            for(int cursor=0;cursor<static_cast<int>(rate*4);cursor+=size)
            {
                const double t=cursor/rate;
                head.bpm=t<1.5?60:t<2.5?120:180;
                head.playing=!(t>1.8 && t<2.1);
                head.ppq=t*head.bpm/60+(t>2.4?8:0);
                midi.clear();
                const int event=cursor/2048;
                if(event!=previousEvent)
                {
                    previousEvent=event;
                    midi.addEvent(juce::MidiMessage::controllerEvent(1,1,(event*37)%128),size/2);
                    midi.addEvent(juce::MidiMessage::controllerEvent(1,7,event%3?0:127),size/2);
                    if(event%5==0)midi.addEvent(juce::MidiMessage::controllerEvent(1,18,event%10?0:127),size-1);
                }
                for(int s=0;s<size;++s)
                    for(int ch=0;ch<2;++ch)
                    {
                        const double time=(cursor+s)/rate;
                        const double phase=std::fmod(time,0.5);
                        const float kick=static_cast<float>(std::exp(-phase*35)*std::sin(2*juce::MathConstants<double>::pi*65*time));
                        block.setSample(ch,s,0.35f*kick+0.2f*static_cast<float>(std::sin(time*ch*7+time*440)));
                    }
                processor.processBlock(block,midi);
                for(int s=0;s<size;++s)for(int ch=0;ch<2;++ch)
                {
                    const float value=block.getSample(ch,s);
                    if(!std::isfinite(value) || std::abs(value)>0.551f)
                        throw std::runtime_error("transport/automation stress produced invalid or excessive output");
                    peak=juce::jmax(peak,std::abs(value));
                }
            }
            processor.setPlayHead(nullptr);processor.releaseResources();++scenarios;
        }
    std::cout<<"PASS: "<<scenarios<<" transport/automation scenarios; 44.1/48/96 kHz; blocks 1/16/64/511/4096; BPM 60/120/180; stop/seek; rapid CC1/CC7/recapture; finite bounded output\n";
}
