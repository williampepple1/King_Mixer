#include "ParametricEQ.h"

void ParametricEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    for (auto& b : bands)
        b.filter.prepare(spec);

    // Initialize default band states
    bandStates[0] = { 80.0f,   0.0f, 0.707f, (int)EQFilterType::LowShelf,  true };
    bandStates[1] = { 250.0f,  0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[2] = { 700.0f,  0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[3] = { 1500.0f, 0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[4] = { 3000.0f, 0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[5] = { 5000.0f, 0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[6] = { 8000.0f, 0.0f, 1.0f,   (int)EQFilterType::Peak,      true };
    bandStates[7] = { 12000.0f,0.0f, 0.707f, (int)EQFilterType::HighShelf, true };

    for (int i = 0; i < kMaxBands; ++i)
    {
        auto c = makeCoeffs(static_cast<EQFilterType>(bandStates[i].filterType),
                            spec.sampleRate, bandStates[i].frequency, bandStates[i].gainDB, bandStates[i].q);
        if (c)
        {
            bands[i].coeffs = c;
            if (bands[i].filter.state) *bands[i].filter.state = *c;
        }
        bands[i].active = bandStates[i].enabled;
    }
}

ParametricEQ::Coeffs::Ptr ParametricEQ::makeCoeffs(EQFilterType type, double sr, float freq, float gain, float q) const
{
    float f = juce::jlimit(20.0f, (float)(sr * 0.499), freq);
    float qVal = juce::jmax(0.05f, q);
    float linGain = juce::Decibels::decibelsToGain(gain);

    switch (type)
    {
        case EQFilterType::Peak:      return Coeffs::makePeakFilter(sr, f, qVal, linGain);
        case EQFilterType::LowShelf:  return Coeffs::makeLowShelf(sr, f, qVal, linGain);
        case EQFilterType::HighShelf: return Coeffs::makeHighShelf(sr, f, qVal, linGain);
        case EQFilterType::LowCut:    return Coeffs::makeHighPass(sr, f, qVal);
        case EQFilterType::HighCut:   return Coeffs::makeLowPass(sr, f, qVal);
        case EQFilterType::BandPass:  return Coeffs::makeBandPass(sr, f, qVal);
        case EQFilterType::Notch:     return Coeffs::makeNotch(sr, f, qVal);
        default:                      return Coeffs::makePeakFilter(sr, f, qVal, linGain);
    }
}

void ParametricEQ::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    const juce::SpinLock::ScopedLockType lock(stateLock);
    for (int i = 0; i < kMaxBands; ++i)
    {
        if (bands[i].active && bands[i].coeffs)
            bands[i].filter.process(context);
    }
}

void ParametricEQ::updateBand(int index, float freq, float gain, float q, int type, bool enabled, double sampleRate)
{
    if (index < 0 || index >= kMaxBands || sampleRate <= 0.0)
        return;

    currentSampleRate = sampleRate;

    auto c = makeCoeffs(static_cast<EQFilterType>(type), sampleRate, freq, gain, q);

    {
        const juce::SpinLock::ScopedLockType lock(stateLock);
        auto& st = bandStates[static_cast<size_t>(index)];
        st.frequency = freq;
        st.gainDB = gain;
        st.q = q;
        st.filterType = type;
        st.enabled = enabled;

        if (c)
        {
            bands[static_cast<size_t>(index)].coeffs = c;
            if (bands[static_cast<size_t>(index)].filter.state)
                *bands[static_cast<size_t>(index)].filter.state = *c;
        }
        bands[static_cast<size_t>(index)].active = enabled;
    }
}

void ParametricEQ::updateBands(float lowFreq, float lowGain,
                                float lowMidFreq, float lowMidGain, float lowMidQ,
                                float highMidFreq, float highMidGain, float highMidQ,
                                float highFreq, float highGain,
                                double sampleRate)
{
    updateBand(0, lowFreq, lowGain, 0.707f, (int)EQFilterType::LowShelf, true, sampleRate);
    updateBand(1, lowMidFreq, lowMidGain, lowMidQ, (int)EQFilterType::Peak, true, sampleRate);
    updateBand(2, highMidFreq, highMidGain, highMidQ, (int)EQFilterType::Peak, true, sampleRate);
    updateBand(3, highFreq, highGain, 0.707f, (int)EQFilterType::HighShelf, true, sampleRate);
}

void ParametricEQ::getMagnitudeResponse(const double* frequencies, double* magnitudes, int numPoints, double sampleRate) const
{
    if (!frequencies || !magnitudes || numPoints <= 0)
        return;

    for (int i = 0; i < numPoints; ++i)
        magnitudes[i] = 1.0;

    if (sampleRate <= 0.0)
        return;

    const juce::SpinLock::ScopedLockType lock(stateLock);
    for (int b = 0; b < kMaxBands; ++b)
    {
        if (!bands[b].active || !bands[b].coeffs)
            continue;
        for (int i = 0; i < numPoints; ++i)
            magnitudes[i] *= bands[b].coeffs->getMagnitudeForFrequency(frequencies[i], sampleRate);
    }
}

void ParametricEQ::getBandMagnitudeResponse(int bandIndex, const double* frequencies, double* magnitudes, int numPoints, double sampleRate) const
{
    if (!frequencies || !magnitudes || numPoints <= 0)
        return;

    for (int i = 0; i < numPoints; ++i)
        magnitudes[i] = 1.0;

    if (bandIndex < 0 || bandIndex >= kMaxBands || sampleRate <= 0.0)
        return;

    const juce::SpinLock::ScopedLockType lock(stateLock);
    if (!bands[bandIndex].active || !bands[bandIndex].coeffs)
        return;

    for (int i = 0; i < numPoints; ++i)
        magnitudes[i] = bands[bandIndex].coeffs->getMagnitudeForFrequency(frequencies[i], sampleRate);
}
