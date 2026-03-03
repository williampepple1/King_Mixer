#include "EQPanel.h"

static constexpr float kMinFreq = 20.0f;
static constexpr float kMaxFreq = 20000.0f;
static constexpr float kMinDB   = -24.0f;
static constexpr float kMaxDB   = 24.0f;

static const juce::Colour kBandColours[] = {
    juce::Colour(0xffff5252),   // red
    juce::Colour(0xffff7043),   // orange
    juce::Colour(0xffffca28),   // yellow
    juce::Colour(0xff66bb6a),   // green
    juce::Colour(0xff26c6da),   // cyan
    juce::Colour(0xff42a5f5),   // blue
    juce::Colour(0xffab47bc),   // purple
    juce::Colour(0xffec407a),   // pink
};

EQPanel::EQPanel(juce::AudioProcessorValueTreeState& a,
                 SpectrumAnalyzer& pre, SpectrumAnalyzer& post,
                 ParametricEQ& e)
    : apvts(a), preAnalyzer(pre), postAnalyzer(post), eq(e)
{
    startTimerHz(30);
}

EQPanel::~EQPanel() { stopTimer(); }

void EQPanel::timerCallback()
{
    preAnalyzer.getNextBlock(preData);
    postAnalyzer.getNextBlock(postData);
    repaint();
}

juce::Colour EQPanel::getBandColour(int idx) const
{
    return kBandColours[idx % 8];
}

juce::String EQPanel::getFilterTypeName(int type) const
{
    switch (type)
    {
        case 0: return "Peak";
        case 1: return "Low Shelf";
        case 2: return "High Shelf";
        case 3: return "Low Cut";
        case 4: return "High Cut";
        case 5: return "Band Pass";
        case 6: return "Notch";
        default: return "Peak";
    }
}

juce::Rectangle<int> EQPanel::getVisArea() const
{
    return getLocalBounds().removeFromTop(juce::jmax(0, getHeight() - 50));
}

float EQPanel::freqToX(float freq, float width) const
{
    if (width <= 0.0f || freq <= 0.0f) return 0.0f;
    return width * (std::log(freq / kMinFreq) / std::log(kMaxFreq / kMinFreq));
}

float EQPanel::xToFreq(float x, float width) const
{
    if (width <= 0.0f) return kMinFreq;
    return kMinFreq * std::pow(kMaxFreq / kMinFreq, x / width);
}

float EQPanel::dbToY(float dB, float height) const
{
    if (height <= 0.0f) return 0.0f;
    return height * (1.0f - (dB - kMinDB) / (kMaxDB - kMinDB));
}

float EQPanel::yToDB(float y, float height) const
{
    if (height <= 0.0f) return 0.0f;
    return kMaxDB - y / height * (kMaxDB - kMinDB);
}

