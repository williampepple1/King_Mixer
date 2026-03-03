#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Analysis/LevelMeter.h"
#include "CustomLookAndFeel.h"

class GainMixPanel : public juce::Component, private juce::Timer
{
public:
    GainMixPanel(juce::AudioProcessorValueTreeState& apvts,
                 LevelMeterData& inputMeter, LevelMeterData& outputMeter);
    ~GainMixPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawStereoMeter(juce::Graphics& g, juce::Rectangle<int> area,
                         float peakL, float peakR, float rmsL, float rmsR,
                         const juce::String& label);
    void drawCorrelationMeter(juce::Graphics& g, juce::Rectangle<int> area);

    juce::AudioProcessorValueTreeState& apvts;
    LevelMeterData& inputMeter;
    LevelMeterData& outputMeter;

    float inPeakL = 0, inPeakR = 0, inRmsL = 0, inRmsR = 0;
    float outPeakL = 0, outPeakR = 0, outRmsL = 0, outRmsR = 0;
    float peakHoldL = 0, peakHoldR = 0;
    float peakHoldDecay = 0.997f;

    juce::Slider inputGainSlider, outputGainSlider, stereoWidthSlider, mixAmountSlider;
    juce::ToggleButton bypassToggle{ "Bypass" };
    juce::Label lblInGain{"","Input"}, lblOutGain{"","Output"}, lblWidth{"","Width"}, lblMix{"","Mix"};

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainMixPanel)
};
