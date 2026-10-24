#include "StutterEngine.h"

void StutterEngine::prepare(
    double sampleRate,
    int maximumBlockSize,
    int numChannels)
{
    juce::ignoreUnused(maximumBlockSize);

    currentSampleRate = sampleRate;
    channelCount = numChannels;

    samplesPerBeat = (currentSampleRate * 60.0) / bpm;
    currentBeatLength = samplesPerBeat;

    bufferSize = (int)(currentSampleRate * 10.0);

    circularBuffer.setSize(channelCount, bufferSize);
    circularBuffer.clear();

    writePosition = 0;
    readPosition = 0;

    loopStart = 0;
    loopEnd = 0;
    loopLength = 0;
    lastLoopLength = 0;

    isLooping = false;


    refreshCounter = 0;
    refreshInterval = 0;

    currentLoopLength = 0.0f;
    targetLoopLength = 0.0f;

}

void StutterEngine::reset()
{
    circularBuffer.clear();

    writePosition = 0;
    readPosition = 0;

    loopStart = 0;
    loopEnd = 0;
    loopLength = 0;
    lastLoopLength = 0;

    isLooping = false;

    refreshCounter = 0;

    currentLoopLength = 0.0f;
    targetLoopLength = 0.0f;
    
}

void StutterEngine::setTempo(double newBpm)
{
    if (newBpm <= 0.0)
        return;

    bpm = newBpm;

    samplesPerBeat =
        (currentSampleRate * 60.0) / bpm;

    currentBeatLength =
        samplesPerBeat;
}

void StutterEngine::process(
    juce::AudioBuffer<float>& buffer,
    float stutterAmount,
    bool refreshMode)
{
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();

  //==================================================
// Loop Length
//==================================================

const float minLoop =
    (float)(currentBeatLength / 16.0);

const float maxLoop =
    (float)currentBeatLength;

// Curve ให้ช่วงต้นละเอียดกว่า
float curve =
    stutterAmount * stutterAmount;

targetLoopLength =
    juce::jmap(
        curve,
        0.0f,
        1.0f,
        maxLoop,
        minLoop
    );

// Smooth
currentLoopLength +=
    (targetLoopLength - currentLoopLength)
    * 0.08f;

loopLength =
    juce::jlimit(
        64,
        bufferSize - 1,
        (int)currentLoopLength);

//==================================================
// Record
//==================================================

for (int ch = 0; ch < channels; ++ch)
{
    auto* input =
        buffer.getReadPointer(ch);

    auto* ring =
        circularBuffer.getWritePointer(ch);

    int wp = writePosition;

    for (int i = 0; i < samples; ++i)
    {
        ring[wp] = input[i];

        ++wp;

        if (wp >= bufferSize)
            wp = 0;
    }
}

writePosition += samples;
writePosition %= bufferSize;

//==================================================
// Disable
//==================================================

if (stutterAmount <= 0.01f)
{
    isLooping = false;
    return;
}

//==================================================
// First Capture
//==================================================

if (!isLooping)
{
    isLooping = true;

    currentLoopLength =
        targetLoopLength;

    loopLength =
        (int)currentLoopLength;

    loopEnd = writePosition;

    loopStart =
        loopEnd - loopLength;

    while (loopStart < 0)
        loopStart += bufferSize;

    readPosition =
        loopStart;

    lastLoopLength =
        loopLength;
}

//==================================================
// Refresh
//==================================================

if (refreshMode)
{
    refreshInterval =
        (int)(currentBeatLength / 4.0);

    refreshCounter += samples;

    if (refreshCounter >= refreshInterval)
    {
        refreshCounter = 0;

        loopEnd = writePosition;

        loopStart =
            loopEnd - loopLength;

        while (loopStart < 0)
            loopStart += bufferSize;

        readPosition =
            loopStart;
    }
}

loopEnd =
    loopStart + loopLength;

if (loopEnd >= bufferSize)
    loopEnd -= bufferSize;
   //==================================================
// Playback
//==================================================

for (int ch = 0; ch < channels; ++ch)
{
    auto* output = buffer.getWritePointer(ch);
    auto* ring = circularBuffer.getReadPointer(ch);

    int rp = readPosition;

    for (int i = 0; i < samples; ++i)
    {
        output[i] = ring[rp];

        ++rp;

        if (rp >= bufferSize)
            rp = 0;

        int distance = rp - loopStart;

        if (distance < 0)
            distance += bufferSize;

        if (distance >= loopLength)
            rp = loopStart;
    }

    if (ch == 0)
        readPosition = rp;
}

} // process()