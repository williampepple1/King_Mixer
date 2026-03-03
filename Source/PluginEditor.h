#pragma once

#include "PluginProcessor.h"

class RotarySliderWithLabel : public juce::Component
{
public:
    RotarySliderWithLabel(const juce::String& labelText);

    void resized() override;
    juce::Slider& getSlider() { return slider; }

private:
    juce::Slider slider;
    juce::Label label;
};

class AssistedMixingEditor : public juce::AudioProcessorEditor,
                              private juce::Button::Listener
{
public:
    explicit AssistedMixingEditor(AssistedMixingProcessor&);
    ~AssistedMixingEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked(juce::Button* button) override;
    void setupSlider(RotarySliderWithLabel& s, juce::AudioProcessorValueTreeState::SliderAttachment*& att,
                     const juce::String& paramId);
    void drawSectionLabel(juce::Graphics& g, int y, const juce::String& text);

    AssistedMixingProcessor& processorRef;

    juce::ComboBox genreBox, instrumentBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> genreAttach, instrumentAttach;

    juce::TextButton applyRuleButton{ "Apply Rule" };

    // Gain
    RotarySliderWithLabel inputGainSlider{ "In" };
    RotarySliderWithLabel outputGainSlider{ "Out" };

    // EQ
    RotarySliderWithLabel eqLowFreqSlider{ "Freq" };
    RotarySliderWithLabel eqLowGainSlider{ "Gain" };
    RotarySliderWithLabel eqLowMidFreqSlider{ "Freq" };
    RotarySliderWithLabel eqLowMidGainSlider{ "Gain" };
    RotarySliderWithLabel eqLowMidQSlider{ "Q" };
    RotarySliderWithLabel eqHighMidFreqSlider{ "Freq" };
    RotarySliderWithLabel eqHighMidGainSlider{ "Gain" };
    RotarySliderWithLabel eqHighMidQSlider{ "Q" };
    RotarySliderWithLabel eqHighFreqSlider{ "Freq" };
    RotarySliderWithLabel eqHighGainSlider{ "Gain" };

    // Compressor
    RotarySliderWithLabel compThreshSlider{ "Thresh" };
    RotarySliderWithLabel compRatioSlider{ "Ratio" };
    RotarySliderWithLabel compAttackSlider{ "Atk" };
    RotarySliderWithLabel compReleaseSlider{ "Rel" };
    RotarySliderWithLabel compMakeupSlider{ "Makeup" };

    // Saturation
    RotarySliderWithLabel satDriveSlider{ "Drive" };
    RotarySliderWithLabel satMixSlider{ "Mix" };

    // Stereo
    RotarySliderWithLabel stereoWidthSlider{ "Width" };

    // Reverb
    RotarySliderWithLabel reverbSendSlider{ "Send" };

    // Mix / Bypass
    RotarySliderWithLabel mixAmountSlider{ "Mix" };
    juce::ToggleButton bypassToggle{ "Bypass" };

    // Attachments
    juce::AudioProcessorValueTreeState::SliderAttachment* inputGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* outputGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqLowFreqAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqLowGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqLowMidFreqAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqLowMidGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqLowMidQAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqHighMidFreqAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqHighMidGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqHighMidQAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqHighFreqAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* eqHighGainAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* compThreshAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* compRatioAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* compAttackAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* compReleaseAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* compMakeupAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* satDriveAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* satMixAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* stereoWidthAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* reverbSendAttach = nullptr;
    juce::AudioProcessorValueTreeState::SliderAttachment* mixAmountAttach = nullptr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AssistedMixingEditor)
};
