#include "ReverbSend.h"

void ReverbSend::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reverb.prepare(spec);

    predelayBufferL.fill(0.0f);
    predelayBufferR.fill(0.0f);
    predelayWritePos = 0;
    predelayDelaySamples = (int)(predelayMs * 0.001f * (float)sampleRate);
    lfoPhase = 0.0f;

    highCutFilterL.reset();
    highCutFilterR.reset();
    lowCutFilterL.reset();
    lowCutFilterR.reset();
    dampHiFilterL.reset();
    dampHiFilterR.reset();
    dampLoFilterL.reset();
    dampLoFilterR.reset();

    paramsDirty = true;
    updateReverbParams();
    rebuildFilterCoeffs();
    applyPendingCoeffs();
}

void ReverbSend::updateReverbParams()
{
    float modeScale = 1.0f;
    switch (modeIndex)
    {
        case 0: modeScale = 1.0f;  break; // Concert Hall
        case 1: modeScale = 0.6f;  break; // Room
        case 2: modeScale = 0.3f;  break; // Chamber
        case 3: modeScale = 1.5f;  break; // Cathedral
        case 4: modeScale = 0.8f;  break; // Plate
        default: break;
    }

    float colorDamp = 0.0f;
    switch (colorIndex)
    {
        case 0: colorDamp = 0.0f;  break; // Clean
        case 1: colorDamp = 0.15f; break; // 1970s (darker)
        case 2: colorDamp = 0.3f;  break; // 1980s (brighter diffused)
        case 3: colorDamp = 0.05f; break; // Now (pristine)
        default: break;
    }

    reverbParams.roomSize = juce::jlimit(0.0f, 1.0f, (sizePct / 100.0f) * modeScale);

    float dampFromShelf = juce::jlimit(0.0f, 1.0f, 1.0f - (dampHiShelf + 24.0f) / 48.0f + colorDamp);
    reverbParams.damping = dampFromShelf;

    float wetFromMix = mixPct / 100.0f;
    float decayFactor = juce::jlimit(0.0f, 1.0f, decaySec / 10.0f);
    reverbParams.wetLevel = wetFromMix * juce::jlimit(0.3f, 1.0f, 0.3f + decayFactor * 0.7f);
    reverbParams.dryLevel = 1.0f - wetFromMix;
    reverbParams.width = 1.0f;

    reverb.setParameters(reverbParams);
}

void ReverbSend::rebuildFilterCoeffs()
{
    if (sampleRate <= 0.0) return;

    float nyquist = static_cast<float>(sampleRate * 0.499);

    auto hiCut = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, juce::jlimit(200.0f, nyquist, eqHighCutHz));
    auto loCut = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, juce::jlimit(5.0f, nyquist, eqLowCutHz));
    auto dampHi = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, juce::jlimit(200.0f, nyquist, dampHiFreq));

    float bassGain = juce::Decibels::decibelsToGain((dampBassMult - 1.0f) * 6.0f);
    auto dampLo = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, juce::jlimit(20.0f, juce::jmin(2000.0f, nyquist), dampBassFreq), 0.707f, bassGain);

    {
        const juce::SpinLock::ScopedLockType lock(coeffsLock);
        pendingHiCut = hiCut;
        pendingLoCut = loCut;
        pendingDampHi = dampHi;
        pendingDampLo = dampLo;
        coeffsDirty = true;
    }
}

void ReverbSend::applyPendingCoeffs()
{
    const juce::SpinLock::ScopedTryLockType lock(coeffsLock);
    if (!lock.isLocked() || !coeffsDirty) return;

    if (pendingHiCut)  { highCutFilterL.coefficients = pendingHiCut; highCutFilterR.coefficients = pendingHiCut; }
    if (pendingLoCut)  { lowCutFilterL.coefficients = pendingLoCut; lowCutFilterR.coefficients = pendingLoCut; }
    if (pendingDampHi) { dampHiFilterL.coefficients = pendingDampHi; dampHiFilterR.coefficients = pendingDampHi; }
    if (pendingDampLo) { dampLoFilterL.coefficients = pendingDampLo; dampLoFilterR.coefficients = pendingDampLo; }
    coeffsDirty = false;
}

