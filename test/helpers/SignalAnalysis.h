#pragma once

#include <cmath>
#include <cstddef>

/// Compute the root-mean-square of @p nSamples output from @p obj.
template<typename T>
float rms(T& obj, int nSamples)
{
    double sum = 0.0;
    for (int i = 0; i < nSamples; ++i) {
        float s = obj.tick();
        sum += static_cast<double>(s) * static_cast<double>(s);
    }
    return static_cast<float>(std::sqrt(sum / nSamples));
}

/// Return the peak absolute value across @p nSamples output from @p obj.
template<typename T>
float maxAbs(T& obj, int nSamples)
{
    float peak = 0.0f;
    for (int i = 0; i < nSamples; ++i) {
        float s = obj.tick();
        float a = std::fabs(s);
        if (a > peak) peak = a;
    }
    return peak;
}

/// Return true if every sample produced by @p obj over @p nSamples is finite
/// (neither NaN nor infinity).
template<typename T>
bool allFinite(T& obj, int nSamples)
{
    for (int i = 0; i < nSamples; ++i) {
        float s = obj.tick();
        if (!std::isfinite(s)) return false;
    }
    return true;
}

/// Count the number of sign changes (zero crossings) in @p nSamples output
/// from @p obj.  A crossing is counted each time consecutive non-zero samples
/// have opposite signs.
template<typename T>
int zeroCrossings(T& obj, int nSamples)
{
    int count = 0;
    float prev = obj.tick();
    for (int i = 1; i < nSamples; ++i) {
        float curr = obj.tick();
        if (prev * curr < 0.0f) ++count;
        prev = curr;
    }
    return count;
}

/// Tick @p obj @p nSamples times without recording any output.
template<typename T>
void advance(T& obj, int nSamples)
{
    for (int i = 0; i < nSamples; ++i)
        obj.tick();
}
