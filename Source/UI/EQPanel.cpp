#include "EQPanel.h"

static constexpr float kMinFreq = 20.0f;
static constexpr float kMaxFreq = 20000.0f;
static constexpr float kMinDB   = -24.0f;
static constexpr float kMaxDB   = 24.0f;

EQPanel::EQPanel(juce::AudioProcessorValueTreeState& a,
                 SpectrumAnalyzer& pre, SpectrumAnalyzer& post,
                 ParametricEQ& e)
    : apvts(a), preAnalyzer(pre), postAnalyzer(post), eq(e)
{
    auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& paramId) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 14);
        addAndMakeVisible(s);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(10.0f));
        addAndMakeVisible(l);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramId, s));
    };

    setupKnob(eqLowFreq, lblLowFreq, "eqLowFreq");
    setupKnob(eqLowGain, lblLowGain, "eqLowGain");
    setupKnob(eqLowMidFreq, lblLMFreq, "eqLowMidFreq");
    setupKnob(eqLowMidGain, lblLMGain, "eqLowMidGain");
    setupKnob(eqLowMidQ, lblLMQ, "eqLowMidQ");
    setupKnob(eqHighMidFreq, lblHMFreq, "eqHighMidFreq");
    setupKnob(eqHighMidGain, lblHMGain, "eqHighMidGain");
    setupKnob(eqHighMidQ, lblHMQ, "eqHighMidQ");
    setupKnob(eqHighFreq, lblHiFreq, "eqHighFreq");
    setupKnob(eqHighGain, lblHiGain, "eqHighGain");

    startTimerHz(30);
}

EQPanel::~EQPanel() { stopTimer(); }

void EQPanel::timerCallback()
{
    preAnalyzer.getNextBlock(preData);
    postAnalyzer.getNextBlock(postData);
    repaint();
}

float EQPanel::freqToX(float freq, float width) const
{
    return width * (std::log(freq / kMinFreq) / std::log(kMaxFreq / kMinFreq));
}

float EQPanel::xToFreq(float x, float width) const
{
    return kMinFreq * std::pow(kMaxFreq / kMinFreq, x / width);
}

float EQPanel::dbToY(float dB, float height) const
{
    return height * (1.0f - (dB - kMinDB) / (kMaxDB - kMinDB));
}

void EQPanel::drawGrid(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.gridLine);

    float freqs[] = { 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    for (float f : freqs)
    {
        float x = area.getX() + freqToX(f, (float)area.getWidth());
        g.drawVerticalLine((int)x, (float)area.getY(), (float)area.getBottom());

        g.setColour(t.textDim);
        g.setFont(9.0f);
        juce::String label = f >= 1000 ? juce::String((int)(f / 1000)) + "k" : juce::String((int)f);
        g.drawText(label, (int)x - 15, area.getBottom() - 14, 30, 12, juce::Justification::centred);
        g.setColour(t.gridLine);
    }

    float dbs[] = { -18, -12, -6, 0, 6, 12, 18 };
    for (float db : dbs)
    {
        float y = area.getY() + dbToY(db, (float)area.getHeight());
        g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
        if (db == 0.0f)
        {
            g.setColour(t.textDim.withAlpha(0.5f));
            g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
            g.setColour(t.gridLine);
        }
    }
}

void EQPanel::drawSpectrum(juce::Graphics& g, juce::Rectangle<int> area,
                            const std::array<float, SpectrumAnalyzer::scopeSize>& data,
                            juce::Colour colour)
{
    juce::Path p;
    bool started = false;

    for (int i = 0; i < SpectrumAnalyzer::scopeSize; ++i)
    {
        float x = area.getX() + (float)i / (float)SpectrumAnalyzer::scopeSize * (float)area.getWidth();
        float y = area.getBottom() - data[(size_t)i] * (float)area.getHeight();
        y = juce::jlimit((float)area.getY(), (float)area.getBottom(), y);

        if (!started) { p.startNewSubPath(x, y); started = true; }
        else p.lineTo(x, y);
    }

    g.setColour(colour);
    g.strokePath(p, juce::PathStrokeType(1.5f));

    p.lineTo((float)area.getRight(), (float)area.getBottom());
    p.lineTo((float)area.getX(), (float)area.getBottom());
    p.closeSubPath();
    g.setColour(colour.withAlpha(0.08f));
    g.fillPath(p);
}

void EQPanel::drawEQCurve(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    const int numPoints = 500;
    std::vector<double> freqs(numPoints);
    std::vector<double> mags(numPoints);

    for (int i = 0; i < numPoints; ++i)
        freqs[i] = (double)xToFreq((float)i / (float)numPoints * (float)area.getWidth(), (float)area.getWidth());

    eq.getMagnitudeResponse(freqs.data(), mags.data(), numPoints, eq.getCurrentSampleRate());

    juce::Path curve;
    for (int i = 0; i < numPoints; ++i)
    {
        float x = area.getX() + (float)i / (float)numPoints * (float)area.getWidth();
        float dB = (float)juce::Decibels::gainToDecibels(mags[i], (double)kMinDB);
        float y = area.getY() + dbToY(dB, (float)area.getHeight());
        y = juce::jlimit((float)area.getY(), (float)area.getBottom(), y);

        if (i == 0) curve.startNewSubPath(x, y);
        else curve.lineTo(x, y);
    }

    g.setColour(t.eqCurve);
    g.strokePath(curve, juce::PathStrokeType(2.0f));

    juce::Path filled(curve);
    float zeroY = area.getY() + dbToY(0.0f, (float)area.getHeight());
    filled.lineTo((float)area.getRight(), zeroY);
    filled.lineTo((float)area.getX(), zeroY);
    filled.closeSubPath();
    g.setColour(t.accent.withAlpha(0.1f));
    g.fillPath(filled);
}

