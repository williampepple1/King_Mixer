#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
    : theme(KingMixerThemes::midnight())
{
    applyThemeColours();
}

void CustomLookAndFeel::setTheme(const KingMixerTheme& newTheme)
{
    theme = newTheme;
    applyThemeColours();
}

void CustomLookAndFeel::applyThemeColours()
{
    setColour(juce::Slider::textBoxTextColourId, theme.textBright);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId, theme.knobFill);
    setColour(juce::ComboBox::textColourId, theme.textBright);
    setColour(juce::ComboBox::arrowColourId, theme.accent);
    setColour(juce::ComboBox::outlineColourId, theme.accentDim);
    setColour(juce::PopupMenu::backgroundColourId, theme.panelBg);
    setColour(juce::PopupMenu::textColourId, theme.textBright);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.accent);
    setColour(juce::Label::textColourId, theme.textDim);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider&)
{
    float radius = (float)juce::jmin(width, height) * 0.4f;
    float centreX = (float)x + (float)width * 0.5f;
    float centreY = (float)y + (float)height * 0.5f;
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(theme.knobFill);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    juce::Path bgArc;
    bgArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f,
                         rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(theme.gridLine);
    g.strokePath(bgArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f,
                            rotaryStartAngle, angle, true);
    g.setColour(theme.knobArc);
    g.strokePath(valueArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    float pointerLength = radius * 0.6f;
    pointer.addRectangle(-1.5f, -pointerLength, 3.0f, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(theme.knobPointer);
    g.fillPath(pointer);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                      int, int, int, int, juce::ComboBox&)
{
    g.setColour(theme.knobFill);
    g.fillRoundedRectangle(0.0f, 0.0f, (float)width, (float)height, 4.0f);
    g.setColour(theme.accentDim);
    g.drawRoundedRectangle(0.5f, 0.5f, (float)width - 1.0f, (float)height - 1.0f, 4.0f, 1.0f);

    juce::Path arrow;
    float arrowX = (float)width - 18.0f;
    float arrowY = (float)height * 0.5f;
    arrow.addTriangle(arrowX, arrowY - 3.0f, arrowX + 8.0f, arrowY - 3.0f, arrowX + 4.0f, arrowY + 3.0f);
    g.setColour(theme.accent);
    g.fillPath(arrow);
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour&,
                                              bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto colour = down ? theme.accent : (highlighted ? theme.accentDim : theme.knobFill);
    g.setColour(colour);
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(theme.accent);
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                          bool highlighted, bool)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    auto on = button.getToggleState();

    g.setColour(on ? theme.accentWarm : theme.knobFill);
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(highlighted ? theme.accent : theme.accentDim);
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    g.setColour(theme.textBright);
    g.setFont(12.0f);
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return juce::Font(13.0f);
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton&, int)
{
    return juce::Font(13.0f);
}
