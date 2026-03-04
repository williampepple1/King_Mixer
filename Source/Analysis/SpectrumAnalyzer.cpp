#include "SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer()
    : forwardFFT(fftOrder),
      window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
}

void SpectrumAnalyzer::pushSamples(const float* data, int numSamples)
{
    if (!data || numSamples <= 0) return;

    for (int i = 0; i < numSamples; ++i)
    {
        fifo[static_cast<size_t>(fifoIndex)] = data[i];
        ++fifoIndex;

        if (fifoIndex >= fftSize)
        {
            std::copy(fifo.begin(), fifo.end(), fftStaging.begin());
            std::fill(fftStaging.begin() + fftSize, fftStaging.end(), 0.0f);
            {
                const juce::SpinLock::ScopedLockType lock(stagingLock);
                std::copy(fftStaging.begin(), fftStaging.end(), fftReady.begin());
            }
            nextBlockReady.store(true, std::memory_order_release);
            fifoIndex = 0;
        }
    }
}

bool SpectrumAnalyzer::getNextBlock(std::array<float, scopeSize>& output)
{
    if (!nextBlockReady.exchange(false, std::memory_order_acq_rel))
        return false;

    {
        const juce::SpinLock::ScopedLockType lock(stagingLock);
        std::copy(fftReady.begin(), fftReady.end(), fftWork.begin());
    }

    window.multiplyWithWindowingTable(fftWork.data(), fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftWork.data());

    const float minDB = -80.0f;
    const float maxDB = 0.0f;
    const float rangeDB = maxDB - minDB;

    for (int i = 0; i < scopeSize; ++i)
    {
        float proportion = static_cast<float>(i) / static_cast<float>(scopeSize);
        int fftIndex = static_cast<int>(std::pow(2.0f, proportion * (std::log2(static_cast<float>(fftSize) / 2.0f))));
        fftIndex = juce::jmin(fftIndex, fftSize / 2 - 1);
        fftIndex = juce::jmax(fftIndex, 0);

        float magnitude = fftWork[static_cast<size_t>(fftIndex)];
        if (!std::isfinite(magnitude)) magnitude = 0.0f;
        float level = juce::jlimit(minDB, maxDB,
            juce::Decibels::gainToDecibels(magnitude / static_cast<float>(fftSize)));
        output[static_cast<size_t>(i)] = juce::jlimit(0.0f, 1.0f, (level - minDB) / rangeDB);
    }
    return true;
}
