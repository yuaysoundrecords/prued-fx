#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>
static void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
int main(int argc,char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    try
    {
        if(argc==3 && juce::String(argv[1])=="--live")
        {
            const auto key=juce::File(juce::String::fromUTF8(argv[2])).loadFileAsString().trim();
            require(!key.isEmpty(),"missing private live-test key file");
            LicenseManager live(false);
            require(live.activate(key),"live activation did not start");
            for(int i=0;i<1500 && live.isChecking();++i)juce::Thread::sleep(10);
            require(!live.isChecking(),"Gumroad check timed out");
            if(!live.isUnlocked())throw std::runtime_error(live.status().toStdString());
            std::cout<<"PASS: real Gumroad-issued key verified over HTTPS by the production verifier; no key or customer data logged\n";
            return 0;
        }
        TrialGate trial;
        require(trial.consume(48000*900,48000),"trial still expires at the old 15-minute limit");
        require(trial.consume(48000*899,48000),"trial expired before 30 minutes");
        require(trial.consume(47999,48000),"trial expired before last sample");
        require(!trial.consume(1,48000),"trial did not expire at 1800 seconds");
        require(!trial.consume(100,96000),"sample-rate change reset expired trial");
        TrialGate reinserted;require(reinserted.remaining()==1800,"new instance did not reset trial");
        auto response=juce::JSON::parse(R"({"success":true,"purchase":{"product_id":"raUk5vGzn7s8SxrosloWIQ==","refunded":false,"disputed":false,"chargebacked":false}})");
        require(LicenseManager::evaluate(response).valid,"valid license rejected");
        auto object=response["purchase"].getDynamicObject();
        for(auto flag:{"refunded","disputed","chargebacked"})
        {
            object->setProperty(flag,true);require(!LicenseManager::evaluate(response).valid,"revoked license accepted");object->setProperty(flag,false);
        }
        object->setProperty("product_id","other");require(!LicenseManager::evaluate(response).valid,"wrong product accepted");
        require(!LicenseManager::evaluate(juce::var()).valid,"malformed response accepted");
        require(!LicenseManager::evaluate(juce::JSON::parse(R"({"success":false})")).valid,"invalid key accepted");
        LicenseManager manager(false,[](const juce::String& key){return LicenseManager::Result{key=="fixture-valid",false,"Invalid fixture key"};});
        require(manager.activate("fixture-valid"),"async activation failed to start");
        for(int i=0;i<100 && manager.isChecking();++i)juce::Thread::sleep(5);
        require(manager.isUnlocked(),"valid background activation did not unlock");
        require(manager.activate("fixture-invalid"),"invalid-key check did not start");
        for(int i=0;i<100 && manager.isChecking();++i)juce::Thread::sleep(5);
        require(!manager.isUnlocked(),"rejected license remained unlocked");

        // Accelerated host clock: actual processor must expire, pass dry, and
        // remain expired after prepare/reload-state. No production clock override.
        YuayStutterAudioProcessor processor(false,[](const juce::String&){return LicenseManager::Result{true,false,{}};});
        processor.apvts.getParameter("STUTTER")->setValueNotifyingHost(1);
        require(processor.trialRemainingSeconds()==1800,"new processor did not start with 30 minutes");
        processor.prepareToPlay(64,64);
        juce::AudioBuffer<float> audio(2,64);juce::MidiBuffer midi;
        for(int second=0;second<1801;++second)
        {
            for(int ch=0;ch<2;++ch)for(int s=0;s<64;++s)audio.setSample(ch,s,0.25f);
            processor.processBlock(audio,midi);
            if(second==900)
            {
                require(processor.trialRemainingSeconds()>0,"actual processor expired at old 15-minute limit");
                require(std::abs(audio.getSample(0,63)-0.25f)>0.1f,"actual processor bypassed at old 15-minute limit");
            }
        }
        require(processor.trialRemainingSeconds()==0,"processor did not expire");
        require(std::abs(audio.getSample(0,63)-0.25f)<0.000001f,"expiry did not return unity dry audio");
        juce::MemoryBlock state;processor.getStateInformation(state);
        processor.releaseResources();processor.prepareToPlay(128,128);
        processor.setStateInformation(state.getData(),static_cast<int>(state.getSize()));
        require(processor.trialRemainingSeconds()==0,"prepare or preset reload reset trial");
        auto xml=juce::AudioProcessor::getXmlFromBinary(state.getData(),static_cast<int>(state.getSize()));
        require(!xml->toString().containsIgnoreCase("license"),"license material leaked into preset");
        require(processor.licensing().activate("fixture-paid"),"expired processor activation failed");
        for(int i=0;i<100 && processor.licensing().isChecking();++i)juce::Thread::sleep(5);
        require(processor.licensing().isUnlocked(),"paid processor did not unlock");
        audio.setSize(2,128);
        for(int b=0;b<4;++b)
        {
            for(int ch=0;ch<2;++ch)for(int s=0;s<128;++s)audio.setSample(ch,s,0.25f);
            processor.processBlock(audio,midi);
        }
        require(std::abs(audio.getSample(0,127)-0.25f*juce::Decibels::decibelsToGain(-12.0f))<0.00001f,"paid activation did not restore effect processing after trial expiry");
        std::cout<<"PASS: active at old 15-minute limit; exact 30-minute expiry; new-instance reset; prepare/preset cannot reset; dry expiry; async license validation; invalid/refunded/disputed/wrong-product keys rejected; no keys in presets\n";
        return 0;
    }
    catch(const std::exception& error){std::cerr<<"FAIL: "<<error.what()<<'\n';return 1;}
}
