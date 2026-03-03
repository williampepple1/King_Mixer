#include "ReverbPanel.h"
#include <cmath>

ReverbPanel::ReverbPanel(juce::AudioProcessorValueTreeState& a,
                          WaveformBuffer& dry, WaveformBuffer& wet,
                          ReverbSend& rev)
    : apvts(a), dryBuf(dry), wetBuf(wet), reverbDSP(rev)
{
    auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& paramId) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 13);
        addAndMakeVisible(s);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(9.0f));
        addAndMakeVisible(l);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramId, s));
    };

    setupKnob(mixSlider, lblMix, "revMix");
    setupKnob(predelaySlider, lblPredelay, "revPredelay");
    setupKnob(decaySlider, lblDecay, "revDecay");

    setupKnob(dampHiFreqSlider, lblDampHiFreq, "revDampHiFreq");
    setupKnob(dampHiShelfSlider, lblDampHiShelf, "revDampHiShelf");
    setupKnob(dampBassFreqSlider, lblDampBassFreq, "revDampBassFreq");
    setupKnob(dampBassMultSlider, lblDampBassMult, "revDampBassMult");

    setupKnob(sizeSlider, lblSize, "revSize");
    setupKnob(attackSlider, lblAttack, "revAttack");

    setupKnob(earlyDiffSlider, lblEarlyDiff, "revEarlyDiff");
    setupKnob(lateDiffSlider, lblLateDiff, "revLateDiff");

    setupKnob(modRateSlider, lblModRate, "revModRate");
    setupKnob(modDepthSlider, lblModDepth, "revModDepth");

    setupKnob(eqHighCutSlider, lblEqHighCut, "revEqHighCut");
    setupKnob(eqLowCutSlider, lblEqLowCut, "revEqLowCut");

    // Mode combo
    modeBox.addItem("Concert Hall", 1);
    modeBox.addItem("Room", 2);
    modeBox.addItem("Chamber", 3);
    modeBox.addItem("Cathedral", 4);
    modeBox.addItem("Plate", 5);
    addAndMakeVisible(modeBox);
    modeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "revMode", modeBox);

    // Color combo
    colorBox.addItem("Clean", 1);
    colorBox.addItem("1970s", 2);
    colorBox.addItem("1980s", 3);
    colorBox.addItem("Now", 4);
    addAndMakeVisible(colorBox);
    colorAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "revColor", colorBox);

    startTimerHz(30);
}

ReverbPanel::~ReverbPanel() { stopTimer(); }

void ReverbPanel::timerCallback()
{
    dryBuf.copyTo(dryData);
    wetBuf.copyTo(wetData);
    repaint();
}

void ReverbPanel::drawGroupBox(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg.brighter(0.05f));
    g.fillRoundedRectangle(area.toFloat(), 6.0f);
    g.setColour(t.gridLine.brighter(0.1f));
    g.drawRoundedRectangle(area.toFloat().reduced(0.5f), 6.0f, 1.0f);

    g.setColour(t.accent);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(title, area.getX(), area.getY() + 2, area.getWidth(), 14, juce::Justification::centred);
}

