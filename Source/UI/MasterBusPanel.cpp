#include "MasterBusPanel.h"

static const juce::StringArray kGenreNames = { "Rock", "Pop", "Hip-Hop", "EDM", "Jazz", "R&B", "Metal", "Classical" };
static const juce::StringArray kInstrumentNames = { "Vocals", "Drums", "Bass", "E.Guitar", "A.Guitar", "Keys", "Strings", "Brass" };

MasterBusPanel::MasterBusPanel(juce::AudioProcessorValueTreeState& a)
    : apvts(a)
{
    startTimerHz(15);
}

MasterBusPanel::~MasterBusPanel() { stopTimer(); }

void MasterBusPanel::timerCallback()
{
    auto views = InstanceHub::getInstance().getTrackViews();

    strips.clear();
    for (auto& v : views)
    {
        TrackStrip s;
        s.slotId = v.slotId;
        s.name = v.name;
        s.params = v.params;
        s.levels = v.levels;
        s.solo = v.solo;
        s.mute = v.mute;
        s.alive = v.alive;
        strips.push_back(s);
    }

    repaint();
}

void MasterBusPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    g.fillAll(t.background);

    if (strips.empty())
    {
        g.setColour(t.textDim);
        g.setFont(16.0f);
        g.drawText("No track instances detected.\nPlace King Mixer on individual tracks to control them from here.",
                    getLocalBounds().reduced(40), juce::Justification::centred);
        return;
    }

    // Header
    g.setColour(t.headerBg);
    g.fillRect(0, 0, getWidth(), 28);
    g.setColour(t.textBright);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("MASTER BUS CONTROLLER  \u2014  " + juce::String(strips.size()) + " track(s)",
               8, 0, getWidth() - 16, 28, juce::Justification::centredLeft);

    auto contentArea = getLocalBounds().withTrimmedTop(30);
    int startX = contentArea.getX() + 4 - scrollOffset;

    for (size_t i = 0; i < strips.size(); ++i)
    {
        int x = startX + (int)i * (kStripWidth + kStripPadding);
        strips[i].bounds = juce::Rectangle<int>(x, contentArea.getY(), kStripWidth, contentArea.getHeight() - 4);

        if (strips[i].bounds.getRight() > 0 && strips[i].bounds.getX() < getWidth())
            drawTrackStrip(g, strips[i]);
    }
}

void MasterBusPanel::drawTrackStrip(juce::Graphics& g, const TrackStrip& strip)
{
    auto& t = getThemeFrom(this);
    auto area = strip.bounds;

    g.setColour(t.panelBg);
    g.fillRoundedRectangle(area.toFloat(), 6.0f);
    g.setColour(t.gridLine);
    g.drawRoundedRectangle(area.toFloat(), 6.0f, 1.0f);

    int y = area.getY() + 4;

    // Track name
    auto nameArea = juce::Rectangle<int>(area.getX() + 4, y, area.getWidth() - 8, 18);
    g.setColour(t.accent);
    g.fillRoundedRectangle(nameArea.toFloat(), 3.0f);
    g.setColour(t.background);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(strip.name, nameArea, juce::Justification::centred);
    y += 22;

    // Genre / Instrument
    g.setColour(t.textDim);
    g.setFont(9.0f);
    juce::String genreName = (strip.params.genreIndex >= 0 && strip.params.genreIndex < kGenreNames.size())
                              ? kGenreNames[strip.params.genreIndex] : "?";
    juce::String instrName = (strip.params.instrumentIndex >= 0 && strip.params.instrumentIndex < kInstrumentNames.size())
                              ? kInstrumentNames[strip.params.instrumentIndex] : "?";
    g.drawText(genreName + " / " + instrName, area.getX() + 4, y, area.getWidth() - 8, 12, juce::Justification::centred);
    y += 14;

    // Level meters (L and R)
    int meterH = area.getHeight() - (y - area.getY()) - 90;
    if (meterH < 40) meterH = 40;

    int meterW = 14;
    int meterX1 = area.getX() + 8;
    int meterX2 = area.getX() + 8 + meterW + 4;

    auto meterAreaL = juce::Rectangle<int>(meterX1, y, meterW, meterH);
    auto meterAreaR = juce::Rectangle<int>(meterX2, y, meterW, meterH);

    drawMeter(g, meterAreaL, strip.levels.peakL, strip.levels.rmsL, t);
    drawMeter(g, meterAreaR, strip.levels.peakR, strip.levels.rmsR, t);

    // Gain reduction indicator
    int grX = meterX2 + meterW + 8;
    int grW = 8;
    auto grArea = juce::Rectangle<int>(grX, y, grW, meterH);
    g.setColour(t.knobFill);
    g.fillRect(grArea);

    float grNorm = juce::jlimit(0.0f, 1.0f, std::abs(strip.levels.gainReductionDB) / 30.0f);
    int grBarH = (int)(grNorm * (float)meterH);
    g.setColour(t.grMeter);
    g.fillRect(grX, y, grW, grBarH);

    g.setColour(t.textDim);
    g.setFont(7.0f);
    g.drawText("GR", grX - 2, y + meterH + 1, grW + 4, 10, juce::Justification::centred);

    // Key param readouts on the right side of meters
    int infoX = grX + grW + 6;
    int infoW = area.getRight() - infoX - 4;
    if (infoW > 10)
    {
        g.setColour(t.textDim);
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

    // Output fader area
    auto faderRect = juce::Rectangle<int>(area.getX() + 8, y, area.getWidth() - 16, 16);
    const_cast<TrackStrip&>(strip).faderArea = faderRect;
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

    // Solo / Mute buttons
    int btnW = (area.getWidth() - 20) / 2;
    auto soloR = juce::Rectangle<int>(area.getX() + 6, y, btnW, 18);
    auto muteR = juce::Rectangle<int>(area.getX() + 10 + btnW, y, btnW, 18);
    const_cast<TrackStrip&>(strip).soloBtn = soloR;
    const_cast<TrackStrip&>(strip).muteBtn = muteR;

    g.setColour(strip.solo ? juce::Colour(0xffffd700) : t.knobFill);
    g.fillRoundedRectangle(soloR.toFloat(), 3.0f);
    g.setColour(strip.solo ? t.background : t.textDim);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("S", soloR, juce::Justification::centred);

    g.setColour(strip.mute ? juce::Colour(0xffef5350) : t.knobFill);
    g.fillRoundedRectangle(muteR.toFloat(), 3.0f);
    g.setColour(strip.mute ? t.background : t.textDim);
    g.drawText("M", muteR, juce::Justification::centred);

    // Bypass indicator
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
}

void MasterBusPanel::mouseDrag(const juce::MouseEvent& e)
{
    if (draggingFaderStrip < 0 || draggingFaderStrip >= (int)strips.size())
        return;

    auto& strip = strips[draggingFaderStrip];
    float norm = juce::jlimit(0.0f, 1.0f,
        (float)(e.getPosition().x - strip.faderArea.getX()) / (float)strip.faderArea.getWidth());
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

void MasterBusPanel::resized() {}
