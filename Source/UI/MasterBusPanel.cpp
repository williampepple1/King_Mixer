#include "MasterBusPanel.h"

static const juce::StringArray kGenreNames = { "Rock", "Pop", "Hip-Hop", "EDM", "Jazz", "R&B", "Metal", "Classical" };
static const juce::StringArray kInstrumentNames = { "Vocals", "Drums", "Bass", "E.Guitar", "A.Guitar", "Keys", "Strings", "Brass" };

MasterBusPanel::MasterBusPanel(juce::AudioProcessorValueTreeState& a)
    : apvts(a)
{
    genreBox.addItemList(kGenreNames, 1);
    genreBox.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(genreBox);

    applyPresetBtn.addListener(this);
    addAndMakeVisible(applyPresetBtn);

    addChildComponent(trackEditor);
    startTimerHz(15);
}

MasterBusPanel::~MasterBusPanel() { stopTimer(); }

void MasterBusPanel::timerCallback()
{
    auto views = InstanceHub::getInstance().getTrackViews();
    int selectedSlot = trackEditor.getSelectedSlotId();

    strips.clear();
    bool selectedStillAlive = false;
    for (auto& v : views)
    {
        TrackStrip s;
        s.slotId = v.slotId;
        s.name = v.name;
        s.params = v.params;
        s.levels = v.levels;
        s.solo = v.solo;
        s.mute = v.mute;
        s.instrumentIndex = v.instrumentIndex;
        s.selected = (v.slotId == selectedSlot);
        if (s.selected) selectedStillAlive = true;
        strips.push_back(s);
    }

    if (editorVisible && !selectedStillAlive)
    {
        trackEditor.deselectTrack();
        editorVisible = false;
        resized();
    }

    repaint();
}

void MasterBusPanel::buttonClicked(juce::Button* button)
{
    if (button == &applyPresetBtn)
        applyGenrePresetToAll();
}

void MasterBusPanel::applyGenrePresetToAll()
{
    int genreIdx = juce::jmax(0, genreBox.getSelectedId() - 1);
    auto genre = static_cast<Genre>(genreIdx);
    auto& hub = InstanceHub::getInstance();

    for (auto& strip : strips)
    {
        auto instrument = static_cast<Instrument>(strip.instrumentIndex);
        MixRule rule = MixRuleDatabase::getRule(genre, instrument);
        InstanceParamSnapshot snap = MixRuleHelper::ruleToSnapshot(rule, genreIdx, strip.instrumentIndex);
        hub.pushParamsToTrack(strip.slotId, snap);
    }
}

void MasterBusPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    g.fillAll(t.background);

    if (strips.empty())
    {
        g.setColour(t.textDim);
        g.setFont(16.0f);
        auto msgArea = editorVisible ? getLocalBounds().removeFromLeft(getWidth() / 3) : getLocalBounds();
        g.drawText("No track instances detected.\nPlace King Mixer on individual tracks to control them from here.",
                    msgArea.reduced(20), juce::Justification::centred);
        return;
    }

    int stripAreaW = editorVisible ? juce::jmax(220, getWidth() / 3) : getWidth();

    // Header
    g.setColour(t.headerBg);
    g.fillRect(0, 0, stripAreaW, 58);

    // Top line: title
    g.setColour(t.textBright);
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    juce::String headerTxt = juce::String(strips.size()) + " track(s)";
    if (editorVisible)
        headerTxt = "TRACKS (" + headerTxt + ")";
    else
        headerTxt = "MASTER BUS CONTROLLER  \u2014  " + headerTxt;
    g.drawText(headerTxt, 8, 0, stripAreaW - 16, 28, juce::Justification::centredLeft);

    if (editorVisible)
    {
        g.setColour(t.textDim);
        g.setFont(9.0f);
        g.drawText("Click a track to edit it", 8, 0, stripAreaW - 16, 28, juce::Justification::centredRight);
    }

    // Separator below header
    g.setColour(t.gridLine);
    g.drawHorizontalLine(57, 0.0f, (float)stripAreaW);

    auto contentArea = juce::Rectangle<int>(0, 60, stripAreaW, getHeight() - 64);
    int startX = contentArea.getX() + 4 - scrollOffset;

    for (size_t i = 0; i < strips.size(); ++i)
    {
        int x = startX + (int)i * (kStripWidth + kStripPadding);
        strips[i].bounds = juce::Rectangle<int>(x, contentArea.getY(), kStripWidth, contentArea.getHeight() - 4);

        if (strips[i].bounds.getRight() > 0 && strips[i].bounds.getX() < stripAreaW)
            drawTrackStrip(g, strips[i]);
    }

    if (editorVisible)
    {
        g.setColour(t.accent.withAlpha(0.5f));
        g.drawVerticalLine(stripAreaW, 0.0f, (float)getHeight());
    }
}