void ReverbPanel::drawDecayRing(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);

    float centreX = (float)area.getCentreX();
    float centreY = (float)area.getCentreY();
    float radius = (float)juce::jmin(area.getWidth(), area.getHeight()) * 0.4f;

    float decay = (float)decaySlider.getValue();
    float normalised = juce::jlimit(0.0f, 1.0f, decay / 30.0f);

    float startAngle = juce::MathConstants<float>::pi * 0.75f;
    float endAngle = juce::MathConstants<float>::pi * 2.25f;

    // Background arc
    juce::Path bgArc;
    bgArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour(t.gridLine);
    g.strokePath(bgArc, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    float valueAngle = startAngle + normalised * (endAngle - startAngle);
    juce::Path valArc;
    valArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, startAngle, valueAngle, true);
    g.setColour(t.accentWarm);
    g.strokePath(valArc, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Inner glow ring
    juce::Path innerArc;
    innerArc.addCentredArc(centreX, centreY, radius - 8.0f, radius - 8.0f, 0.0f, startAngle, valueAngle, true);
    g.setColour(t.accentWarm.withAlpha(0.15f));
    g.strokePath(innerArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Decay text
    g.setColour(t.textBright);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(juce::String(decay, 2) + " s", area, juce::Justification::centred);

    g.setColour(t.textDim);
    g.setFont(9.0f);
    g.drawText("DECAY", area.getX(), area.getBottom() - 14, area.getWidth(), 12, juce::Justification::centred);
}

void ReverbPanel::drawWaveformOverlay(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg.darker(0.2f));
    g.fillRoundedRectangle(area.toFloat(), 4.0f);

    auto drawWave = [&](const std::array<float, WaveformBuffer::bufferSize>& data, juce::Colour colour) {
        juce::Path p;
        int displaySamples = juce::jmin((int)data.size(), area.getWidth());
        if (displaySamples <= 0) return;
        int step = juce::jmax(1, (int)data.size() / displaySamples);
        for (int i = 0; i < displaySamples; ++i)
        {
            float x = area.getX() + (float)i / (float)displaySamples * (float)area.getWidth();
            size_t idx = static_cast<size_t>(juce::jmin(i * step, (int)data.size() - 1));
            float sample = data[idx];
            float y = area.getCentreY() - sample * (float)area.getHeight() * 0.4f;
            if (i == 0) p.startNewSubPath(x, y);
            else p.lineTo(x, y);
        }
        g.setColour(colour);
        g.strokePath(p, juce::PathStrokeType(1.0f));
    };

    drawWave(dryData, t.textDim.withAlpha(0.4f));
    drawWave(wetData, t.accent);

    g.setFont(8.0f);
    g.setColour(t.textDim);
    g.drawText("DRY", area.getX() + 4, area.getY() + 2, 22, 10, juce::Justification::centredLeft);
    g.setColour(t.accent);
    g.drawText("WET", area.getX() + 28, area.getY() + 2, 22, 10, juce::Justification::centredLeft);
}

void ReverbPanel::drawModeColorBar(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);

    g.setColour(t.textDim);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("MODE:", area.getX(), area.getY(), 45, area.getHeight(), juce::Justification::centredLeft);

    int colorLabelX = area.getCentreX() - 10;
    g.drawText("COLOR:", colorLabelX, area.getY(), 50, area.getHeight(), juce::Justification::centredLeft);
}

void ReverbPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    g.fillAll(t.panelBg);
    auto area = getLocalBounds();

    // Bottom status bar with mode/color
    auto bottomBar = area.removeFromBottom(30);
    drawModeColorBar(g, bottomBar);

    // Waveform strip
    auto waveArea = area.removeFromBottom(55).reduced(4, 2);
    drawWaveformOverlay(g, waveArea);

    // Main knob area
    auto topArea = area;

    // Left column: MIX (small knob) + DECAY ring + PREDELAY
    auto leftCol = topArea.removeFromLeft(160);
    drawDecayRing(g, leftCol.reduced(10, 20));

    // Group boxes for the parameter sections
    int groupW = juce::jmax(10, topArea.getWidth() / 5);
    auto dampingArea = topArea.removeFromLeft(groupW + 20).reduced(3, 3);
    auto shapeArea   = topArea.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto diffArea    = topArea.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto modArea     = topArea.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto eqArea      = topArea.reduced(3, 3);

    drawGroupBox(g, dampingArea, "DAMPING");
    drawGroupBox(g, shapeArea, "SHAPE");
    drawGroupBox(g, diffArea, "DIFF");
    drawGroupBox(g, modArea, "MOD");
    drawGroupBox(g, eqArea, "EQ");
}

