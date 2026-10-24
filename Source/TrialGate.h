#pragma once
#include <algorithm>
#include <cmath>

// Lifetime belongs to a processor instance, never to a preset or prepareToPlay.
class TrialGate
{
public:
    static constexpr double durationSeconds = 15.0 * 60.0;
    bool consume(int samples, double sampleRate) noexcept
    {
        if(samples>0 && std::isfinite(sampleRate) && sampleRate>0)
            elapsed=std::min(durationSeconds,elapsed+samples/sampleRate);
        return elapsed<durationSeconds-1.0e-9;
    }
    double remaining() const noexcept { return std::max(0.0,durationSeconds-elapsed); }
private:
    double elapsed=0.0;
};
