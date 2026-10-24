#pragma once
#include "PluginEditor.h"
#include <stdexcept>
#include <iostream>

inline void runControlsChecks()
{
    auto require=[](bool ok,const char* why){if(!ok)throw std::runtime_error(why);};
    YuayStutterAudioProcessor p(false);
    require(p.getBypassParameter()==p.apvts.getParameter("BYPASS"),"host bypass is not the original BYPASS parameter");
    YuayStutterAudioProcessorEditor editor(p);
    editor.setVisible(true);
    auto* button=dynamic_cast<juce::Button*>(editor.getComponentAt(359,485));
    require(button!=nullptr,"BYPASS text is not clickable");
    require(editor.getComponentAt(332,470)==button && editor.getComponentAt(386,470)==button,"BYPASS click target does not cover its label");
    const auto before=editor.createComponentSnapshot(editor.getLocalBounds());
    button->setToggleState(true,juce::sendNotification);
    require(p.apvts.getRawParameterValue("BYPASS")->load()==1,"editor bypass click did not reach DSP parameter");
    p.prepareToPlay(48000,256);
    juce::AudioBuffer<float> audio(2,256);juce::MidiBuffer midi;
    for(int b=0;b<8;++b)
    {
        for(int c=0;c<2;++c)for(int s=0;s<256;++s)audio.setSample(c,s,0.3f);
        p.processBlock(audio,midi);
    }
    require(std::abs(audio.getSample(0,255)-0.3f)<1.e-6f,"bypass did not return unity dry audio");
    button->setToggleState(false,juce::sendNotification);
    require(p.apvts.getRawParameterValue("BYPASS")->load()==0,"second bypass click did not restore effect");
    for(int b=0;b<8;++b)
    {
        for(int c=0;c<2;++c)for(int s=0;s<256;++s)audio.setSample(c,s,0.3f);
        p.processBlock(audio,midi);
    }
    require(std::abs(audio.getSample(0,255)-0.3f*juce::Decibels::decibelsToGain(-12.f))<1.e-6f,"unbypass did not restore output gain");
    p.getBypassParameter()->setValueNotifyingHost(1);
    editor.timerCallback();
    require(button->getToggleState(),"host bypass automation did not update editor state");
    const auto after=editor.createComponentSnapshot(editor.getLocalBounds());
    bool changed=false;
    for(int y=459;y<490;++y)for(int x=341;x<375;++x)
        changed=changed || before.getPixelAt(x,y)!=after.getPixelAt(x,y);
    require(changed,"host bypass automation did not update indicator");

    // A circular output knob has a fixed silhouette; check every supplied frame,
    // rather than only the startup image, to catch accumulating crop drift.
    FilmStripKnob knob;knob.setSize(180,180);knob.setScale(0.13f);
    knob.setImage(juce::ImageCache::getFromMemory(BinaryData::knob3_png,BinaryData::knob3_pngSize));
    int minTop=180,maxTop=0,minBottom=180,maxBottom=0;
    for(int frame=0;frame<127;++frame)
    {
        knob.setValue(static_cast<float>(frame)/126.f);
        const auto image=knob.createComponentSnapshot(knob.getLocalBounds());
        int top=180,bottom=-1;
        for(int y=0;y<180;++y)for(int x=0;x<180;++x)
            if(image.getPixelAt(x,y).getAlpha()>64){top=juce::jmin(top,y);bottom=juce::jmax(bottom,y);}
        require(bottom>=top,"knob frame is empty");
        minTop=juce::jmin(minTop,top);maxTop=juce::jmax(maxTop,top);
        minBottom=juce::jmin(minBottom,bottom);maxBottom=juce::jmax(maxBottom,bottom);
    }
    require(maxTop-minTop<=1 && maxBottom-minBottom<=1,"rotary knob drifts vertically across its filmstrip");
    std::cout<<"PASS: BYPASS label hit target, editor toggle, host bypass mapping/indicator, unity dry and restored gain; 127 knob frames stay within one pixel\n";
}
