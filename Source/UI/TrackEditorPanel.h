#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../IPC/InstanceHub.h"
#include "CustomLookAndFeel.h"

class TrackEditorPanel : public juce::Component,
                         private juce::Timer,
                         private juce::Slider::Listener,
                         private juce::ComboBox::Listener,
                         private juce::Button::Listener
{
public:
    TrackEditorPanel();
    ~TrackEditorPanel() override;

    void selectTrack(int slotId);
    void deselectTrack();
    int getSelectedSlotId() const { return selectedSlotId; }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    void buttonClicked(juce::Button* button) override;

    void pullFromHub();
    void pushToHub();
    void setSliderSilent(juce::Slider& s, double val);

    juce::Slider& makeKnob(const juce::String& name, double min, double max, double step,
                           double skew = 1.0);

    int selectedSlotId = -1;
    bool ignoreCallbacks = false;

    InstanceParamSnapshot currentSnap;
    InstanceLevelSnapshot currentLevels;
    juce::String trackName;

    // Gain section
    juce::Slider inputGainKnob, outputGainKnob, mixAmountKnob, stereoWidthKnob;
    juce::ToggleButton bypassBtn{ "Bypass" };

    // EQ section (8 bands x freq/gain/Q)
    struct EQBandUI {
        juce::Slider freqKnob, gainKnob, qKnob;
    };
    std::array<EQBandUI, 8> eqBands;

    // Compressor section
    juce::Slider compThreshKnob, compRatioKnob, compAttackKnob, compReleaseKnob, compMakeupKnob;

    // Saturation section
    juce::Slider satDriveKnob, satMixKnob;

    // Reverb section
    juce::Slider revMixKnob, revPredelayKnob, revDecayKnob, revSizeKnob;
    juce::Slider revDampHiFreqKnob, revDampHiShelfKnob, revDampBassFreqKnob, revDampBassMultKnob;
    juce::Slider revAttackKnob, revEarlyDiffKnob, revLateDiffKnob;
    juce::Slider revModRateKnob, revModDepthKnob;
    juce::Slider revEqHighCutKnob, revEqLowCutKnob;
    juce::ComboBox revModeBox, revColorBox;

    // Back button
    juce::TextButton backButton{ "<  Back" };

    // Scrollable viewport
    juce::Viewport viewport;

    struct ContentComponent : public juce::Component
    {
        TrackEditorPanel& owner;
        struct SectionInfo { juce::String name; int y = 0; int w = 0; };
        std::vector<SectionInfo> sections;
        struct KnobLabel { juce::String name; juce::Rectangle<int> bounds; };
        std::vector<KnobLabel> knobLabels;

        ContentComponent(TrackEditorPanel& o) : owner(o) {}
        void paint(juce::Graphics& g) override;
    };
    ContentComponent contentComp{ *this };

    std::vector<juce::Slider*> allSliders;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditorPanel)
};
