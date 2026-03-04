#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>

class SpectrumAnalyzer
{
public:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder; // 2048
    static constexpr int scopeSize = 512;

    SpectrumAnalyzer();

    void pushSamples(const float* data, int numSamples);
    bool getNextBlock(std::array<float, scopeSize>& output);
    void setSampleRate(double sr) { sampleRate = sr; }
    double getSampleRate() const { return sampleRate; }

private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, fftSize * 2> fftStaging{};
    std::array<float, fftSize * 2> fftReady{};
    std::array<float, fftSize * 2> fftWork{};
    int fifoIndex = 0;
    std::array<float, fftSize> fifo{};
    juce::SpinLock stagingLock;
    std::atomic<bool> nextBlockReady{ false };
    double sampleRate = 44100.0;
};
