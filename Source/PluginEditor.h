#pragma once

#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/EQPanel.h"
#include "UI/CompressorPanel.h"
#include "UI/SaturationPanel.h"
#include "UI/ReverbPanel.h"
#include "UI/GainMixPanel.h"
#include "UI/MasterBusPanel.h"

class AssistedMixingEditor : public juce::AudioProcessorEditor,
                              private juce::Button::Listener,
                              private juce::ComboBox::Listener,
                              private juce::Label::Listener,
                              private juce::Timer
{
public:
    explicit AssistedMixingEditor(AssistedMixingProcessor&);
    ~AssistedMixingEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    void labelTextChanged(juce::Label* label) override;
    void timerCallback() override;
    void showTab(int index);
    void applyTheme(int themeIndex);
    void rebuildTabBar();

    AssistedMixingProcessor& processorRef;
    CustomLookAndFeel customLnf;

    // Header
    juce::ComboBox genreBox, instrumentBox, themeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> genreAttach, instrumentAttach;
    juce::TextButton applyRuleButton{ "Apply Rule" };

    // Master bus toggle + track name
    juce::ToggleButton masterBusToggle{ "Master" };
    juce::Label trackNameLabel;

    // Tab bar
    enum TabIndex { TabEQ = 0, TabComp, TabSat, TabReverb, TabGainMix, TabMaster, NumTabs };
    std::array<juce::TextButton, NumTabs> tabButtons;
    int activeTab = TabEQ;

    // Panels
    std::unique_ptr<EQPanel> eqPanel;
    std::unique_ptr<CompressorPanel> compPanel;
    std::unique_ptr<SaturationPanel> satPanel;
    std::unique_ptr<ReverbPanel> reverbPanel;
    std::unique_ptr<GainMixPanel> gainMixPanel;
    std::unique_ptr<MasterBusPanel> masterBusPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AssistedMixingEditor)
};
