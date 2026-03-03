#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../IPC/InstanceHub.h"
#include "CustomLookAndFeel.h"
#include "TrackEditorPanel.h"

class MasterBusPanel : public juce::Component, private juce::Timer
{
public:
    explicit MasterBusPanel(juce::AudioProcessorValueTreeState& apvts);
    ~MasterBusPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    void timerCallback() override;

    struct TrackStrip
    {
        int slotId = -1;
        juce::String name;
        InstanceParamSnapshot params;
        InstanceLevelSnapshot levels;
        bool solo = false;
        bool mute = false;
        bool selected = false;

        juce::Rectangle<int> bounds;
        juce::Rectangle<int> faderArea;
        juce::Rectangle<int> soloBtn;
        juce::Rectangle<int> muteBtn;
        juce::Rectangle<int> nameArea;
    };

    void drawTrackStrip(juce::Graphics& g, const TrackStrip& strip);
    void drawMeter(juce::Graphics& g, juce::Rectangle<int> area, float peak, float rms, const KingMixerTheme& t);

    int getStripIndexAt(juce::Point<int> pos) const;

    juce::AudioProcessorValueTreeState& apvts;
    std::vector<TrackStrip> strips;

    int scrollOffset = 0;
    static constexpr int kStripWidth = 110;
    static constexpr int kStripPadding = 4;

    int draggingFaderStrip = -1;

    // Track editor
    TrackEditorPanel trackEditor;
    bool editorVisible = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterBusPanel)
};
