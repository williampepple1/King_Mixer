#include "CompressorPanel.h"

CompressorPanel::CompressorPanel(juce::AudioProcessorValueTreeState& a, CompressorDSP& c)
    : apvts(a), comp(c)
{
    auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& paramId) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
        addAndMakeVisible(s);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(10.0f));
        addAndMakeVisible(l);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramId, s));
    };

    setupKnob(threshSlider, lblThresh, "compThreshold");
    setupKnob(ratioSlider, lblRatio, "compRatio");
    setupKnob(attackSlider, lblAttack, "compAttack");
    setupKnob(releaseSlider, lblRelease, "compRelease");
    setupKnob(makeupSlider, lblMakeup, "compMakeup");

    startTimerHz(30);
}

CompressorPanel::~CompressorPanel() { stopTimer(); }

void CompressorPanel::timerCallback()
{
    float rawGR = comp.getGainReduction();
    displaySmoothedGR = displaySmoothedGR * 0.7f + rawGR * 0.3f;
    grHistory[static_cast<size_t>(grWritePos)] = displaySmoothedGR;
    grWritePos = (grWritePos + 1) % kGRHistorySize;
    repaint();
}

void CompressorPanel::drawGRMeter(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg.darker(0.3f));
    g.fillRect(area);

    float gr = comp.getGainReduction();
    float normalised = juce::jlimit(0.0f, 1.0f, -gr / 30.0f);
    int meterH = (int)(normalised * (float)area.getHeight());

    auto meterRect = area.removeFromBottom(meterH);
    g.setColour(t.grMeter);
    g.fillRect(meterRect);

    g.setColour(t.textDim);
    g.setFont(10.0f);
    g.drawText(juce::String(gr, 1) + " dB", area.getX(), area.getBottom() - 16, area.getWidth(), 14,
               juce::Justification::centred);

    g.setColour(t.textDim);
    g.drawText("GR", area.getX(), area.getY() + 2, area.getWidth(), 14, juce::Justification::centred);
}

void CompressorPanel::drawGRTimeline(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg);
    g.fillRect(area);

    g.setColour(t.gridLine);
    float zeroY = (float)area.getY() + 2.0f;
    g.drawHorizontalLine((int)zeroY, (float)area.getX(), (float)area.getRight());

    juce::Path p;
    bool started = false;
    for (int i = 0; i < kGRHistorySize; ++i)
    {
        int idx = (grWritePos + i) % kGRHistorySize;
        float x = area.getX() + (float)i / (float)kGRHistorySize * (float)area.getWidth();
        float normalised = juce::jlimit(0.0f, 1.0f, -grHistory[static_cast<size_t>(idx)] / 30.0f);
        float y = area.getY() + normalised * (float)area.getHeight();
        if (!started) { p.startNewSubPath(x, y); started = true; }
        else p.lineTo(x, y);
    }

    g.setColour(t.grTimeline);
    g.strokePath(p, juce::PathStrokeType(1.5f));

    p.lineTo((float)area.getRight(), (float)area.getY());
    p.lineTo((float)area.getX(), (float)area.getY());
    p.closeSubPath();
    g.setColour(t.grMeter.withAlpha(0.1f));
    g.fillPath(p);
}

