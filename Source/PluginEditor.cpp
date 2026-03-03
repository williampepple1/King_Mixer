#include "PluginEditor.h"

static constexpr int kHeaderHeight   = 60;
static constexpr int kRowHeight      = 70;
static constexpr int kLabelHeight    = 16;
static constexpr int kKnobSize       = 54;
static constexpr int kSectionLabelW  = 75;
static constexpr int kFooterHeight   = 50;
static constexpr int kMargin         = 6;

// ============================================================
// RotarySliderWithLabel
// ============================================================
RotarySliderWithLabel::RotarySliderWithLabel(const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(11.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(label);
}

void RotarySliderWithLabel::resized()
{
    auto area = getLocalBounds();
    label.setBounds(area.removeFromTop(kLabelHeight));
    slider.setBounds(area);
}

// ============================================================
// AssistedMixingEditor
// ============================================================
AssistedMixingEditor::AssistedMixingEditor(AssistedMixingProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    auto& apvts = processorRef.getAPVTS();

    // Genre / Instrument combo boxes
    auto genreNames = getGenreNames();
    for (int i = 0; i < genreNames.size(); ++i)
        genreBox.addItem(genreNames[i], i + 1);
    addAndMakeVisible(genreBox);
    genreAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "genre", genreBox);

    auto instrNames = getInstrumentNames();
    for (int i = 0; i < instrNames.size(); ++i)
        instrumentBox.addItem(instrNames[i], i + 1);
    addAndMakeVisible(instrumentBox);
    instrumentAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "instrument", instrumentBox);

    applyRuleButton.addListener(this);
    addAndMakeVisible(applyRuleButton);

    // Knob attachments
    setupSlider(inputGainSlider,     inputGainAttach,     "inputGain");
    setupSlider(outputGainSlider,    outputGainAttach,    "outputGain");

    setupSlider(eqLowFreqSlider,     eqLowFreqAttach,     "eqLowFreq");
    setupSlider(eqLowGainSlider,     eqLowGainAttach,     "eqLowGain");
    setupSlider(eqLowMidFreqSlider,  eqLowMidFreqAttach,  "eqLowMidFreq");
    setupSlider(eqLowMidGainSlider,  eqLowMidGainAttach,  "eqLowMidGain");
    setupSlider(eqLowMidQSlider,     eqLowMidQAttach,     "eqLowMidQ");
    setupSlider(eqHighMidFreqSlider, eqHighMidFreqAttach, "eqHighMidFreq");
    setupSlider(eqHighMidGainSlider, eqHighMidGainAttach, "eqHighMidGain");
    setupSlider(eqHighMidQSlider,    eqHighMidQAttach,    "eqHighMidQ");
    setupSlider(eqHighFreqSlider,    eqHighFreqAttach,    "eqHighFreq");
    setupSlider(eqHighGainSlider,    eqHighGainAttach,    "eqHighGain");

    setupSlider(compThreshSlider,    compThreshAttach,    "compThreshold");
    setupSlider(compRatioSlider,     compRatioAttach,     "compRatio");
    setupSlider(compAttackSlider,    compAttackAttach,    "compAttack");
    setupSlider(compReleaseSlider,   compReleaseAttach,   "compRelease");
    setupSlider(compMakeupSlider,    compMakeupAttach,    "compMakeup");

    setupSlider(satDriveSlider,      satDriveAttach,      "satDrive");
    setupSlider(satMixSlider,        satMixAttach,        "satMix");

    setupSlider(stereoWidthSlider,   stereoWidthAttach,   "stereoWidth");
    setupSlider(reverbSendSlider,    reverbSendAttach,    "reverbSend");
    setupSlider(mixAmountSlider,     mixAmountAttach,     "mixAmount");

    bypassToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(bypassToggle);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "bypass", bypassToggle);

    setSize(720, kHeaderHeight + kRowHeight * 6 + kFooterHeight + kMargin * 2);
}

AssistedMixingEditor::~AssistedMixingEditor()
{
    delete inputGainAttach;
    delete outputGainAttach;
    delete eqLowFreqAttach;
    delete eqLowGainAttach;
    delete eqLowMidFreqAttach;
    delete eqLowMidGainAttach;
    delete eqLowMidQAttach;
    delete eqHighMidFreqAttach;
    delete eqHighMidGainAttach;
    delete eqHighMidQAttach;
    delete eqHighFreqAttach;
    delete eqHighGainAttach;
    delete compThreshAttach;
    delete compRatioAttach;
    delete compAttackAttach;
    delete compReleaseAttach;
    delete compMakeupAttach;
    delete satDriveAttach;
    delete satMixAttach;
    delete stereoWidthAttach;
    delete reverbSendAttach;
    delete mixAmountAttach;
}

void AssistedMixingEditor::setupSlider(RotarySliderWithLabel& s,
                                        juce::AudioProcessorValueTreeState::SliderAttachment*& att,
                                        const juce::String& paramId)
{
    addAndMakeVisible(s);
    att = new juce::AudioProcessorValueTreeState::SliderAttachment(
        processorRef.getAPVTS(), paramId, s.getSlider());
}

void AssistedMixingEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Title bar
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, 0, getWidth(), kHeaderHeight);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("Assisted Mixing", 10, 0, 200, kHeaderHeight, juce::Justification::centredLeft);

    // Section labels
    int y = kHeaderHeight + kMargin;
    drawSectionLabel(g, y, "GAIN");
    y += kRowHeight;
    drawSectionLabel(g, y, "EQ");
    y += kRowHeight * 2;
    drawSectionLabel(g, y, "COMP");
    y += kRowHeight;
    drawSectionLabel(g, y, "SAT / ST / REV");
    y += kRowHeight;

    // Footer bar
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, getHeight() - kFooterHeight, getWidth(), kFooterHeight);
}

void AssistedMixingEditor::drawSectionLabel(juce::Graphics& g, int y, const juce::String& text)
{
    g.setColour(juce::Colour(0xff0f3460));
    g.fillRoundedRectangle(2.0f, (float)y + 2.0f, (float)kSectionLabelW - 4.0f, (float)kRowHeight - 4.0f, 4.0f);
    g.setColour(juce::Colour(0xffe94560));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawFittedText(text, 2, y, kSectionLabelW - 4, kRowHeight, juce::Justification::centred, 2);
}

void AssistedMixingEditor::resized()
{
    auto area = getLocalBounds();

    // Header: combo boxes + apply button
    auto header = area.removeFromTop(kHeaderHeight);
    header.removeFromLeft(210);
    genreBox.setBounds(header.removeFromLeft(130).reduced(4, 16));
    instrumentBox.setBounds(header.removeFromLeft(150).reduced(4, 16));
    applyRuleButton.setBounds(header.removeFromLeft(100).reduced(4, 16));

    area.removeFromTop(kMargin);

    int knobW = kKnobSize + kMargin;

    // Row 1: Gain
    {
        auto row = area.removeFromTop(kRowHeight);
        row.removeFromLeft(kSectionLabelW);
        inputGainSlider.setBounds(row.removeFromLeft(knobW));
        outputGainSlider.setBounds(row.removeFromLeft(knobW));
    }

    // Row 2-3: EQ (4 bands, split into two rows)
    {
        auto row = area.removeFromTop(kRowHeight);
        row.removeFromLeft(kSectionLabelW);
        eqLowFreqSlider.setBounds(row.removeFromLeft(knobW));
        eqLowGainSlider.setBounds(row.removeFromLeft(knobW));
        row.removeFromLeft(kMargin);
        eqLowMidFreqSlider.setBounds(row.removeFromLeft(knobW));
        eqLowMidGainSlider.setBounds(row.removeFromLeft(knobW));
        eqLowMidQSlider.setBounds(row.removeFromLeft(knobW));
    }
    {
        auto row = area.removeFromTop(kRowHeight);
        row.removeFromLeft(kSectionLabelW);
        eqHighMidFreqSlider.setBounds(row.removeFromLeft(knobW));
        eqHighMidGainSlider.setBounds(row.removeFromLeft(knobW));
        eqHighMidQSlider.setBounds(row.removeFromLeft(knobW));
        row.removeFromLeft(kMargin);
        eqHighFreqSlider.setBounds(row.removeFromLeft(knobW));
        eqHighGainSlider.setBounds(row.removeFromLeft(knobW));
    }

    // Row 4: Compressor
    {
        auto row = area.removeFromTop(kRowHeight);
        row.removeFromLeft(kSectionLabelW);
        compThreshSlider.setBounds(row.removeFromLeft(knobW));
        compRatioSlider.setBounds(row.removeFromLeft(knobW));
        compAttackSlider.setBounds(row.removeFromLeft(knobW));
        compReleaseSlider.setBounds(row.removeFromLeft(knobW));
        compMakeupSlider.setBounds(row.removeFromLeft(knobW));
    }

    // Row 5: Saturation + Stereo + Reverb
    {
        auto row = area.removeFromTop(kRowHeight);
        row.removeFromLeft(kSectionLabelW);
        satDriveSlider.setBounds(row.removeFromLeft(knobW));
        satMixSlider.setBounds(row.removeFromLeft(knobW));
        row.removeFromLeft(kMargin);
        stereoWidthSlider.setBounds(row.removeFromLeft(knobW));
        row.removeFromLeft(kMargin);
        reverbSendSlider.setBounds(row.removeFromLeft(knobW));
    }

    // Footer: Mix Amount + Bypass
    auto footer = area.removeFromBottom(kFooterHeight);
    footer.removeFromLeft(kSectionLabelW);
    mixAmountSlider.setBounds(footer.removeFromLeft(knobW + 30).withHeight(kFooterHeight));
    bypassToggle.setBounds(footer.removeFromLeft(80).reduced(4, 12));
}

void AssistedMixingEditor::buttonClicked(juce::Button* button)
{
    if (button == &applyRuleButton)
    {
        auto genreIdx = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("genre")->load());
        auto instrIdx = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("instrument")->load());

        processorRef.applyRule(static_cast<Genre>(genreIdx),
                               static_cast<Instrument>(instrIdx));
    }
}