void MasterBusPanel::drawTrackStrip(juce::Graphics& g, TrackStrip& strip)
{
    auto& t = getThemeFrom(this);
    auto area = strip.bounds;

    if (strip.selected)
    {
        g.setColour(t.accent.withAlpha(0.15f));
        g.fillRoundedRectangle(area.toFloat(), 6.0f);
        g.setColour(t.accent);
        g.drawRoundedRectangle(area.toFloat(), 6.0f, 2.0f);
    }
    else
    {
        g.setColour(t.panelBg);
        g.fillRoundedRectangle(area.toFloat(), 6.0f);
        g.setColour(t.gridLine);
        g.drawRoundedRectangle(area.toFloat(), 6.0f, 1.0f);
    }

    int y = area.getY() + 4;

    // Track name
    auto nameArea = juce::Rectangle<int>(area.getX() + 4, y, area.getWidth() - 8, 18);
    g.setColour(strip.selected ? t.accent : t.accent.withAlpha(0.8f));
    g.fillRoundedRectangle(nameArea.toFloat(), 3.0f);
    g.setColour(t.background);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(strip.name, nameArea, juce::Justification::centred);
    y += 22;

    // Instrument selector display
    juce::String instrName = (strip.instrumentIndex >= 0 && strip.instrumentIndex < kInstrumentNames.size())
                              ? kInstrumentNames[strip.instrumentIndex] : "?";
    auto instrRect = juce::Rectangle<int>(area.getX() + 4, y, area.getWidth() - 8, 16);
    strip.instrArea = instrRect;

    g.setColour(t.accentWarm.withAlpha(0.15f));
    g.fillRoundedRectangle(instrRect.toFloat(), 3.0f);
    g.setColour(t.accentWarm);
    g.drawRoundedRectangle(instrRect.toFloat(), 3.0f, 1.0f);
    g.setFont(9.0f);
    g.drawText(instrName, instrRect, juce::Justification::centred);
    y += 19;

    // Level meters
    int meterH = area.getHeight() - (y - area.getY()) - 90;
    if (meterH < 40) meterH = 40;

    int meterW = 14;
    int meterX1 = area.getX() + 8;
    int meterX2 = area.getX() + 8 + meterW + 4;

    drawMeter(g, { meterX1, y, meterW, meterH }, strip.levels.peakL, strip.levels.rmsL, t);
    drawMeter(g, { meterX2, y, meterW, meterH }, strip.levels.peakR, strip.levels.rmsR, t);

    // GR indicator
    int grX = meterX2 + meterW + 8;
    int grW = 8;
    g.setColour(t.knobFill);
    g.fillRect(grX, y, grW, meterH);

    float grNorm = juce::jlimit(0.0f, 1.0f, std::abs(strip.levels.gainReductionDB) / 30.0f);
    int grBarH = (int)(grNorm * (float)meterH);
    g.setColour(t.grMeter);
    g.fillRect(grX, y, grW, grBarH);

    g.setColour(t.textDim);
    g.setFont(7.0f);
    g.drawText("GR", grX - 2, y + meterH + 1, grW + 4, 10, juce::Justification::centred);

    // Param readouts
    int infoX = grX + grW + 6;
    int infoW = area.getRight() - infoX - 4;
    if (infoW > 10)
    {
        g.setFont(8.0f);
        int iy = y;
        auto drawInfo = [&](const juce::String& label, const juce::String& value) {
            g.setColour(t.textDim);
            g.drawText(label, infoX, iy, infoW, 10, juce::Justification::left);
            iy += 10;
            g.setColour(t.textBright);
            g.drawText(value, infoX, iy, infoW, 10, juce::Justification::left);
            iy += 13;
        };

        drawInfo("IN", juce::String(strip.params.inputGain, 1) + "dB");
        drawInfo("OUT", juce::String(strip.params.outputGain, 1) + "dB");
        drawInfo("COMP", juce::String(strip.params.compThreshold, 0) + "dB");
        drawInfo("SAT", juce::String(strip.params.satDrive, 0) + "%");
        drawInfo("REV", juce::String(strip.params.revMix, 0) + "%");
        drawInfo("MIX", juce::String(strip.params.mixAmount, 2));
    }

    y += meterH + 12;

    // Fader
    auto faderRect = juce::Rectangle<int>(area.getX() + 8, y, area.getWidth() - 16, 16);
    strip.faderArea = faderRect;
    g.setColour(t.knobFill);
    g.fillRoundedRectangle(faderRect.toFloat(), 3.0f);

    float outNorm = juce::jlimit(0.0f, 1.0f, (strip.params.outputGain + 24.0f) / 48.0f);
    int faderFillW = (int)(outNorm * (float)faderRect.getWidth());
    g.setColour(t.accent.withAlpha(0.7f));
    g.fillRoundedRectangle(juce::Rectangle<int>(faderRect.getX(), faderRect.getY(), faderFillW, faderRect.getHeight()).toFloat(), 3.0f);

    g.setColour(t.textBright);
    g.setFont(8.0f);
    g.drawText(juce::String(strip.params.outputGain, 1) + " dB", faderRect, juce::Justification::centred);
    y += 20;

    // Solo / Mute
    int btnW = (area.getWidth() - 20) / 2;
    auto soloR = juce::Rectangle<int>(area.getX() + 6, y, btnW, 18);
    auto muteR = juce::Rectangle<int>(area.getX() + 10 + btnW, y, btnW, 18);
    strip.soloBtn = soloR;
    strip.muteBtn = muteR;

    g.setColour(strip.solo ? juce::Colour(0xffffd700) : t.knobFill);
    g.fillRoundedRectangle(soloR.toFloat(), 3.0f);
    g.setColour(strip.solo ? t.background : t.textDim);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("S", soloR, juce::Justification::centred);

    g.setColour(strip.mute ? juce::Colour(0xffef5350) : t.knobFill);
    g.fillRoundedRectangle(muteR.toFloat(), 3.0f);
    g.setColour(strip.mute ? t.background : t.textDim);
    g.drawText("M", muteR, juce::Justification::centred);

    y += 22;
    if (strip.params.bypass)
    {
        g.setColour(juce::Colour(0xffef5350));
        g.setFont(8.0f);
        g.drawText("BYPASSED", area.getX(), y, area.getWidth(), 10, juce::Justification::centred);
    }
}

