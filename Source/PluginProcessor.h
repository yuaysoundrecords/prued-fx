#pragma once

#include <JuceHeader.h>
#include "PerformanceLooper.h"


namespace YuayParameterID
{
    static constexpr const char* Stutter = "STUTTER";
    static constexpr const char* RefreshMode = "REFRESH_MODE";

    static constexpr const char* OutputLevel = "OUTPUT_LEVEL";
    static constexpr const char* DryWet = "DRY_WET";
    static constexpr const char* Bypass = "BYPASS";
    static constexpr const char* Intensity = "INTENSITY";
    static constexpr const char* BeatSync = "BEAT_SYNC";
    static constexpr const char* LoopBeats = "LOOP_BEATS";
    static constexpr const char* Recapture = "RECAPTURE";
}


class YuayStutterAudioProcessor :
    public juce::AudioProcessor
{

public:

    YuayStutterAudioProcessor();

    ~YuayStutterAudioProcessor() override;


    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();


    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock
    ) override;


    void releaseResources() override;


    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&
    ) override;



    bool hasEditor() const override;
    juce::AudioProcessorParameter* getBypassParameter() const override
    {
        return apvts.getParameter(YuayParameterID::Bypass);
    }


    juce::AudioProcessorEditor* createEditor() override;



    const juce::String getName() const override;



    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;


    double getTailLengthSeconds() const override;



    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int) override;


    const juce::String getProgramName(int) override;


    void changeProgramName(
        int,
        const juce::String&
    ) override;



    void getStateInformation(
        juce::MemoryBlock&
    ) override;


    void setStateInformation(
        const void*,
        int
    ) override;



    float getStutterAmount() const noexcept;

    bool isRefreshMode() const noexcept;



    juce::AudioProcessorValueTreeState apvts;



private:

    PerformanceLooper performanceLooper;
    void processRange(juce::AudioBuffer<float>&, int start, int count);
    juce::AudioBuffer<float> dryBuffer;
    juce::ValueTree defaultState;
    juce::SmoothedValue<float> mixSmoother, gainSmoother, bypassSmoother;
    int maximumBlock = 1;
    bool previousRecapture = false, midiRecaptureDown = false;
    std::atomic<float>* intensityParameter = nullptr;
    std::atomic<float>* syncParameter = nullptr;
    std::atomic<float>* beatsParameter = nullptr;
    std::atomic<float>* recaptureParameter = nullptr;
    static constexpr int StutterCC = 1;
    static constexpr int RefreshCC = 7;


    std::atomic<float>* stutterParameter = nullptr;

    std::atomic<float>* refreshParameter = nullptr;
    
    std::atomic<float>* outputLevelParameter = nullptr;
std::atomic<float>* dryWetParameter = nullptr;
std::atomic<float>* bypassParameter = nullptr;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        YuayStutterAudioProcessor
    )
};
