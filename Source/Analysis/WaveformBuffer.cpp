#include "WaveformBuffer.h"

void WaveformBuffer::pushSamples(const float* data, int numSamples)
{
    if (!data || numSamples <= 0) return;

    for (int i = 0; i < numSamples; ++i)
    {
        writeBuffer[static_cast<size_t>(writePos)] = data[i];
        writePos = (writePos + 1) % bufferSize;
    }

    {
        const juce::SpinLock::ScopedLockType lock(snapLock);
        for (int i = 0; i < bufferSize; ++i)
            readBuffer[static_cast<size_t>(i)] = writeBuffer[static_cast<size_t>((writePos + i) % bufferSize)];
    }
}

void WaveformBuffer::copyTo(std::array<float, bufferSize>& dest) const
{
    const juce::SpinLock::ScopedLockType lock(snapLock);
    std::copy(readBuffer.begin(), readBuffer.end(), dest.begin());
}