void MasterBusPanel::drawMeter(juce::Graphics& g, juce::Rectangle<int> area, float peak, float rms, const KingMixerTheme& t)
{
    g.setColour(t.knobFill);
    g.fillRect(area);

    int rmsH = (int)(rms * (float)area.getHeight());
    auto rmsColour = t.meterGreen;
    if (rms > 0.7f) rmsColour = t.meterYellow;
    if (rms > 0.9f) rmsColour = t.meterRed;
    g.setColour(rmsColour);
    g.fillRect(area.getX(), area.getBottom() - rmsH, area.getWidth(), rmsH);

    int peakY = area.getBottom() - (int)(peak * (float)area.getHeight());
    g.setColour(t.textBright);
    g.drawHorizontalLine(peakY, (float)area.getX(), (float)area.getRight());
}

int MasterBusPanel::getStripIndexAt(juce::Point<int> pos) const
{
    for (size_t i = 0; i < strips.size(); ++i)
        if (strips[i].bounds.contains(pos))
            return (int)i;
    return -1;
}

void MasterBusPanel::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.getPosition();

    for (size_t i = 0; i < strips.size(); ++i)
    {
        // Instrument area click: show popup to change instrument
        if (strips[i].instrArea.contains(pos))
        {
            juce::PopupMenu menu;
            for (int j = 0; j < kInstrumentNames.size(); ++j)
                menu.addItem(j + 1, kInstrumentNames[j], true, j == strips[i].instrumentIndex);

            int slotId = strips[i].slotId;
            auto safeThis = juce::Component::SafePointer<MasterBusPanel>(this);
            menu.showMenuAsync(juce::PopupMenu::Options(), [safeThis, slotId](int result) {
                if (safeThis == nullptr || result <= 0) return;
                InstanceHub::getInstance().setInstrument(slotId, result - 1);
            });
            return;
        }

        if (strips[i].soloBtn.contains(pos))
        {
            bool newSolo = !strips[i].solo;
            InstanceHub::getInstance().setSoloFromMaster(strips[i].slotId, newSolo);
            repaint();
            return;
        }
        if (strips[i].muteBtn.contains(pos))
        {
            bool newMute = !strips[i].mute;
            InstanceHub::getInstance().setMuteFromMaster(strips[i].slotId, newMute);
            repaint();
            return;
        }
        if (strips[i].faderArea.contains(pos))
        {
            draggingFaderStrip = (int)i;
            mouseDrag(e);
            return;
        }
    }

    int idx = getStripIndexAt(pos);
    if (idx >= 0)
    {
        int slotId = strips[idx].slotId;
        if (trackEditor.getSelectedSlotId() == slotId)
        {
            trackEditor.deselectTrack();
            editorVisible = false;
        }
        else
        {
            trackEditor.selectTrack(slotId);
            editorVisible = true;
        }
        resized();
        repaint();
        return;
    }
}

