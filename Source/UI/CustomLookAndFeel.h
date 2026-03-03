#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct KingMixerTheme
{
    juce::String name;

    juce::Colour background;
    juce::Colour panelBg;
    juce::Colour headerBg;
    juce::Colour tabActive;
    juce::Colour tabInactive;
    juce::Colour accent;
    juce::Colour accentDim;
    juce::Colour accentWarm;
    juce::Colour gridLine;
    juce::Colour textBright;
    juce::Colour textDim;
    juce::Colour spectrumPre;
    juce::Colour spectrumPost;
    juce::Colour eqCurve;
    juce::Colour grMeter;
    juce::Colour grTimeline;
    juce::Colour meterGreen;
    juce::Colour meterYellow;
    juce::Colour meterRed;
    juce::Colour knobFill;
    juce::Colour knobArc;
    juce::Colour knobPointer;
};

namespace KingMixerThemes
{
    inline KingMixerTheme midnight()
    {
        return {
            "Midnight",
            juce::Colour(0xff0d0d1a), juce::Colour(0xff141428), juce::Colour(0xff0a0a1e),
            juce::Colour(0xff1a1a3e), juce::Colour(0xff0e0e22),
            juce::Colour(0xff4fc3f7), juce::Colour(0xff1a6fa0), juce::Colour(0xffff7043),
            juce::Colour(0xff1e1e3a),
            juce::Colour(0xffe0e0e0), juce::Colour(0xff808090),
            juce::Colour(0x554fc3f7), juce::Colour(0xaa4fc3f7), juce::Colour(0xffffffff),
            juce::Colour(0xffff5252), juce::Colour(0xccff5252),
            juce::Colour(0xff66bb6a), juce::Colour(0xffffca28), juce::Colour(0xffef5350),
            juce::Colour(0xff252545), juce::Colour(0xff4fc3f7), juce::Colour(0xffffffff)
        };
    }

    inline KingMixerTheme charcoal()
    {
        return {
            "Charcoal",
            juce::Colour(0xff1a1a1a), juce::Colour(0xff242424), juce::Colour(0xff151515),
            juce::Colour(0xff333333), juce::Colour(0xff1e1e1e),
            juce::Colour(0xff00e676), juce::Colour(0xff1b7a3d), juce::Colour(0xffff9100),
            juce::Colour(0xff303030),
            juce::Colour(0xffe0e0e0), juce::Colour(0xff8a8a8a),
            juce::Colour(0x5500e676), juce::Colour(0xaa00e676), juce::Colour(0xffffffff),
            juce::Colour(0xffff5252), juce::Colour(0xccff5252),
            juce::Colour(0xff66bb6a), juce::Colour(0xffffca28), juce::Colour(0xffef5350),
            juce::Colour(0xff2a2a2a), juce::Colour(0xff00e676), juce::Colour(0xffffffff)
        };
    }

    inline KingMixerTheme mocha()
    {
        return {
            "Mocha",
            juce::Colour(0xff1e1510), juce::Colour(0xff2a1f18), juce::Colour(0xff181010),
            juce::Colour(0xff3a2a1e), juce::Colour(0xff201810),
            juce::Colour(0xffe8a87c), juce::Colour(0xff8b5e3c), juce::Colour(0xffff6b6b),
            juce::Colour(0xff332820),
            juce::Colour(0xfff0dcc8), juce::Colour(0xff9a8878),
            juce::Colour(0x55e8a87c), juce::Colour(0xaae8a87c), juce::Colour(0xfff0dcc8),
            juce::Colour(0xffff5252), juce::Colour(0xccff5252),
            juce::Colour(0xff66bb6a), juce::Colour(0xffffca28), juce::Colour(0xffef5350),
            juce::Colour(0xff302218), juce::Colour(0xffe8a87c), juce::Colour(0xfff0dcc8)
        };
    }

    inline KingMixerTheme royalGold()
    {
        return {
            "Royal Gold",
            juce::Colour(0xff0f0f0f), juce::Colour(0xff1a1a1a), juce::Colour(0xff0a0a0a),
            juce::Colour(0xff282218), juce::Colour(0xff141414),
            juce::Colour(0xffffd700), juce::Colour(0xff8b7500), juce::Colour(0xffff4444),
            juce::Colour(0xff252520),
            juce::Colour(0xfff5e6c8), juce::Colour(0xff9a9080),
            juce::Colour(0x55ffd700), juce::Colour(0xaaffd700), juce::Colour(0xfff5e6c8),
            juce::Colour(0xffff5252), juce::Colour(0xccff5252),
            juce::Colour(0xff66bb6a), juce::Colour(0xffffca28), juce::Colour(0xffef5350),
            juce::Colour(0xff222218), juce::Colour(0xffffd700), juce::Colour(0xfff5e6c8)
        };
    }

    inline KingMixerTheme arctic()
    {
        return {
            "Arctic",
            juce::Colour(0xfff0f4f8), juce::Colour(0xffe2e8f0), juce::Colour(0xffdce3ec),
            juce::Colour(0xffcbd5e1), juce::Colour(0xffe8edf2),
            juce::Colour(0xff3b82f6), juce::Colour(0xff6ea8f7), juce::Colour(0xffef4444),
            juce::Colour(0xffc8d4e0),
            juce::Colour(0xff1e293b), juce::Colour(0xff64748b),
            juce::Colour(0x553b82f6), juce::Colour(0xaa3b82f6), juce::Colour(0xff1e293b),
            juce::Colour(0xffef4444), juce::Colour(0xccef4444),
            juce::Colour(0xff22c55e), juce::Colour(0xffeab308), juce::Colour(0xffef4444),
            juce::Colour(0xffd0d8e4), juce::Colour(0xff3b82f6), juce::Colour(0xff1e293b)
        };
    }

    inline KingMixerTheme getByIndex(int index)
    {
        switch (index)
        {
            case 0: return charcoal();
            case 1: return midnight();
            case 2: return mocha();
            case 3: return royalGold();
            case 4: return arctic();
            default: return charcoal();
        }
    }

    inline juce::StringArray getNames()
    {
        return { "Charcoal", "Midnight", "Mocha", "Royal Gold", "Arctic" };
    }

    inline int getCount() { return 5; }
}

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();

    void setTheme(const KingMixerTheme& newTheme);
    const KingMixerTheme& getTheme() const { return theme; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;

private:
    KingMixerTheme theme;
    void applyThemeColours();
};

inline const KingMixerTheme& getThemeFrom(juce::Component* comp)
{
    static KingMixerTheme fallback = KingMixerThemes::charcoal();
    if (comp == nullptr) return fallback;

    if (auto* lnf = dynamic_cast<CustomLookAndFeel*>(&comp->getLookAndFeel()))
        return lnf->getTheme();

    return fallback;
}
