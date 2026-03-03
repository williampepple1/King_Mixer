#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../DSP/Compressor.h"
#include "CustomLookAndFeel.h"
#include <array>

class CompressorPanel : public juce::Component, private juce::Timer
{
public:
    CompressorPanel(juce::AudioProcessorValueTreeState& apvts, CompressorDSP& comp);
    ~CompressorPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawGRMeter(juce::Graphics& g, juce::Rectangle<int> area);
    void drawGRTimeline(juce::Graphics& g, juce::Rectangle<int> area);
    void drawTransferCurve(juce::Graphics& g, juce::Rectangle<int> area);

    juce::AudioProcessorValueTreeState& apvts;
    CompressorDSP& comp;

    static constexpr int kGRHistorySize = 256;
    std::array<float, kGRHistorySize> grHistory{};
    int grWritePos = 0;

    juce::Slider threshSlider, ratioSlider, attackSlider, releaseSlider, makeupSlider;
    juce::Label lblThresh{"","Threshold"}, lblRatio{"","Ratio"}, lblAttack{"","Attack"};
    juce::Label lblRelease{"","Release"}, lblMakeup{"","Makeup"};

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorPanel)
};
