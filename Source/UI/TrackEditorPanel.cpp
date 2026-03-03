#include "TrackEditorPanel.h"

static constexpr int kKnobSize = 58;
static constexpr int kKnobSpacing = 4;
static constexpr int kSectionHeaderH = 22;
static constexpr int kSectionPad = 6;

TrackEditorPanel::TrackEditorPanel()
{
    auto initKnob = [&](juce::Slider& s, double min, double max, double step, double skew = 1.0) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 54, 12);
        s.setRange(min, max, step);
        s.setSkewFactor(skew);
        s.addListener(this);
        contentComp.addAndMakeVisible(s);
        allSliders.push_back(&s);
    };

    // Gain
    initKnob(inputGainKnob, -24, 24, 0.1);
    initKnob(outputGainKnob, -24, 24, 0.1);
    initKnob(mixAmountKnob, 0, 1, 0.01);
    initKnob(stereoWidthKnob, 0, 200, 1);

    bypassBtn.addListener(this);
    contentComp.addAndMakeVisible(bypassBtn);

    // EQ
    for (int i = 0; i < 8; ++i)
    {
        initKnob(eqBands[i].freqKnob, 20, 20000, 0.1, 0.25);
        initKnob(eqBands[i].gainKnob, -24, 24, 0.1);
        initKnob(eqBands[i].qKnob, 0.05, 30, 0.01, 0.4);
    }

    // Compressor
    initKnob(compThreshKnob, -60, 0, 0.1);
    initKnob(compRatioKnob, 1, 20, 0.1, 0.5);
    initKnob(compAttackKnob, 0.1, 200, 0.1, 0.5);
    initKnob(compReleaseKnob, 5, 1000, 1, 0.5);
    initKnob(compMakeupKnob, -6, 24, 0.1);

    // Saturation
    initKnob(satDriveKnob, 0, 100, 0.1);
    initKnob(satMixKnob, 0, 100, 0.1);

    // Reverb
    initKnob(revMixKnob, 0, 100, 0.1);
    initKnob(revPredelayKnob, 0, 1000, 0.01, 0.4);
    initKnob(revDecayKnob, 0.1, 30, 0.01, 0.4);
    initKnob(revSizeKnob, 0, 100, 0.1);
    initKnob(revDampHiFreqKnob, 200, 20000, 1, 0.3);
    initKnob(revDampHiShelfKnob, -48, 0, 0.01);
    initKnob(revDampBassFreqKnob, 20, 2000, 1, 0.4);
    initKnob(revDampBassMultKnob, 0.1, 4, 0.01);
    initKnob(revAttackKnob, 0, 100, 0.1);
    initKnob(revEarlyDiffKnob, 0, 100, 0.1);
    initKnob(revLateDiffKnob, 0, 100, 0.1);
    initKnob(revModRateKnob, 0.01, 20, 0.01, 0.4);
    initKnob(revModDepthKnob, 0, 100, 0.1);
    initKnob(revEqHighCutKnob, 200, 20000, 1, 0.3);
    initKnob(revEqLowCutKnob, 5, 2000, 0.1, 0.4);

    revModeBox.addItemList({ "Concert Hall", "Room", "Chamber", "Cathedral", "Plate" }, 1);
    revModeBox.addListener(this);
    contentComp.addAndMakeVisible(revModeBox);

    revColorBox.addItemList({ "Clean", "1970s", "1980s", "Now" }, 1);
    revColorBox.addListener(this);
    contentComp.addAndMakeVisible(revColorBox);

    backButton.addListener(this);
    addAndMakeVisible(backButton);

    viewport.setViewedComponent(&contentComp, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);

    startTimerHz(15);
}

TrackEditorPanel::~TrackEditorPanel() { stopTimer(); }

void TrackEditorPanel::selectTrack(int slotId)
{
    selectedSlotId = slotId;
    pullFromHub();
    setVisible(true);
    resized();
    repaint();
}

void TrackEditorPanel::deselectTrack()
{
    selectedSlotId = -1;
    setVisible(false);
}

void TrackEditorPanel::setSliderSilent(juce::Slider& s, double val)
{
    ignoreCallbacks = true;
    s.setValue(val, juce::dontSendNotification);
    ignoreCallbacks = false;
}

