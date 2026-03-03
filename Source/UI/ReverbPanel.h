#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Analysis/WaveformBuffer.h"
#include "CustomLookAndFeel.h"

class ReverbPanel : public juce::Component, private juce::Timer
{
public:
    ReverbPanel(juce::AudioProcessorValueTreeState& apvts,
                WaveformBuffer& dryBuf, WaveformBuffer& wetBuf);
    ~ReverbPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawDecayEnvelope(juce::Graphics& g, juce::Rectangle<int> area);
    void drawWaveformOverlay(juce::Graphics& g, juce::Rectangle<int> area);

    juce::AudioProcessorValueTreeState& apvts;
    WaveformBuffer& dryBuf;
    WaveformBuffer& wetBuf;

    std::array<float, WaveformBuffer::bufferSize> dryData{};
    std::array<float, WaveformBuffer::bufferSize> wetData{};

    juce::Slider sendSlider, roomSizeSlider, dampingSlider;
    juce::Label lblSend{"","Send"}, lblRoomSize{"","Room Size"}, lblDamping{"","Damping"};

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbPanel)
};
