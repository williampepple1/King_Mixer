#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "DSP/GainStage.h"
#include "DSP/ParametricEQ.h"
#include "DSP/Compressor.h"
#include "DSP/Saturation.h"
#include "DSP/StereoWidth.h"
#include "DSP/ReverbSend.h"
#include "Rules/GenreInstrumentDefs.h"
#include "Rules/MixRuleDatabase.h"
#include "Analysis/SpectrumAnalyzer.h"
#include "Analysis/LevelMeter.h"
#include "Analysis/WaveformBuffer.h"

class AssistedMixingProcessor : public juce::AudioProcessor
{
public:
    AssistedMixingProcessor();
    ~AssistedMixingProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    void applyRule(Genre genre, Instrument instrument);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    SpectrumAnalyzer& getPreEQAnalyzer()  { return preEQAnalyzer; }
    SpectrumAnalyzer& getPostEQAnalyzer() { return postEQAnalyzer; }
    LevelMeterData& getInputMeter()  { return inputMeter; }
    LevelMeterData& getOutputMeter() { return outputMeter; }
    CompressorDSP& getCompressor() { return compressor; }
    ParametricEQ& getParametricEQ() { return parametricEQ; }
    WaveformBuffer& getPreSatBuffer()  { return preSatBuffer; }
    WaveformBuffer& getPostSatBuffer() { return postSatBuffer; }
    WaveformBuffer& getDryRevBuffer()  { return dryRevBuffer; }
    WaveformBuffer& getWetRevBuffer()  { return wetRevBuffer; }
    ReverbSend& getReverbSend() { return reverbSend; }

    int getThemeIndex() const { return themeIndex.load(); }
    void setThemeIndex(int idx) { themeIndex.store(idx); }

private:
    juce::AudioProcessorValueTreeState apvts;

    GainStage gainStage;
    ParametricEQ parametricEQ;
    CompressorDSP compressor;
    Saturation saturation;
    StereoWidthDSP stereoWidth;
    ReverbSend reverbSend;

    SpectrumAnalyzer preEQAnalyzer;
    SpectrumAnalyzer postEQAnalyzer;
    LevelMeterData inputMeter;
    LevelMeterData outputMeter;
    WaveformBuffer preSatBuffer;
    WaveformBuffer postSatBuffer;
    WaveformBuffer dryRevBuffer;
    WaveformBuffer wetRevBuffer;

    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* outputGainParam = nullptr;
    struct EQBandParams {
        std::atomic<float>* freq = nullptr;
        std::atomic<float>* gain = nullptr;
        std::atomic<float>* q = nullptr;
        std::atomic<float>* type = nullptr;
        std::atomic<float>* enabled = nullptr;
    };
    std::array<EQBandParams, 8> eqBandParams;
    std::atomic<float>* compThresholdParam = nullptr;
    std::atomic<float>* compRatioParam = nullptr;
    std::atomic<float>* compAttackParam = nullptr;
    std::atomic<float>* compReleaseParam = nullptr;
    std::atomic<float>* compMakeupParam = nullptr;
    std::atomic<float>* satDriveParam = nullptr;
    std::atomic<float>* satMixParam = nullptr;
    std::atomic<float>* stereoWidthParam = nullptr;
    std::atomic<float>* revMixParam = nullptr;
    std::atomic<float>* revPredelayParam = nullptr;
    std::atomic<float>* revDecayParam = nullptr;
    std::atomic<float>* revDampHiFreqParam = nullptr;
    std::atomic<float>* revDampHiShelfParam = nullptr;
    std::atomic<float>* revDampBassFreqParam = nullptr;
    std::atomic<float>* revDampBassMultParam = nullptr;
    std::atomic<float>* revSizeParam = nullptr;
    std::atomic<float>* revAttackParam = nullptr;
    std::atomic<float>* revEarlyDiffParam = nullptr;
    std::atomic<float>* revLateDiffParam = nullptr;
    std::atomic<float>* revModRateParam = nullptr;
    std::atomic<float>* revModDepthParam = nullptr;
    std::atomic<float>* revEqHighCutParam = nullptr;
    std::atomic<float>* revEqLowCutParam = nullptr;
    std::atomic<float>* revModeParam = nullptr;
    std::atomic<float>* revColorParam = nullptr;
    std::atomic<float>* mixAmountParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    std::atomic<int> themeIndex{ 0 };

    juce::AudioBuffer<float> dryBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AssistedMixingProcessor)
};