void TrackEditorPanel::pullFromHub()
{
    if (selectedSlotId < 0) return;
    auto& hub = InstanceHub::getInstance();
    if (!hub.isSlotAlive(selectedSlotId)) { deselectTrack(); return; }

    currentSnap = hub.getParams(selectedSlotId);
    currentLevels = hub.getLevels(selectedSlotId);
    trackName = hub.getTrackName(selectedSlotId);

    setSliderSilent(inputGainKnob, currentSnap.inputGain);
    setSliderSilent(outputGainKnob, currentSnap.outputGain);
    setSliderSilent(mixAmountKnob, currentSnap.mixAmount);
    setSliderSilent(stereoWidthKnob, currentSnap.stereoWidth);

    ignoreCallbacks = true;
    bypassBtn.setToggleState(currentSnap.bypass, juce::dontSendNotification);
    ignoreCallbacks = false;

    for (int i = 0; i < 8; ++i)
    {
        setSliderSilent(eqBands[i].freqKnob, currentSnap.eqBands[i].freq);
        setSliderSilent(eqBands[i].gainKnob, currentSnap.eqBands[i].gain);
        setSliderSilent(eqBands[i].qKnob, currentSnap.eqBands[i].q);
    }

    setSliderSilent(compThreshKnob, currentSnap.compThreshold);
    setSliderSilent(compRatioKnob, currentSnap.compRatio);
    setSliderSilent(compAttackKnob, currentSnap.compAttack);
    setSliderSilent(compReleaseKnob, currentSnap.compRelease);
    setSliderSilent(compMakeupKnob, currentSnap.compMakeup);

    setSliderSilent(satDriveKnob, currentSnap.satDrive);
    setSliderSilent(satMixKnob, currentSnap.satMix);

    setSliderSilent(revMixKnob, currentSnap.revMix);
    setSliderSilent(revPredelayKnob, currentSnap.revPredelay);
    setSliderSilent(revDecayKnob, currentSnap.revDecay);
    setSliderSilent(revSizeKnob, currentSnap.revSize);
    setSliderSilent(revDampHiFreqKnob, currentSnap.revDampHiFreq);
    setSliderSilent(revDampHiShelfKnob, currentSnap.revDampHiShelf);
    setSliderSilent(revDampBassFreqKnob, currentSnap.revDampBassFreq);
    setSliderSilent(revDampBassMultKnob, currentSnap.revDampBassMult);
    setSliderSilent(revAttackKnob, currentSnap.revAttack);
    setSliderSilent(revEarlyDiffKnob, currentSnap.revEarlyDiff);
    setSliderSilent(revLateDiffKnob, currentSnap.revLateDiff);
    setSliderSilent(revModRateKnob, currentSnap.revModRate);
    setSliderSilent(revModDepthKnob, currentSnap.revModDepth);
    setSliderSilent(revEqHighCutKnob, currentSnap.revEqHighCut);
    setSliderSilent(revEqLowCutKnob, currentSnap.revEqLowCut);

    ignoreCallbacks = true;
    revModeBox.setSelectedId(currentSnap.revMode + 1, juce::dontSendNotification);
    revColorBox.setSelectedId(currentSnap.revColor + 1, juce::dontSendNotification);
    ignoreCallbacks = false;
}

void TrackEditorPanel::pushToHub()
{
    if (selectedSlotId < 0) return;

    currentSnap.inputGain = (float)inputGainKnob.getValue();
    currentSnap.outputGain = (float)outputGainKnob.getValue();
    currentSnap.mixAmount = (float)mixAmountKnob.getValue();
    currentSnap.stereoWidth = (float)stereoWidthKnob.getValue();
    currentSnap.bypass = bypassBtn.getToggleState();

    for (int i = 0; i < 8; ++i)
    {
        currentSnap.eqBands[i].freq = (float)eqBands[i].freqKnob.getValue();
        currentSnap.eqBands[i].gain = (float)eqBands[i].gainKnob.getValue();
        currentSnap.eqBands[i].q = (float)eqBands[i].qKnob.getValue();
    }

    currentSnap.compThreshold = (float)compThreshKnob.getValue();
    currentSnap.compRatio = (float)compRatioKnob.getValue();
    currentSnap.compAttack = (float)compAttackKnob.getValue();
    currentSnap.compRelease = (float)compReleaseKnob.getValue();
    currentSnap.compMakeup = (float)compMakeupKnob.getValue();

    currentSnap.satDrive = (float)satDriveKnob.getValue();
    currentSnap.satMix = (float)satMixKnob.getValue();

    currentSnap.revMix = (float)revMixKnob.getValue();
    currentSnap.revPredelay = (float)revPredelayKnob.getValue();
    currentSnap.revDecay = (float)revDecayKnob.getValue();
    currentSnap.revSize = (float)revSizeKnob.getValue();
    currentSnap.revDampHiFreq = (float)revDampHiFreqKnob.getValue();
    currentSnap.revDampHiShelf = (float)revDampHiShelfKnob.getValue();
    currentSnap.revDampBassFreq = (float)revDampBassFreqKnob.getValue();
    currentSnap.revDampBassMult = (float)revDampBassMultKnob.getValue();
    currentSnap.revAttack = (float)revAttackKnob.getValue();
    currentSnap.revEarlyDiff = (float)revEarlyDiffKnob.getValue();
    currentSnap.revLateDiff = (float)revLateDiffKnob.getValue();
    currentSnap.revModRate = (float)revModRateKnob.getValue();
    currentSnap.revModDepth = (float)revModDepthKnob.getValue();
    currentSnap.revEqHighCut = (float)revEqHighCutKnob.getValue();
    currentSnap.revEqLowCut = (float)revEqLowCutKnob.getValue();
    currentSnap.revMode = revModeBox.getSelectedId() - 1;
    currentSnap.revColor = revColorBox.getSelectedId() - 1;

    InstanceHub::getInstance().pushParamsToTrack(selectedSlotId, currentSnap);
}

