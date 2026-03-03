#pragma once

#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/EQPanel.h"
#include "UI/CompressorPanel.h"
#include "UI/SaturationPanel.h"
#include "UI/ReverbPanel.h"
#include "UI/GainMixPanel.h"

class AssistedMixingEditor : public juce::AudioProcessorEditor,
                              private juce::Button::Listener,
                              private juce::ComboBox::Listener
{
public:
    explicit AssistedMixingEditor(AssistedMixingProcessor&);
    ~AssistedMixingEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    void showTab(int index);
    void applyTheme(int themeIndex);

    AssistedMixingProcessor& processorRef;
    CustomLookAndFeel customLnf;

    // Header
    juce::ComboBox genreBox, instrumentBox, themeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> genreAttach, instrumentAttach;
    juce::TextButton applyRuleButton{ "Apply Rule" };

    // Tab bar
    enum TabIndex { TabEQ = 0, TabComp, TabSat, TabReverb, TabGainMix, NumTabs };
    std::array<juce::TextButton, NumTabs> tabButtons;
    int activeTab = TabEQ;

    // Panels
    std::unique_ptr<EQPanel> eqPanel;
    std::unique_ptr<CompressorPanel> compPanel;
    std::unique_ptr<SaturationPanel> satPanel;
    std::unique_ptr<ReverbPanel> reverbPanel;
    std::unique_ptr<GainMixPanel> gainMixPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AssistedMixingEditor)
};