void ReverbPanel::resized()
{
    auto area = getLocalBounds();

    // Bottom bar: mode/color combos
    auto bottomBar = area.removeFromBottom(30);
    auto bbLeft = bottomBar.removeFromLeft(bottomBar.getWidth() / 2);
    modeBox.setBounds(bbLeft.removeFromLeft(180).reduced(48, 4));
    colorBox.setBounds(bottomBar.removeFromLeft(180).reduced(50, 4));

    // Waveform strip
    area.removeFromBottom(55);

    // Left column: MIX + PREDELAY small knobs surrounding DECAY
    auto leftCol = area.removeFromLeft(160);
    auto mixArea = leftCol.removeFromTop(70);
    mixSlider.setBounds(mixArea.removeFromLeft(75).reduced(4, 2));
    lblMix.setBounds(mixArea.getX() - 75, mixArea.getY() - 2, 75, 12);

    auto preArea = leftCol.removeFromBottom(70);
    predelaySlider.setBounds(preArea.removeFromLeft(75).reduced(4, 2));
    lblPredelay.setBounds(preArea.getX() - 75, preArea.getY() - 2, 75, 12);

    // The DECAY slider is hidden (ring draws the value), but still needs bounds for attachment
    decaySlider.setBounds(leftCol.reduced(20));
    decaySlider.setAlpha(0.0f);
    lblDecay.setBounds(0, 0, 0, 0);

    // Parameter group columns
    int groupW = juce::jmax(10, area.getWidth() / 5);
    int knobH = 55;
    int lblH = 11;
    int knobW = 52;
    int topPad = 18;

    auto layoutGroup2 = [&](juce::Rectangle<int> groupArea,
                           juce::Slider& s1, juce::Label& l1,
                           juce::Slider& s2, juce::Label& l2) {
        auto inner = groupArea.reduced(4, topPad);
        int halfW = inner.getWidth() / 2;
        auto c1 = inner.removeFromLeft(halfW);
        auto c2 = inner;
        l1.setBounds(c1.removeFromTop(lblH));
        s1.setBounds(c1.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
        l2.setBounds(c2.removeFromTop(lblH));
        s2.setBounds(c2.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
    };

    auto layoutGroup4 = [&](juce::Rectangle<int> groupArea,
                           juce::Slider& s1, juce::Label& l1,
                           juce::Slider& s2, juce::Label& l2,
                           juce::Slider& s3, juce::Label& l3,
                           juce::Slider& s4, juce::Label& l4) {
        auto inner = groupArea.reduced(2, topPad);
        int halfW = inner.getWidth() / 2;
        int halfH = inner.getHeight() / 2;

        auto topRow = inner.removeFromTop(halfH);
        auto botRow = inner;
        auto tl = topRow.removeFromLeft(halfW);
        auto tr = topRow;
        auto bl = botRow.removeFromLeft(halfW);
        auto br = botRow;

        l1.setBounds(tl.removeFromTop(lblH));
        s1.setBounds(tl.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
        l2.setBounds(tr.removeFromTop(lblH));
        s2.setBounds(tr.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
        l3.setBounds(bl.removeFromTop(lblH));
        s3.setBounds(bl.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
        l4.setBounds(br.removeFromTop(lblH));
        s4.setBounds(br.removeFromTop(knobH).withSizeKeepingCentre(knobW, knobH));
    };

    auto dampingArea = area.removeFromLeft(groupW + 20).reduced(3, 3);
    auto shapeArea   = area.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto diffArea    = area.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto modArea     = area.removeFromLeft(juce::jmax(0, groupW - 10)).reduced(3, 3);
    auto eqArea      = area.reduced(3, 3);

    layoutGroup4(dampingArea,
        dampHiFreqSlider, lblDampHiFreq,
        dampHiShelfSlider, lblDampHiShelf,
        dampBassFreqSlider, lblDampBassFreq,
        dampBassMultSlider, lblDampBassMult);

    layoutGroup2(shapeArea, sizeSlider, lblSize, attackSlider, lblAttack);
    layoutGroup2(diffArea, earlyDiffSlider, lblEarlyDiff, lateDiffSlider, lblLateDiff);
    layoutGroup2(modArea, modRateSlider, lblModRate, modDepthSlider, lblModDepth);
    layoutGroup2(eqArea, eqHighCutSlider, lblEqHighCut, eqLowCutSlider, lblEqLowCut);
}