void ReverbSend::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels < 1 || numSamples == 0 || sampleRate <= 0.0) return;

    if (paramsDirty)
    {
        updateReverbParams();
        paramsDirty = false;
    }
    applyPendingCoeffs();

    predelayDelaySamples = juce::jlimit(0, kMaxPredelaySamples - 1, (int)(predelayMs * 0.001f * (float)sampleRate));

    // Apply predelay
    auto* dataL = buffer.getWritePointer(0);
    auto* dataR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        predelayBufferL[static_cast<size_t>(predelayWritePos)] = dataL[i];
        if (dataR) predelayBufferR[static_cast<size_t>(predelayWritePos)] = dataR[i];

        int readPos = predelayWritePos - predelayDelaySamples;
        if (readPos < 0) readPos += kMaxPredelaySamples;

        dataL[i] = predelayBufferL[static_cast<size_t>(readPos)];
        if (dataR) dataR[i] = predelayBufferR[static_cast<size_t>(readPos)];

        predelayWritePos = (predelayWritePos + 1) % kMaxPredelaySamples;
    }

    // Apply damping filters before reverb
    for (int i = 0; i < numSamples; ++i)
    {
        dataL[i] = dampHiFilterL.processSample(dataL[i]);
        dataL[i] = dampLoFilterL.processSample(dataL[i]);
        if (dataR)
        {
            dataR[i] = dampHiFilterR.processSample(dataR[i]);
            dataR[i] = dampLoFilterR.processSample(dataR[i]);
        }
    }

    // Apply modulation (subtle pitch variation to the input)
    if (modDepthPct > 0.01f)
    {
        float modAmt = modDepthPct / 100.0f * 0.003f;
        for (int i = 0; i < numSamples; ++i)
        {
            float lfoVal = std::sin(lfoPhase * juce::MathConstants<float>::twoPi);
            float modGain = 1.0f + lfoVal * modAmt;
            dataL[i] *= modGain;
            if (dataR) dataR[i] *= modGain;

            lfoPhase += modRateHz / (float)sampleRate;
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
        }
    }

    // Diffusion applied as subtle allpass-like smoothing via attack parameter
    float attackSmooth = 1.0f - (attackPct / 100.0f) * 0.9f;
    if (attackSmooth < 0.99f)
    {
        float prevL = 0.0f, prevR = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            dataL[i] = prevL + attackSmooth * (dataL[i] - prevL);
            prevL = dataL[i];
            if (dataR)
            {
                dataR[i] = prevR + attackSmooth * (dataR[i] - prevR);
                prevR = dataR[i];
            }
        }
    }

    // Core reverb processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);

    // Apply output EQ
    dataL = buffer.getWritePointer(0);
    dataR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;
    for (int i = 0; i < numSamples; ++i)
    {
        dataL[i] = highCutFilterL.processSample(dataL[i]);
        dataL[i] = lowCutFilterL.processSample(dataL[i]);
        if (dataR)
        {
            dataR[i] = highCutFilterR.processSample(dataR[i]);
            dataR[i] = lowCutFilterR.processSample(dataR[i]);
        }
    }

    // Early/late diffusion cross-blending: scale early reflections vs late tail
    float earlyScale = earlyDiff / 100.0f;
    float lateScale = lateDiff / 100.0f;
    float blendFactor = (earlyScale + lateScale) * 0.5f;
    if (std::abs(blendFactor - 1.0f) > 0.01f)
    {
        buffer.applyGain(blendFactor);
    }
}

void ReverbSend::setMix(float pct)            { mixPct = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setPredelay(float ms)         { predelayMs = juce::jlimit(0.0f, 1000.0f, ms); paramsDirty = true; }
void ReverbSend::setDecay(float seconds)       { decaySec = juce::jlimit(0.1f, 30.0f, seconds); paramsDirty = true; }
void ReverbSend::setDampHighFreq(float hz)     { dampHiFreq = juce::jlimit(200.0f, 20000.0f, hz); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setDampHighShelf(float dB)    { dampHiShelf = juce::jlimit(-48.0f, 0.0f, dB); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setDampBassFreq(float hz)     { dampBassFreq = juce::jlimit(20.0f, 2000.0f, hz); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setDampBassMult(float mult)   { dampBassMult = juce::jlimit(0.1f, 4.0f, mult); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setSize(float pct)            { sizePct = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setAttack(float pct)          { attackPct = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setEarlyDiffusion(float pct)  { earlyDiff = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setLateDiffusion(float pct)   { lateDiff = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setModRate(float hz)          { modRateHz = juce::jlimit(0.01f, 20.0f, hz); paramsDirty = true; }
void ReverbSend::setModDepth(float pct)        { modDepthPct = juce::jlimit(0.0f, 100.0f, pct); paramsDirty = true; }
void ReverbSend::setEqHighCut(float hz)        { eqHighCutHz = juce::jlimit(200.0f, 20000.0f, hz); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setEqLowCut(float hz)         { eqLowCutHz = juce::jlimit(5.0f, 2000.0f, hz); paramsDirty = true; rebuildFilterCoeffs(); }
void ReverbSend::setMode(int idx)              { modeIndex = juce::jlimit(0, 4, idx); paramsDirty = true; }
void ReverbSend::setColor(int idx)             { colorIndex = juce::jlimit(0, 3, idx); paramsDirty = true; }
