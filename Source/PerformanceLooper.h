#pragma once
#include <JuceHeader.h>
#include <array>

// Updated DSP behind the original Stutter knob; no separate activation mode.
class PerformanceLooper
{
public:
    void prepare(double sampleRate, int channels);
    void reset();
    void setTransport(double bpm, double ppq, bool hasPpq, bool playing);
    void requestRecapture() noexcept { recapture = true; }
    void process(juce::AudioBuffer<float>&, int start, int count,
                 float stutter, float intensity, bool sync, float beats, bool refresh, bool render = true);
    int getCaptureCount() const noexcept { return captures; }

private:
    struct Voice { int bank = 0, length = 0, slice = 0, phase = 0; };
    float read(const Voice&, int channel) const noexcept;
    void advance(Voice&) noexcept;
    void beginCapture(int length);
    double sr = 44100.0, tempo = 120.0, quarter = 0.0;
    bool validPpq = false, playing = false, wasPlaying = false;
    bool recapture = false, wanted = false, copying = false, ready = false;
    int capacity = 0, channels = 0, write = 0, filled = 0;
    int copyStart = 0, copyLength = 0, copied = 0, targetBank = 0;
    int fadeLength = 1, transition = 0, captures = 0;
    int divisionIndex = 0;
    double refreshPhase = 0.0;
    float activation = 0.0f;
    juce::AudioBuffer<float> history;
    std::array<juce::AudioBuffer<float>, 2> loops;
    Voice current, previous;
};
