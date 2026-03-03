#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Analysis/WaveformBuffer.h"
#include "CustomLookAndFeel.h"

class SaturationPanel : public juce::Component, private juce::Timer
{
public:
    SaturationPanel(juce::AudioProcessorValueTreeState& apvts,
                    WaveformBuffer& preBuf, WaveformBuffer& postBuf);
    ~SaturationPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawTransferCurve(juce::Graphics& g, juce::Rectangle<int> area);
    void drawWaveformComparison(juce::Graphics& g, juce::Rectangle<int> area);

    juce::AudioProcessorValueTreeState& apvts;
    WaveformBuffer& preBuf;
    WaveformBuffer& postBuf;

    std::array<float, WaveformBuffer::bufferSize> preData{};
    std::array<float, WaveformBuffer::bufferSize> postData{};

    juce::Slider driveSlider, mixSlider;
    juce::Label lblDrive{"","Drive"}, lblMix{"","Mix"};

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturationPanel)
};
