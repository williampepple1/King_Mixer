#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <cmath>

class ReverbSend
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void setMix(float pct);
    void setPredelay(float ms);
    void setDecay(float seconds);

    void setDampHighFreq(float hz);
    void setDampHighShelf(float dB);
    void setDampBassFreq(float hz);
    void setDampBassMult(float mult);

    void setSize(float pct);
    void setAttack(float pct);

    void setEarlyDiffusion(float pct);
    void setLateDiffusion(float pct);

    void setModRate(float hz);
    void setModDepth(float pct);

    void setEqHighCut(float hz);
    void setEqLowCut(float hz);

    void setMode(int modeIdx);
    void setColor(int colorIdx);

    float getMix() const { return mixPct; }
    float getPredelay() const { return predelayMs; }
    float getDecay() const { return decaySec; }
    float getDampHighFreq() const { return dampHiFreq; }
    float getDampHighShelf() const { return dampHiShelf; }
    float getDampBassFreq() const { return dampBassFreq; }
    float getDampBassMult() const { return dampBassMult; }
    float getSize() const { return sizePct; }
    float getAttack() const { return attackPct; }
    float getEarlyDiffusion() const { return earlyDiff; }
    float getLateDiffusion() const { return lateDiff; }
    float getModRate() const { return modRateHz; }
    float getModDepth() const { return modDepthPct; }
    float getEqHighCut() const { return eqHighCutHz; }
    float getEqLowCut() const { return eqLowCutHz; }
    int getMode() const { return modeIndex; }
    int getColor() const { return colorIndex; }

private:
    void updateReverbParams();
    void rebuildFilterCoeffs();
    void applyPendingCoeffs();

    double sampleRate = 44100.0;
    bool paramsDirty = true;
    bool coeffsDirty = false;

    float mixPct = 100.0f;
    float predelayMs = 20.0f;
    float decaySec = 4.0f;

    float dampHiFreq = 6000.0f;
    float dampHiShelf = -24.0f;
    float dampBassFreq = 300.0f;
    float dampBassMult = 1.5f;

    float sizePct = 100.0f;
    float attackPct = 50.0f;

    float earlyDiff = 100.0f;
    float lateDiff = 100.0f;

    float modRateHz = 2.53f;
    float modDepthPct = 38.0f;

    float eqHighCutHz = 8000.0f;
    float eqLowCutHz = 10.0f;

    int modeIndex = 0;
    int colorIndex = 0;

    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    // Predelay line
    static constexpr int kMaxPredelaySamples = 96000;
    std::array<float, kMaxPredelaySamples> predelayBufferL{};
    std::array<float, kMaxPredelaySamples> predelayBufferR{};
    int predelayWritePos = 0;
    int predelayDelaySamples = 0;

    // Output EQ filters
    juce::dsp::IIR::Filter<float> highCutFilterL, highCutFilterR;
    juce::dsp::IIR::Filter<float> lowCutFilterL, lowCutFilterR;

    // Damping filters applied to reverb tail
    juce::dsp::IIR::Filter<float> dampHiFilterL, dampHiFilterR;
    juce::dsp::IIR::Filter<float> dampLoFilterL, dampLoFilterR;

    // Pre-computed filter coefficients (computed on message thread)
    juce::dsp::IIR::Coefficients<float>::Ptr pendingHiCut, pendingLoCut, pendingDampHi, pendingDampLo;
    juce::SpinLock coeffsLock;

    // Modulation LFO state
    float lfoPhase = 0.0f;
};
