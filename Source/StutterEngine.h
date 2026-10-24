#pragma once

#include <JuceHeader.h>


class StutterEngine
{
public:

    void prepare(double sampleRate,
                 int maximumBlockSize,
                 int numChannels);

    void reset();

    void process(juce::AudioBuffer<float>& buffer,
                 float stutterAmount,
                 bool refreshMode);

    void setTempo(double newBpm);


private:
    double bpm = 120.0;

    double samplesPerBeat = 22050.0;
    double currentBeatLength = 22050.0;
    double stutterDivision = 16.0;
    double targetDivision = 16.0;
   
    double currentSampleRate = 44100.0;

    int channelCount = 2;


    // ==========================
    // Continuous audio buffer
    // ==========================

    juce::AudioBuffer<float> circularBuffer;

    int bufferSize = 0;

    int writePosition = 0;


    // ==========================
    // Stutter playback
    // ==========================

    int readPosition = 0;

    bool isLooping = false;


    int loopStart = 0;

    int loopEnd = 0;

    int loopLength = 0;
    
    int lastLoopLength = 0;



   


    // ==========================
    // Loop smoothing
    // ==========================

    float currentLoopLength = 0.0f;

    float targetLoopLength = 0.0f;



    // ==========================
    // Refresh mode
    // ==========================

    int refreshCounter = 0;

    int refreshInterval = 0;

};