void EQPanel::drawGrid(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);
    g.setColour(t.gridLine);

    float freqs[] = { 30, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    for (float f : freqs)
    {
        float x = area.getX() + freqToX(f, (float)area.getWidth());
        g.drawVerticalLine((int)x, (float)area.getY(), (float)area.getBottom());

        g.setColour(t.textDim.withAlpha(0.5f));
        g.setFont(8.0f);
        juce::String label = f >= 1000 ? juce::String((int)(f / 1000)) + "k" : juce::String((int)f);
        g.drawText(label, (int)x - 15, area.getBottom() - 13, 30, 12, juce::Justification::centred);
        g.setColour(t.gridLine);
    }

    float dbs[] = { -18, -12, -6, 0, 6, 12, 18 };
    for (float db : dbs)
    {
        float y = area.getY() + dbToY(db, (float)area.getHeight());
        if (db == 0.0f)
        {
            g.setColour(t.textDim.withAlpha(0.4f));
            g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
        }
        else
        {
            g.setColour(t.gridLine);
            g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
        }

        g.setColour(t.textDim.withAlpha(0.4f));
        g.setFont(8.0f);
        g.drawText(juce::String((int)db), area.getRight() - 28, (int)y - 6, 24, 12, juce::Justification::centredRight);
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
    g.setColour(colour.withAlpha(0.06f));
    g.fillPath(p);
}

void EQPanel::drawBandCurve(juce::Graphics& g, juce::Rectangle<int> area, int bandIdx, juce::Colour colour)
{
    const int numPoints = 300;
    std::vector<double> freqs(numPoints);
    std::vector<double> mags(numPoints);

    for (int i = 0; i < numPoints; ++i)
        freqs[i] = (double)xToFreq((float)i / (float)numPoints * (float)area.getWidth(), (float)area.getWidth());

    eq.getBandMagnitudeResponse(bandIdx, freqs.data(), mags.data(), numPoints, eq.getCurrentSampleRate());

    juce::Path curve;
    float zeroY = area.getY() + dbToY(0.0f, (float)area.getHeight());
    bool hasShape = false;

    for (int i = 0; i < numPoints; ++i)
    {
        float dB = (float)juce::Decibels::gainToDecibels(mags[i], (double)kMinDB);
        if (std::abs(dB) > 0.05f) hasShape = true;
        float x = area.getX() + (float)i / (float)numPoints * (float)area.getWidth();
        float y = area.getY() + dbToY(dB, (float)area.getHeight());
        y = juce::jlimit((float)area.getY(), (float)area.getBottom(), y);

        if (i == 0) curve.startNewSubPath(x, y);
        else curve.lineTo(x, y);
    }

    if (!hasShape) return;

    bool isSelected = (bandIdx == selectedBand);
    g.setColour(colour.withAlpha(isSelected ? 0.9f : 0.4f));
    g.strokePath(curve, juce::PathStrokeType(isSelected ? 1.5f : 1.0f));

    juce::Path filled(curve);
    filled.lineTo((float)area.getRight(), zeroY);
    filled.lineTo((float)area.getX(), zeroY);
    filled.closeSubPath();
    g.setColour(colour.withAlpha(isSelected ? 0.08f : 0.03f));
    g.fillPath(filled);
}

void EQPanel::drawCompositeCurve(juce::Graphics& g, juce::Rectangle<int> area)
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
    g.setColour(t.accent.withAlpha(0.07f));
    g.fillPath(filled);
}

void EQPanel::drawBandNodes(juce::Graphics& g, juce::Rectangle<int> area)
{
    for (int i = 0; i < ParametricEQ::kMaxBands; ++i)
    {
        auto& st = eq.getBandState(i);
        if (!st.enabled) continue;

        auto colour = getBandColour(i);
        float x = area.getX() + freqToX(st.frequency, (float)area.getWidth());
        float y = area.getY() + dbToY(st.gainDB, (float)area.getHeight());

        bool isLowCut = st.filterType == (int)EQFilterType::LowCut;
        bool isHighCut = st.filterType == (int)EQFilterType::HighCut;
        if (isLowCut || isHighCut)
            y = area.getY() + dbToY(0.0f, (float)area.getHeight());

        float nodeSize = (i == selectedBand) ? 9.0f : 7.0f;
        bool isHovered = (i == hoveredBand);

        g.setColour(colour.withAlpha(isHovered ? 0.3f : 0.0f));
        g.fillEllipse(x - 14, y - 14, 28, 28);

        g.setColour(colour);
        g.fillEllipse(x - nodeSize, y - nodeSize, nodeSize * 2, nodeSize * 2);

        if (i == selectedBand)
        {
            g.setColour(colour.brighter(0.5f));
            g.drawEllipse(x - nodeSize - 1.5f, y - nodeSize - 1.5f, (nodeSize + 1.5f) * 2, (nodeSize + 1.5f) * 2, 1.5f);
        }
    }
}

