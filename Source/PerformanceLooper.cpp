#include "PerformanceLooper.h"
#include <cmath>

void PerformanceLooper::prepare(double sampleRate, int numChannels)
{
    sr = sampleRate;
    channels = numChannels;
    capacity = static_cast<int>(sr * 16.0);
    history.setSize(channels, capacity);
    for (auto& loop : loops) loop.setSize(channels, capacity / 2);
    fadeLength = juce::jmax(1, static_cast<int>(sr * 0.005));
    reset();
}

void PerformanceLooper::reset()
{
    history.clear();
    for (auto& loop : loops) loop.clear();
    write = filled = copied = captures = transition = 0;
    copying = ready = recapture = wanted = false;
    activation = 0.0f;
    current = {}; previous = {};
    refreshPhase = 0.0;
    divisionIndex = 0;
}

void PerformanceLooper::setTransport(double bpm, double ppq, bool hasPpq, bool isPlaying)
{
    if (std::isfinite(bpm) && bpm >= 20.0 && bpm <= 999.0) tempo = bpm;
    if (hasPpq && std::isfinite(ppq))
    {
        // A seek invalidates the musical phase of any pending capture.
        if (validPpq && std::abs(ppq - quarter) > 0.125)
            recapture = true;
        quarter = ppq;
    }
    validPpq = hasPpq && std::isfinite(ppq);
    wasPlaying = playing;
    playing = isPlaying;
    if (playing != wasPlaying) recapture = true;
}

void PerformanceLooper::beginCapture(int length)
{
    copyLength = juce::jlimit(1, capacity / 2, length);
    copyStart = (write - copyLength + capacity) % capacity;
    copied = 0;
    targetBank = 1 - current.bank;
    copying = true;
    ready = false;
    recapture = false;
}

float PerformanceLooper::read(const Voice& voice, int channel) const noexcept
{
    if (voice.length == 0 || voice.slice == 0) return 0.0f;
    const auto* data = loops[voice.bank].getReadPointer(channel);
    const int offset = (voice.length - voice.slice) % voice.length;
    const int pos = (offset + voice.phase) % voice.length;
    // Blend both sides of the seam toward a common midpoint. The loop
    // retains its exact musical period, including the crossfade frames.
    const int overlap = juce::jmin(fadeLength, voice.slice / 4);
    const float seam = (data[offset] + data[(offset + voice.slice - 1) % voice.length]) * 0.5f;
    if (overlap > 0 && voice.phase < overlap)
    {
        const float t = static_cast<float>(voice.phase) / overlap;
        const float mix = t*t*(3.0f-2.0f*t);
        return seam * (1.0f - mix) + data[pos] * mix;
    }
    if (overlap > 0 && voice.phase >= voice.slice - overlap)
    {
        const int fadePos = voice.phase - (voice.slice - overlap);
        const float t = static_cast<float>(fadePos + 1) / static_cast<float>(overlap);
        const float mix = t*t*(3.0f-2.0f*t);
        return data[pos] * (1.0f - mix) + seam * mix;
    }
    return data[pos];
}

void PerformanceLooper::advance(Voice& voice) noexcept
{
    if (voice.slice == 0) return;
    if (++voice.phase >= voice.slice) voice.phase = 0;
}

