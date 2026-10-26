#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>
static void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(){juce::ScopedJuceInitialiser_GUI init;try{
 YuayStutterAudioProcessor p;
 require(p.apvts.getRawParameterValue("OUTPUT_LEVEL")->load()==0.0f,"default output is not unity");
 p.prepareToPlay(64,64);juce::AudioBuffer<float> audio(2,64);juce::MidiBuffer midi;
 for(int second=0;second<3601;++second){audio.clear();for(int ch=0;ch<2;++ch)for(int i=0;i<64;++i)audio.setSample(ch,i,0.25f);p.processBlock(audio,midi);require(std::abs(audio.getSample(0,63)-0.25f)<1.e-6f,"default audio changed level");}
 auto* gain=p.apvts.getParameter("OUTPUT_LEVEL");gain->setValueNotifyingHost(gain->convertTo0to1(-6.0f));
 juce::MemoryBlock state;p.getStateInformation(state);YuayStutterAudioProcessor restored;restored.setStateInformation(state.getData(),(int)state.getSize());
 require(restored.apvts.getRawParameterValue("OUTPUT_LEVEL")->load()==-6.0f,"saved gain was overwritten");
 std::cout<<"PASS: unity default, one hour unrestricted processing, saved output retained\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
