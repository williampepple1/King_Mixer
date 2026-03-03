#include "GainMixPanel.h"

GainMixPanel::GainMixPanel(juce::AudioProcessorValueTreeState& a,
                            LevelMeterData& in, LevelMeterData& out)
    : apvts(a), inputMeter(in), outputMeter(out)
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

    setupKnob(inputGainSlider, lblInGain, "inputGain");
    setupKnob(outputGainSlider, lblOutGain, "outputGain");
    setupKnob(stereoWidthSlider, lblWidth, "stereoWidth");
    setupKnob(mixAmountSlider, lblMix, "mixAmount");

    addAndMakeVisible(bypassToggle);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "bypass", bypassToggle);

    startTimerHz(30);
}

GainMixPanel::~GainMixPanel() { stopTimer(); }

void GainMixPanel::timerCallback()
{
    inPeakL = inputMeter.getPeakL(); inPeakR = inputMeter.getPeakR();
    inRmsL = inputMeter.getRmsL();   inRmsR = inputMeter.getRmsR();
    outPeakL = outputMeter.getPeakL(); outPeakR = outputMeter.getPeakR();
    outRmsL = outputMeter.getRmsL();   outRmsR = outputMeter.getRmsR();

    peakHoldL = std::max(outPeakL, peakHoldL * peakHoldDecay);
    peakHoldR = std::max(outPeakR, peakHoldR * peakHoldDecay);

    repaint();
}

void GainMixPanel::drawStereoMeter(juce::Graphics& g, juce::Rectangle<int> area,
                                     float pL, float pR, float rL, float rR,
                                     const juce::String& label)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg.darker(0.3f));
    g.fillRoundedRectangle(area.toFloat(), 4.0f);

    int meterW = (area.getWidth() - 30) / 2;
    int meterH = area.getHeight() - 24;

    auto drawBar = [&](int x, int y, int w, int h, float peak, float rms) {
        g.setColour(t.knobFill);
        g.fillRect(x, y, w, h);

        int rmsH = (int)(rms * (float)h);
        auto rmsRect = juce::Rectangle<int>(x, y + h - rmsH, w, rmsH);
        g.setColour(t.meterGreen);
        if (rms > 0.7f) g.setColour(t.meterYellow);
        if (rms > 0.9f) g.setColour(t.meterRed);
        g.fillRect(rmsRect);

        int peakY = y + h - (int)(peak * (float)h);
        g.setColour(t.textBright);
        g.drawHorizontalLine(peakY, (float)x, (float)(x + w));
    };

    int x1 = area.getX() + 8;
    int x2 = x1 + meterW + 6;
    int my = area.getY() + 18;

    drawBar(x1, my, meterW, meterH, pL, rL);
    drawBar(x2, my, meterW, meterH, pR, rR);

    g.setColour(t.textDim);
    g.setFont(10.0f);
    g.drawText(label, area.getX(), area.getY() + 2, area.getWidth(), 14, juce::Justification::centred);
    g.drawText("L", x1, my + meterH + 2, meterW, 12, juce::Justification::centred);
    g.drawText("R", x2, my + meterH + 2, meterW, 12, juce::Justification::centred);

    g.setFont(8.0f);
    float dbs[] = { 0.0f, -6.0f, -12.0f, -24.0f, -48.0f };
    for (float db : dbs)
    {
        float norm = juce::Decibels::decibelsToGain(db);
        int ypos = my + meterH - (int)(norm * (float)meterH);
        g.drawText(juce::String((int)db), area.getRight() - 24, ypos - 5, 22, 10, juce::Justification::centredRight);
    }
}

void GainMixPanel::drawCorrelationMeter(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.panelBg.darker(0.3f));
    g.fillRoundedRectangle(area.toFloat(), 4.0f);

    g.setColour(t.textDim);
    g.setFont(10.0f);
    g.drawText("STEREO FIELD", area.getX(), area.getY() + 2, area.getWidth(), 14, juce::Justification::centred);

    int barY = area.getCentreY() - 4;
    int barH = 8;
    int barX = area.getX() + 10;
    int barW = area.getWidth() - 20;

    g.setColour(t.knobFill);
    g.fillRect(barX, barY, barW, barH);

    int centreX = barX + barW / 2;
    g.setColour(t.textDim);
    g.drawVerticalLine(centreX, (float)(barY - 3), (float)(barY + barH + 3));

    float diffL = std::abs(outPeakL - outPeakR);
    float avgPeak = (outPeakL + outPeakR) * 0.5f;
    float correlation = avgPeak > 0.001f ? 1.0f - diffL / avgPeak : 1.0f;
    correlation = juce::jlimit(0.0f, 1.0f, correlation);

    int indicatorX = centreX + (int)((correlation - 0.5f) * 2.0f * (float)(barW / 2));
    g.setColour(t.accent);
    g.fillRect(indicatorX - 3, barY - 2, 6, barH + 4);

    g.setFont(8.0f);
    g.setColour(t.textDim);
    g.drawText("-1", barX, barY + barH + 4, 16, 10, juce::Justification::centred);
    g.drawText("0", centreX - 8, barY + barH + 4, 16, 10, juce::Justification::centred);
    g.drawText("+1", barX + barW - 16, barY + barH + 4, 16, 10, juce::Justification::centred);
}

void GainMixPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto visArea = area.removeFromTop(area.getHeight() - 140);

    int meterW = (visArea.getWidth() - 40) / 2;
    int meterW2 = juce::jmin(meterW, 140);

    auto inMeterArea = visArea.removeFromLeft(meterW2 + 10).reduced(6);
    auto outMeterArea = visArea.removeFromLeft(meterW2 + 10).reduced(6);
    auto corrArea = visArea.reduced(6, visArea.getHeight() / 4);

    drawStereoMeter(g, inMeterArea, inPeakL, inPeakR, inRmsL, inRmsR, "INPUT");
    drawStereoMeter(g, outMeterArea, outPeakL, outPeakR, outRmsL, outRmsR, "OUTPUT");
    drawCorrelationMeter(g, corrArea);
}

void GainMixPanel::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(area.getHeight() - 140);

    auto knobArea = area.reduced(20, 5);
    int knobW = knobArea.getWidth() / 5;

    struct KL { juce::Slider* s; juce::Label* l; };
    KL knobs[] = {
        { &inputGainSlider, &lblInGain }, { &outputGainSlider, &lblOutGain },
        { &stereoWidthSlider, &lblWidth }, { &mixAmountSlider, &lblMix }
    };

    for (int i = 0; i < 4; ++i)
    {
        auto col = knobArea.removeFromLeft(knobW);
        knobs[i].l->setBounds(col.removeFromTop(14));
        knobs[i].s->setBounds(col.removeFromTop(65));
    }

    auto lastCol = knobArea.removeFromLeft(knobW);
    lastCol.removeFromTop(14);
    bypassToggle.setBounds(lastCol.removeFromTop(30).reduced(4, 0));
}