void PerformanceLooper::process(juce::AudioBuffer<float>& buffer, int start, int count,
                               float stutter, float intensity, bool sync, float beats, bool refresh, bool render)
{
    // Deadband prevents controller noise from repeatedly capturing at zero.
    const bool active = stutter > (wanted ? 0.0075f : 0.015f);
    if (active && !wanted)
        divisionIndex = juce::jlimit(0,5,static_cast<int>(stutter*5.999f));
    if (active && !wanted) recapture = true;
    wanted = active;
    const double samplesPerBeat = sr * 60.0 / tempo;
    const int captureLength = juce::jlimit(64, capacity / 2,
        static_cast<int>(samplesPerBeat * beats));
    const double step = 1.0 / samplesPerBeat;
    const float fadeStep = 1.0f / static_cast<float>(fadeLength);
    const int divisions[] = { 1, 2, 4, 8, 16, 32 };
    // Hysteresis avoids flutter between two rates near a knob detent.
    while(divisionIndex<5 && stutter>(divisionIndex+1)/6.0f+0.015f)++divisionIndex;
    while(divisionIndex>0 && stutter<divisionIndex/6.0f-0.015f)--divisionIndex;
    const int division = divisions[divisionIndex];
    const int requestedSlice = juce::jmax(64, static_cast<int>(captureLength /
        (sync ? static_cast<double>(division) : 1.0 + 31.0 * stutter * stutter)));
    if (wanted && current.length > 0 && current.length != captureLength && !copying && !ready)
        recapture = true;

    for (int i = start; i < start + count; ++i)
    {
        // Snapshot work is bounded to 64 frames per sample. A trigger takes
        // length/64 frames to prepare, then commits on the next sixteenth.
        // History is twice the maximum capture length, so copying cannot
        // race the recording write head.
        if (wanted && !copying && !ready && transition == 0 && recapture && filled >= captureLength)
            beginCapture(captureLength);
        if (copying)
        {
            const int n = juce::jmin(64, copyLength - copied);
            for (int ch = 0; ch < channels; ++ch)
                for (int k = 0; k < n; ++k)
                    loops[targetBank].setSample(ch, copied + k,
                        history.getSample(ch, (copyStart + copied + k) % capacity));
            copied += n;
            if (copied == copyLength) { copying = false; ready = true; }
        }

        const double gridPhase = quarter * 4.0;
        const bool boundary = std::floor(gridPhase + 1.0e-8) > std::floor(gridPhase - step * 4.0 + 1.0e-8);
        if (wanted && ready && (!sync || !validPpq || !playing || boundary))
        {
            previous = current;
            current = { targetBank, copyLength, juce::jmin(copyLength, requestedSlice), 0 };
            transition = previous.length > 0 ? fadeLength : 0;
            ready = false;
            ++captures;
        }
        // Slice changes are crossfaded between two read heads in the same bank.
        if (wanted && current.length > 0 && transition == 0 && !copying && !ready
            && (!sync || !validPpq || !playing || boundary))
        {
            const int nextSlice = juce::jmin(current.length, requestedSlice);
            if (nextSlice != current.slice)
            {
                previous = current;
                current.slice = nextSlice;
                current.phase %= nextSlice;
                transition = fadeLength;
            }
        }
        const float target = wanted && current.length > 0 ? juce::jlimit(0.0f, 1.0f, intensity) : 0.0f;
        activation += juce::jlimit(-fadeStep, fadeStep, target - activation);
        for (int ch = 0; ch < channels; ++ch)
        {
            const float dry = buffer.getSample(ch, i);
            history.setSample(ch, write, dry);
            float wet = read(current, ch);
            if (transition > 0)
            {
                const float t = 1.0f - static_cast<float>(transition) / fadeLength;
                const float mix = t*t*(3.0f-2.0f*t);
                wet = read(previous, ch) * (1.0f - mix) + wet * mix;
            }
            if (render) buffer.setSample(ch, i, dry + activation * (wet - dry));
        }
        if (++write >= capacity) write = 0;
        filled = juce::jmin(capacity, filled + 1);
        advance(current);
        if (transition > 0) { advance(previous); --transition; }
        if (!wanted && activation <= 0.0f)
        {
            current.length = 0;
            copying = ready = false;
        }
        refreshPhase += step;
        if (refreshPhase >= beats)
        {
            refreshPhase = std::fmod(refreshPhase, static_cast<double>(beats));
            if (refresh && wanted) recapture = true;
        }
        quarter += step;
    }
}
