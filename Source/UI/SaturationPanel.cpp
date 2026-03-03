#include "SaturationPanel.h"
#include <cmath>

SaturationPanel::SaturationPanel(juce::AudioProcessorValueTreeState& a,
                                  WaveformBuffer& pre, WaveformBuffer& post)
    : apvts(a), preBuf(pre), postBuf(post)
{
    auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& paramId) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
        addAndMakeVisible(s);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(10.0f));
        l.setColour(juce::Label::textColourId, KingMixerColours::textDim);
        addAndMakeVisible(l);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramId, s));
    };

    setupKnob(driveSlider, lblDrive, "satDrive");
    setupKnob(mixSlider, lblMix, "satMix");

    startTimerHz(30);
}

SaturationPanel::~SaturationPanel() { stopTimer(); }

void SaturationPanel::timerCallback()
{
    preBuf.copyTo(preData);
    postBuf.copyTo(postData);
    repaint();
}

void SaturationPanel::drawTransferCurve(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(KingMixerColours::panelBg);
    g.fillRect(area);

    g.setColour(KingMixerColours::gridLine);
    g.drawLine((float)area.getX(), (float)area.getCentreY(),
               (float)area.getRight(), (float)area.getCentreY(), 0.5f);
    g.drawLine((float)area.getCentreX(), (float)area.getY(),
               (float)area.getCentreX(), (float)area.getBottom(), 0.5f);

    // Unity line
    g.setColour(KingMixerColours::textDim.withAlpha(0.2f));
    g.drawLine((float)area.getX(), (float)area.getBottom(),
               (float)area.getRight(), (float)area.getY(), 0.5f);

    float drive = (float)driveSlider.getValue() / 100.0f;
    float driveGain = 1.0f + drive * 9.0f;

    juce::Path curve;
    for (int i = 0; i <= area.getWidth(); ++i)
    {
        float inputNorm = (float)i / (float)area.getWidth() * 2.0f - 1.0f;
        float output;
        if (driveGain > 0.01f)
            output = std::tanh(inputNorm * driveGain) / std::tanh(driveGain);
        else
            output = inputNorm;

        float x = (float)area.getX() + (float)i;
        float y = area.getCentreY() - output * (float)area.getHeight() * 0.5f;
        if (i == 0) curve.startNewSubPath(x, y);
        else curve.lineTo(x, y);
    }

    g.setColour(KingMixerColours::accentWarm);
    g.strokePath(curve, juce::PathStrokeType(2.0f));
}

void SaturationPanel::drawWaveformComparison(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(KingMixerColours::panelBg.darker(0.2f));
    g.fillRect(area);

    auto drawWave = [&](const std::array<float, WaveformBuffer::bufferSize>& data, juce::Colour colour) {
        juce::Path p;
        int displaySamples = juce::jmin((int)data.size(), area.getWidth());
        int step = (int)data.size() / displaySamples;
        for (int i = 0; i < displaySamples; ++i)
        {
            float x = area.getX() + (float)i / (float)displaySamples * (float)area.getWidth();
            float sample = data[static_cast<size_t>(i * step)];
            float y = area.getCentreY() - sample * (float)area.getHeight() * 0.45f;
            if (i == 0) p.startNewSubPath(x, y);
            else p.lineTo(x, y);
        }
        g.setColour(colour);
        g.strokePath(p, juce::PathStrokeType(1.0f));
    };

    drawWave(preData, KingMixerColours::textDim.withAlpha(0.5f));
    drawWave(postData, KingMixerColours::accentWarm);

    g.setColour(KingMixerColours::textDim);
    g.setFont(9.0f);
    g.drawText("DRY", area.getX() + 4, area.getY() + 2, 30, 12, juce::Justification::centredLeft);
    g.setColour(KingMixerColours::accentWarm);
    g.drawText("WET", area.getX() + 34, area.getY() + 2, 30, 12, juce::Justification::centredLeft);
}

void SaturationPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto visArea = area.removeFromTop(area.getHeight() - 140);

    auto transferArea = visArea.removeFromLeft(visArea.getWidth() / 2).reduced(4);
    auto waveArea = visArea.reduced(4);

    drawTransferCurve(g, transferArea);
    drawWaveformComparison(g, waveArea);
}

void SaturationPanel::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(area.getHeight() - 140);

    auto knobArea = area.reduced(40, 5);
    int knobW = knobArea.getWidth() / 2;

    struct KL { juce::Slider* s; juce::Label* l; };
    KL knobs[] = { { &driveSlider, &lblDrive }, { &mixSlider, &lblMix } };
    for (int i = 0; i < 2; ++i)
    {
        auto col = knobArea.removeFromLeft(knobW);
        knobs[i].l->setBounds(col.removeFromTop(14));
        knobs[i].s->setBounds(col.removeFromTop(65));
    }
}