void EQPanel::drawBandInfo(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto& t = getThemeFrom(this);

    for (int i = 0; i < ParametricEQ::kMaxBands; ++i)
    {
        auto& st = eq.getBandState(i);
        auto colour = getBandColour(i);
        int cellW = area.getWidth() / ParametricEQ::kMaxBands;
        auto cell = juce::Rectangle<int>(area.getX() + i * cellW, area.getY(), cellW, area.getHeight());

        g.setColour(st.enabled ? colour.withAlpha(0.15f) : t.panelBg.darker(0.1f));
        g.fillRoundedRectangle(cell.reduced(2, 3).toFloat(), 3.0f);

        if (i == selectedBand)
        {
            g.setColour(colour);
            g.drawRoundedRectangle(cell.reduced(2, 3).toFloat(), 3.0f, 1.5f);
        }

        g.setColour(st.enabled ? colour : t.textDim.withAlpha(0.3f));
        g.setFont(8.0f);

        juce::String freqStr = st.frequency >= 1000 ? juce::String(st.frequency / 1000.0f, 1) + "k" : juce::String((int)st.frequency);
        juce::String gainStr = juce::String(st.gainDB, 1) + "dB";
        juce::String typeStr = getFilterTypeName(st.filterType);

        g.drawText(freqStr + "  " + gainStr, cell.reduced(4, 0), juce::Justification::centred);
        g.setFont(7.0f);
        g.setColour(t.textDim.withAlpha(st.enabled ? 0.6f : 0.2f));
        g.drawText(typeStr, cell.getX(), cell.getBottom() - 12, cell.getWidth(), 10, juce::Justification::centred);
    }
}

int EQPanel::findNearestBand(float x, float y, juce::Rectangle<int> area)
{
    int best = -1;
    float bestDist = 22.0f;
    for (int i = 0; i < ParametricEQ::kMaxBands; ++i)
    {
        auto& st = eq.getBandState(i);
        if (!st.enabled) continue;

        float bx = area.getX() + freqToX(st.frequency, (float)area.getWidth());
        float by = area.getY() + dbToY(st.gainDB, (float)area.getHeight());

        if (st.filterType == (int)EQFilterType::LowCut || st.filterType == (int)EQFilterType::HighCut)
            by = area.getY() + dbToY(0.0f, (float)area.getHeight());

        float dist = std::sqrt((x - bx) * (x - bx) + (y - by) * (y - by));
        if (dist < bestDist) { bestDist = dist; best = i; }
    }
    return best;
}

void EQPanel::mouseDown(const juce::MouseEvent& e)
{
    auto visArea = getVisArea();
    if (!visArea.contains(e.getPosition())) return;

    int band = findNearestBand((float)e.x, (float)e.y, visArea);

    if (e.mods.isRightButtonDown() && band >= 0)
    {
        selectedBand = band;
        showBandContextMenu(band);
        return;
    }

    if (band >= 0)
    {
        selectedBand = band;
        dragBand = band;
    }
    else
    {
        selectedBand = -1;
    }
    repaint();
}

void EQPanel::mouseDrag(const juce::MouseEvent& e)
{
    if (dragBand < 0) return;
    auto visArea = getVisArea();

    float freq = xToFreq((float)(e.x - visArea.getX()), (float)visArea.getWidth());
    float dB = yToDB((float)(e.y - visArea.getY()), (float)visArea.getHeight());

    freq = juce::jlimit(20.0f, 20000.0f, freq);
    dB = juce::jlimit(-24.0f, 24.0f, dB);

    auto si = juce::String(dragBand);
    if (auto* p = apvts.getParameter("eqFreq" + si))
        p->setValueNotifyingHost(p->convertTo0to1(freq));

    auto& st = eq.getBandState(dragBand);
    if (st.filterType != (int)EQFilterType::LowCut && st.filterType != (int)EQFilterType::HighCut)
    {
        if (auto* p = apvts.getParameter("eqGain" + si))
            p->setValueNotifyingHost(p->convertTo0to1(dB));
    }
}