void TrackEditorPanel::timerCallback()
{
    if (selectedSlotId < 0 || !isVisible()) return;
    auto& hub = InstanceHub::getInstance();
    if (!hub.isSlotAlive(selectedSlotId)) { deselectTrack(); return; }
    currentLevels = hub.getLevels(selectedSlotId);
    repaint();
}

void TrackEditorPanel::sliderValueChanged(juce::Slider*)
{
    if (!ignoreCallbacks) pushToHub();
}

void TrackEditorPanel::comboBoxChanged(juce::ComboBox*)
{
    if (!ignoreCallbacks) pushToHub();
}

void TrackEditorPanel::buttonClicked(juce::Button* btn)
{
    if (btn == &backButton)
    {
        deselectTrack();
        return;
    }
    if (btn == &bypassBtn && !ignoreCallbacks)
        pushToHub();
}

void TrackEditorPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    g.fillAll(t.background);

    // Top bar with track name and meters
    g.setColour(t.headerBg);
    g.fillRect(0, 0, getWidth(), 36);

    g.setColour(t.accent);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    juce::String title = selectedSlotId >= 0 ? ("Editing: " + trackName) : "No track selected";
    g.drawText(title, 50, 0, getWidth() - 180, 36, juce::Justification::centredLeft);

    if (selectedSlotId >= 0)
    {
        int barX = getWidth() - 120;
        int barW = 50;
        int barH = 8;

        g.setColour(t.knobFill);
        g.fillRect(barX, 8, barW, barH);
        g.fillRect(barX, 20, barW, barH);

        auto meterCol = [&](float rms) {
            if (rms > 0.9f) return t.meterRed;
            if (rms > 0.7f) return t.meterYellow;
            return t.meterGreen;
        };

        int lW = (int)(currentLevels.rmsL * barW);
        g.setColour(meterCol(currentLevels.rmsL));
        g.fillRect(barX, 8, lW, barH);

        int rW = (int)(currentLevels.rmsR * barW);
        g.setColour(meterCol(currentLevels.rmsR));
        g.fillRect(barX, 20, rW, barH);

        g.setColour(t.textDim);
        g.setFont(7.0f);
        g.drawText("L", barX - 10, 6, 10, 10, juce::Justification::centredRight);
        g.drawText("R", barX - 10, 18, 10, 10, juce::Justification::centredRight);

        g.setColour(t.textDim);
        g.setFont(8.0f);
        g.drawText("GR: " + juce::String(currentLevels.gainReductionDB, 1) + " dB",
                   barX + barW + 4, 0, 60, 36, juce::Justification::centredLeft);
    }
}

static void drawSectionHeader(juce::Graphics& g, int x, int y, int w, const juce::String& text, const KingMixerTheme& t)
{
    g.setColour(t.accent.withAlpha(0.2f));
    g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)kSectionHeaderH - 2, 3.0f);
    g.setColour(t.accent);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText(text, x + 6, y, w - 12, kSectionHeaderH - 2, juce::Justification::centredLeft);
}

static void drawKnobLabel(juce::Graphics& g, juce::Rectangle<int> knobBounds, const juce::String& text, const KingMixerTheme& t)
{
    g.setColour(t.textDim);
    g.setFont(8.0f);
    g.drawText(text, knobBounds.getX(), knobBounds.getY() - 10, knobBounds.getWidth(), 10, juce::Justification::centred);
}