void CompressorPanel::drawTransferCurve(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg);
    g.fillRect(area);

    float threshold = (float)threshSlider.getValue();
    float ratio = (float)ratioSlider.getValue();

    g.setColour(t.gridLine);
    for (int db = -60; db <= 0; db += 12)
    {
        float norm = (float)(db + 60) / 60.0f;
        float x = area.getX() + norm * (float)area.getWidth();
        float y = area.getBottom() - norm * (float)area.getHeight();
        g.drawVerticalLine((int)x, (float)area.getY(), (float)area.getBottom());
        g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
    }

    g.setColour(t.textDim.withAlpha(0.3f));
    g.drawLine((float)area.getX(), (float)area.getBottom(),
               (float)area.getRight(), (float)area.getY(), 1.0f);

    juce::Path curve;
    for (int i = 0; i <= area.getWidth(); ++i)
    {
        float inputDB = -60.0f + (float)i / (float)area.getWidth() * 60.0f;
        float outputDB;
        if (inputDB < threshold)
            outputDB = inputDB;
        else
            outputDB = threshold + (inputDB - threshold) / ratio;

        float x = (float)area.getX() + (float)i;
        float normOut = (outputDB + 60.0f) / 60.0f;
        float y = area.getBottom() - normOut * (float)area.getHeight();
        y = juce::jlimit((float)area.getY(), (float)area.getBottom(), y);

        if (i == 0) curve.startNewSubPath(x, y);
        else curve.lineTo(x, y);
    }

    g.setColour(t.accent);
    g.strokePath(curve, juce::PathStrokeType(2.0f));

    float threshNorm = (threshold + 60.0f) / 60.0f;
    float threshX = area.getX() + threshNorm * (float)area.getWidth();
    g.setColour(t.accentWarm.withAlpha(0.5f));
    g.drawVerticalLine((int)threshX, (float)area.getY(), (float)area.getBottom());

    // Live input level indicator dot on the transfer curve
    float inDB = comp.getInputLevel();
    if (inDB > -100.0f)
    {
        float mappedOutDB = (inDB < threshold) ? inDB : threshold + (inDB - threshold) / ratio;
        float inNorm = (inDB + 60.0f) / 60.0f;
        float outNorm = (mappedOutDB + 60.0f) / 60.0f;

        float dotX = area.getX() + inNorm * (float)area.getWidth();
        float dotY = (float)area.getBottom() - outNorm * (float)area.getHeight();
        dotX = juce::jlimit((float)area.getX(), (float)area.getRight(), dotX);
        dotY = juce::jlimit((float)area.getY(), (float)area.getBottom(), dotY);

        // Draw a glow behind the dot
        g.setColour(t.accentWarm.withAlpha(0.2f));
        g.fillEllipse(dotX - 8.0f, dotY - 8.0f, 16.0f, 16.0f);

        g.setColour(t.accentWarm);
        g.fillEllipse(dotX - 4.0f, dotY - 4.0f, 8.0f, 8.0f);

        // Horizontal reference line from the dot to the y-axis
        g.setColour(t.accentWarm.withAlpha(0.25f));
        g.drawHorizontalLine((int)dotY, (float)area.getX(), dotX);

        // Vertical reference line from the dot to the x-axis
        g.drawVerticalLine((int)dotX, dotY, (float)area.getBottom());
    }

    g.setColour(t.textDim);
    g.setFont(9.0f);
    g.drawText("IN", area.getX(), area.getBottom() - 12, 20, 12, juce::Justification::centredLeft);
    g.drawText("OUT", area.getX() + 2, area.getY() + 2, 25, 12, juce::Justification::centredLeft);
}

void CompressorPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto visArea = area.removeFromTop(juce::jmax(0, area.getHeight() - 140));

    auto grMeterArea = visArea.removeFromLeft(40);
    auto transferArea = visArea.removeFromRight(visArea.getHeight());
    auto timelineArea = visArea;

    drawGRMeter(g, grMeterArea);
    drawGRTimeline(g, timelineArea);
    drawTransferCurve(g, transferArea);
}

void CompressorPanel::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(juce::jmax(0, area.getHeight() - 140));

    auto knobArea = area.reduced(40, 5);
    int knobW = knobArea.getWidth() / 5;

    struct KL { juce::Slider* s; juce::Label* l; };
    KL knobs[] = {
        { &threshSlider, &lblThresh }, { &ratioSlider, &lblRatio },
        { &attackSlider, &lblAttack }, { &releaseSlider, &lblRelease },
        { &makeupSlider, &lblMakeup }
    };

    for (int i = 0; i < 5; ++i)
    {
        auto col = knobArea.removeFromLeft(knobW);
        knobs[i].l->setBounds(col.removeFromTop(14));
        knobs[i].s->setBounds(col.removeFromTop(65));
    }
}