void EQPanel::mouseUp(const juce::MouseEvent&)
{
    dragBand = -1;
}

void EQPanel::mouseDoubleClick(const juce::MouseEvent& e)
{
    auto visArea = getVisArea();
    int band = findNearestBand((float)e.x, (float)e.y, visArea);

    if (band >= 0)
    {
        auto si = juce::String(band);
        if (auto* p = apvts.getParameter("eqGain" + si))
            p->setValueNotifyingHost(p->convertTo0to1(0.0f));
    }
}

void EQPanel::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    auto visArea = getVisArea();
    int band = findNearestBand((float)e.x, (float)e.y, visArea);
    if (band < 0) return;

    auto si = juce::String(band);
    if (auto* p = apvts.getParameter("eqQ" + si))
    {
        float current = p->convertFrom0to1(p->getValue());
        float delta = wheel.deltaY * 0.5f;
        float newVal = juce::jlimit(0.05f, 30.0f, current + delta);
        p->setValueNotifyingHost(p->convertTo0to1(newVal));
    }
}

void EQPanel::showBandContextMenu(int bandIdx)
{
    juce::PopupMenu menu;
    auto si = juce::String(bandIdx);
    auto& st = eq.getBandState(bandIdx);

    menu.addItem(1, st.enabled ? "Disable Band" : "Enable Band");
    menu.addSeparator();

    juce::PopupMenu shapeMenu;
    juce::StringArray types = { "Peak", "Low Shelf", "High Shelf", "Low Cut", "High Cut", "Band Pass", "Notch" };
    for (int i = 0; i < types.size(); ++i)
        shapeMenu.addItem(100 + i, types[i], true, i == st.filterType);
    menu.addSubMenu("Shape", shapeMenu);

    menu.addSeparator();
    menu.addItem(10, "Reset to 0 dB");

    auto safeThis = juce::Component::SafePointer<EQPanel>(this);
    menu.showMenuAsync(juce::PopupMenu::Options(), [safeThis, bandIdx, si](int result) {
        if (safeThis == nullptr) return;
        auto& apvtsRef = safeThis->apvts;
        if (result == 1)
        {
            if (auto* p = apvtsRef.getParameter("eqOn" + si))
            {
                bool current = p->getValue() > 0.5f;
                p->setValueNotifyingHost(current ? 0.0f : 1.0f);
            }
        }
        else if (result == 10)
        {
            if (auto* p = apvtsRef.getParameter("eqGain" + si))
                p->setValueNotifyingHost(p->convertTo0to1(0.0f));
        }
        else if (result >= 100 && result < 107)
        {
            if (auto* p = apvtsRef.getParameter("eqType" + si))
                p->setValueNotifyingHost(p->convertTo0to1((float)(result - 100)));
        }
    });
}

void EQPanel::paint(juce::Graphics& g)
{
    auto& t = getThemeFrom(this);
    auto area = getLocalBounds();
    auto infoBar = area.removeFromBottom(50);
    auto visArea = area;

    g.setColour(t.panelBg);
    g.fillRect(visArea);

    drawGrid(g, visArea);
    drawSpectrum(g, visArea, preData, t.spectrumPre);
    drawSpectrum(g, visArea, postData, t.spectrumPost);

    for (int i = 0; i < ParametricEQ::kMaxBands; ++i)
    {
        if (eq.getBandState(i).enabled)
            drawBandCurve(g, visArea, i, getBandColour(i));
    }

    drawCompositeCurve(g, visArea);
    drawBandNodes(g, visArea);

    g.setColour(t.panelBg.darker(0.1f));
    g.fillRect(infoBar);
    drawBandInfo(g, infoBar);
}

void EQPanel::resized()
{
    // No knobs needed - everything is mouse-interactive
}
