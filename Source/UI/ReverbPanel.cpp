#include "ReverbPanel.h"
#include <cmath>

ReverbPanel::ReverbPanel(juce::AudioProcessorValueTreeState& a,
                          WaveformBuffer& dry, WaveformBuffer& wet)
    : apvts(a), dryBuf(dry), wetBuf(wet)
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

    setupKnob(sendSlider, lblSend, "reverbSend");
    setupKnob(roomSizeSlider, lblRoomSize, "reverbRoomSize");
    setupKnob(dampingSlider, lblDamping, "reverbDamping");

    startTimerHz(30);
}

ReverbPanel::~ReverbPanel() { stopTimer(); }

void ReverbPanel::timerCallback()
{
    dryBuf.copyTo(dryData);
    wetBuf.copyTo(wetData);
    repaint();
}

void ReverbPanel::drawDecayEnvelope(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(KingMixerColours::panelBg);
    g.fillRect(area);

    float roomSize = (float)roomSizeSlider.getValue();
    float damping  = (float)dampingSlider.getValue();

    float decayTime = 0.2f + roomSize * 2.8f;
    float dampFactor = 1.0f - damping * 0.7f;

    g.setColour(KingMixerColours::gridLine);
    for (int i = 1; i <= 4; ++i)
    {
        float x = area.getX() + (float)i / 5.0f * (float)area.getWidth();
        g.drawVerticalLine((int)x, (float)area.getY(), (float)area.getBottom());
    }

    // Decay envelope curve
    juce::Path envelope;
    for (int i = 0; i <= area.getWidth(); ++i)
    {
        float t = (float)i / (float)area.getWidth() * 3.0f;
        float amplitude = std::exp(-t / decayTime) * dampFactor;
        amplitude = juce::jlimit(0.0f, 1.0f, amplitude);

        float x = (float)area.getX() + (float)i;
        float y = area.getBottom() - amplitude * (float)area.getHeight() * 0.9f;

        if (i == 0) envelope.startNewSubPath(x, y);
        else envelope.lineTo(x, y);
    }

    g.setColour(KingMixerColours::accent);
    g.strokePath(envelope, juce::PathStrokeType(2.0f));

    // Fill under envelope
    juce::Path filled(envelope);
    filled.lineTo((float)area.getRight(), (float)area.getBottom());
    filled.lineTo((float)area.getX(), (float)area.getBottom());
    filled.closeSubPath();
    g.setColour(KingMixerColours::accent.withAlpha(0.08f));
    g.fillPath(filled);

    // Simulated early reflections
    juce::Random rng(42);
    g.setColour(KingMixerColours::accent.withAlpha(0.4f));
    for (int r = 0; r < 12; ++r)
    {
        float t = rng.nextFloat() * 0.3f;
        float amp = std::exp(-t / decayTime) * dampFactor * (0.5f + rng.nextFloat() * 0.5f);
        float x = area.getX() + t / 3.0f * (float)area.getWidth();
        float baseY = (float)area.getBottom();
        float h = amp * (float)area.getHeight() * 0.8f;
        g.drawLine(x, baseY, x, baseY - h, 1.5f);
    }

    g.setColour(KingMixerColours::textDim);
    g.setFont(9.0f);
    g.drawText("DECAY ENVELOPE", area.getX() + 4, area.getY() + 2, 120, 12, juce::Justification::centredLeft);
}

void ReverbPanel::drawWaveformOverlay(juce::Graphics& g, juce::Rectangle<int> area)
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

    drawWave(dryData, KingMixerColours::textDim.withAlpha(0.5f));
    drawWave(wetData, KingMixerColours::accent);

    g.setColour(KingMixerColours::textDim);
    g.setFont(9.0f);
    g.drawText("DRY", area.getX() + 4, area.getY() + 2, 25, 12, juce::Justification::centredLeft);
    g.setColour(KingMixerColours::accent);
    g.drawText("WET", area.getX() + 30, area.getY() + 2, 25, 12, juce::Justification::centredLeft);
}

void ReverbPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto visArea = area.removeFromTop(area.getHeight() - 140);

    auto decayArea = visArea.removeFromTop(visArea.getHeight() * 2 / 3).reduced(4);
    auto waveArea = visArea.reduced(4);

    drawDecayEnvelope(g, decayArea);
    drawWaveformOverlay(g, waveArea);
}

void ReverbPanel::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(area.getHeight() - 140);

    auto knobArea = area.reduced(40, 5);
    int knobW = knobArea.getWidth() / 3;

    struct KL { juce::Slider* s; juce::Label* l; };
    KL knobs[] = { { &sendSlider, &lblSend }, { &roomSizeSlider, &lblRoomSize }, { &dampingSlider, &lblDamping } };
    for (int i = 0; i < 3; ++i)
    {
        auto col = knobArea.removeFromLeft(knobW);
        knobs[i].l->setBounds(col.removeFromTop(14));
        knobs[i].s->setBounds(col.removeFromTop(65));
    }
}
