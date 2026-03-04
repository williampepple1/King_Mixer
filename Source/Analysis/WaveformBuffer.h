#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <array>
#include <atomic>

class WaveformBuffer
{
public:
    static constexpr int bufferSize = 1024;

    void pushSamples(const float* data, int numSamples);
    void copyTo(std::array<float, bufferSize>& dest) const;

private:
    std::array<float, bufferSize> writeBuffer{};
    std::array<float, bufferSize> readBuffer{};
    int writePos = 0;
    mutable juce::SpinLock snapLock;
};