void TrackEditorPanel::ContentComponent::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    for (auto& s : sections)
        drawSectionHeader(g, kSectionPad, s.y, s.w, s.name, t);
    for (auto& kl : knobLabels)
        drawKnobLabel(g, kl.bounds, kl.name, t);
}

void TrackEditorPanel::resized()
{
    auto area = getLocalBounds();

    backButton.setBounds(4, 6, 44, 24);

    auto contentArea = area.withTrimmedTop(38);
    viewport.setBounds(contentArea);

    int totalW = contentArea.getWidth() - 16;
    int y = kSectionPad;
    int knobsPerRow = juce::jmax(1, totalW / (kKnobSize + kKnobSpacing));

    contentComp.sections.clear();
    contentComp.knobLabels.clear();

    auto addLabel = [&](juce::Slider* k, const juce::String& name) {
        contentComp.knobLabels.push_back({ name, k->getBounds() });
    };

    auto placeSection = [&](const juce::String& label,
                            std::vector<std::pair<juce::Slider*, juce::String>> knobs) {
        contentComp.sections.push_back({ label, y, totalW });
        y += kSectionHeaderH;
        int col = 0;
        for (auto& [k, name] : knobs)
        {
            int cx = kSectionPad + col * (kKnobSize + kKnobSpacing);
            k->setBounds(cx, y + 10, kKnobSize, kKnobSize + 14);
            addLabel(k, name);
            col++;
            if (col >= knobsPerRow) { col = 0; y += kKnobSize + 14 + kKnobSpacing + 10; }
        }
        if (col > 0) y += kKnobSize + 14 + kKnobSpacing + 10;
        y += kSectionPad;
    };

    placeSection("GAIN / MIX", {
        { &inputGainKnob, "Input" }, { &outputGainKnob, "Output" },
        { &mixAmountKnob, "Mix" }, { &stereoWidthKnob, "Width" }
    });

    bypassBtn.setBounds(kSectionPad, y - kSectionPad - 8, 70, 20);
    y += 16;

    // EQ
    contentComp.sections.push_back({ "EQUALIZER (8 bands)", y, totalW });
    y += kSectionHeaderH;
    for (int i = 0; i < 8; ++i)
    {
        int bx = kSectionPad;
        int by = y + 10;
        eqBands[i].freqKnob.setBounds(bx, by, kKnobSize, kKnobSize + 14);
        addLabel(&eqBands[i].freqKnob, "Freq " + juce::String(i + 1));
        eqBands[i].gainKnob.setBounds(bx + kKnobSize + kKnobSpacing, by, kKnobSize, kKnobSize + 14);
        addLabel(&eqBands[i].gainKnob, "Gain " + juce::String(i + 1));
        eqBands[i].qKnob.setBounds(bx + 2 * (kKnobSize + kKnobSpacing), by, kKnobSize, kKnobSize + 14);
        addLabel(&eqBands[i].qKnob, "Q " + juce::String(i + 1));
        y += kKnobSize + 14 + 12;
    }
    y += kSectionPad;

    placeSection("COMPRESSOR", {
        { &compThreshKnob, "Thresh" }, { &compRatioKnob, "Ratio" },
        { &compAttackKnob, "Attack" }, { &compReleaseKnob, "Release" },
        { &compMakeupKnob, "Makeup" }
    });

    placeSection("SATURATION", {
        { &satDriveKnob, "Drive" }, { &satMixKnob, "Mix" }
    });

    placeSection("REVERB", {
        { &revMixKnob, "Mix" }, { &revPredelayKnob, "Predly" },
        { &revDecayKnob, "Decay" }, { &revSizeKnob, "Size" },
        { &revAttackKnob, "Attack" }, { &revEarlyDiffKnob, "E.Diff" },
        { &revLateDiffKnob, "L.Diff" },
        { &revDampHiFreqKnob, "Hi Freq" }, { &revDampHiShelfKnob, "Hi Shelf" },
        { &revDampBassFreqKnob, "Lo Freq" }, { &revDampBassMultKnob, "Lo Mult" },
        { &revModRateKnob, "Mod Hz" }, { &revModDepthKnob, "Mod %" },
        { &revEqHighCutKnob, "HiCut" }, { &revEqLowCutKnob, "LoCut" }
    });

    contentComp.sections.push_back({ "REVERB MODE", y, totalW });
    y += kSectionHeaderH;
    revModeBox.setBounds(kSectionPad, y, 130, 22);
    revColorBox.setBounds(kSectionPad + 140, y, 100, 22);
    y += 30;

    y += kSectionPad * 2;
    contentComp.setSize(totalW, y);
}