void MasterBusPanel::mouseDrag(const juce::MouseEvent& e)
{
    if (draggingFaderStrip < 0 || draggingFaderStrip >= (int)strips.size())
        return;

    auto& strip = strips[draggingFaderStrip];
    float faderW = (float)strip.faderArea.getWidth();
    if (faderW <= 0.0f) return;
    float norm = juce::jlimit(0.0f, 1.0f,
        (float)(e.getPosition().x - strip.faderArea.getX()) / faderW);
    float newGainDB = norm * 48.0f - 24.0f;

    InstanceParamSnapshot snap = strip.params;
    snap.outputGain = newGainDB;
    InstanceHub::getInstance().pushParamsToTrack(strip.slotId, snap);
    repaint();
}

void MasterBusPanel::mouseUp(const juce::MouseEvent&)
{
    draggingFaderStrip = -1;
}

void MasterBusPanel::resized()
{
    int stripAreaW = editorVisible ? juce::jmax(220, getWidth() / 3) : getWidth();

    // Genre selector and Apply button in the header bar (second row)
    int comboW = juce::jmin(140, stripAreaW / 3);
    int btnW = juce::jmin(120, stripAreaW / 3);
    genreBox.setBounds(8, 30, comboW, 22);
    applyPresetBtn.setBounds(comboW + 16, 30, btnW, 22);

    if (editorVisible)
    {
        int editorW = juce::jmax(0, getWidth() - stripAreaW - 1);
        trackEditor.setBounds(stripAreaW + 1, 0, editorW, getHeight());
        trackEditor.setVisible(true);
    }
    else
    {
        trackEditor.setVisible(false);
    }
}
