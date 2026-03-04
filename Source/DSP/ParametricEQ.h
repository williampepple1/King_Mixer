#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <array>
#include <mutex>

enum class EQFilterType
{
    Peak = 0,
    LowShelf,
    HighShelf,
    LowCut,
    HighCut,
    BandPass,
    Notch,
    NumTypes
};

struct EQBandState
{
    float frequency = 1000.0f;
    float gainDB = 0.0f;
    float q = 1.0f;
    int filterType = 0;
    bool enabled = true;
};

class ParametricEQ
{
public:
    static constexpr int kMaxBands = 8;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void updateBand(int index, float freq, float gain, float q, int type, bool enabled, double sampleRate);

    void getMagnitudeResponse(const double* frequencies, double* magnitudes, int numPoints, double sampleRate) const;
    void getBandMagnitudeResponse(int bandIndex, const double* frequencies, double* magnitudes, int numPoints, double sampleRate) const;

    double getCurrentSampleRate() const { return currentSampleRate; }

    EQBandState getBandState(int index) const
    {
        int i = juce::jlimit(0, kMaxBands - 1, index);
        const juce::SpinLock::ScopedLockType lock(stateLock);
        return bandStates[static_cast<size_t>(i)];
    }

    // Backward-compat wrapper used by processBlock
    void updateBands(float lowFreq, float lowGain,
                     float lowMidFreq, float lowMidGain, float lowMidQ,
                     float highMidFreq, float highMidGain, float highMidQ,
                     float highFreq, float highGain,
                     double sampleRate);

private:
    using Coeffs = juce::dsp::IIR::Coefficients<float>;
    using Filter = juce::dsp::IIR::Filter<float>;

    struct BandProcessor
    {
        juce::dsp::ProcessorDuplicator<Filter, Coeffs> filter;
        Coeffs::Ptr coeffs;
        bool active = false;
    };

    std::array<BandProcessor, kMaxBands> bands;
    std::array<EQBandState, kMaxBands> bandStates;
    mutable juce::SpinLock stateLock;
    double currentSampleRate = 44100.0;

    Coeffs::Ptr makeCoeffs(EQFilterType type, double sampleRate, float freq, float gain, float q) const;
};