void EQPanel::drawBandNodes(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    struct BandInfo { float freq; float gain; juce::Colour colour; };
    BandInfo bands[] = {
        { (float)eqLowFreq.getValue(), (float)eqLowGain.getValue(), t.accentWarm },
        { (float)eqLowMidFreq.getValue(), (float)eqLowMidGain.getValue(), t.accent },
        { (float)eqHighMidFreq.getValue(), (float)eqHighMidGain.getValue(), t.meterGreen },
        { (float)eqHighFreq.getValue(), (float)eqHighGain.getValue(), t.meterYellow }
    };

    for (auto& b : bands)
    {
        float x = area.getX() + freqToX(b.freq, (float)area.getWidth());
        float y = area.getY() + dbToY(b.gain, (float)area.getHeight());
        g.setColour(b.colour);
        g.fillEllipse(x - 6, y - 6, 12, 12);
        g.setColour(b.colour.brighter(0.4f));
        g.drawEllipse(x - 7, y - 7, 14, 14, 1.5f);
    }
}

int EQPanel::findNearestBand(float x, float y, juce::Rectangle<int> area)
{
    float freqs[] = { (float)eqLowFreq.getValue(), (float)eqLowMidFreq.getValue(),
                      (float)eqHighMidFreq.getValue(), (float)eqHighFreq.getValue() };
    float gains[] = { (float)eqLowGain.getValue(), (float)eqLowMidGain.getValue(),
                      (float)eqHighMidGain.getValue(), (float)eqHighGain.getValue() };

    int best = -1;
    float bestDist = 20.0f;
    for (int i = 0; i < 4; ++i)
    {
        float bx = area.getX() + freqToX(freqs[i], (float)area.getWidth());
        float by = area.getY() + dbToY(gains[i], (float)area.getHeight());
        float dist = std::sqrt((x - bx) * (x - bx) + (y - by) * (y - by));
        if (dist < bestDist) { bestDist = dist; best = i; }
    }
    return best;
}

void EQPanel::mouseDown(const juce::MouseEvent& e)
{
    auto visArea = getLocalBounds().removeFromTop(getHeight() - 160);
    dragBand = findNearestBand((float)e.x, (float)e.y, visArea);
}

void EQPanel::mouseDrag(const juce::MouseEvent& e)
{
    if (dragBand < 0) return;

    auto visArea = getLocalBounds().removeFromTop(getHeight() - 160);
    float freq = xToFreq((float)(e.x - visArea.getX()), (float)visArea.getWidth());
    float dB = kMaxDB - (float)(e.y - visArea.getY()) / (float)visArea.getHeight() * (kMaxDB - kMinDB);

    juce::Slider* freqSliders[] = { &eqLowFreq, &eqLowMidFreq, &eqHighMidFreq, &eqHighFreq };
    juce::Slider* gainSliders[] = { &eqLowGain, &eqLowMidGain, &eqHighMidGain, &eqHighGain };

    freq = juce::jlimit((float)freqSliders[dragBand]->getMinimum(),
                         (float)freqSliders[dragBand]->getMaximum(), freq);
    dB = juce::jlimit((float)gainSliders[dragBand]->getMinimum(),
                       (float)gainSliders[dragBand]->getMaximum(), dB);

    freqSliders[dragBand]->setValue(freq, juce::sendNotificationSync);
    gainSliders[dragBand]->setValue(dB, juce::sendNotificationSync);
}

void EQPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    auto area = getLocalBounds();
    auto visArea = area.removeFromTop(area.getHeight() - 160);

    g.setColour(t.panelBg);
    g.fillRect(visArea);

    drawGrid(g, visArea);
    drawSpectrum(g, visArea, preData, t.spectrumPre);
    drawSpectrum(g, visArea, postData, t.spectrumPost);
    drawEQCurve(g, visArea);
    drawBandNodes(g, visArea);
}

void EQPanel::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(area.getHeight() - 160);

    auto knobArea = area.reduced(10, 5);
    int knobW = knobArea.getWidth() / 10;
    int knobH = 65;
    int lblH = 14;

    struct KnobLabel { juce::Slider* s; juce::Label* l; };
    KnobLabel knobs[] = {
        { &eqLowFreq, &lblLowFreq }, { &eqLowGain, &lblLowGain },
        { &eqLowMidFreq, &lblLMFreq }, { &eqLowMidGain, &lblLMGain }, { &eqLowMidQ, &lblLMQ },
        { &eqHighMidFreq, &lblHMFreq }, { &eqHighMidGain, &lblHMGain }, { &eqHighMidQ, &lblHMQ },
        { &eqHighFreq, &lblHiFreq }, { &eqHighGain, &lblHiGain }
    };

    for (int i = 0; i < 10; ++i)
    {
        auto col = knobArea.removeFromLeft(knobW);
        knobs[i].l->setBounds(col.removeFromTop(lblH));
        knobs[i].s->setBounds(col.removeFromTop(knobH));
    }
}
