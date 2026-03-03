#include "PluginEditor.h"

static constexpr int kHeaderHeight = 50;
static constexpr int kTabBarHeight = 32;

AssistedMixingEditor::AssistedMixingEditor(AssistedMixingProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&customLnf);

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

    // Tab buttons
    juce::StringArray tabNames = { "EQ", "COMP", "SAT", "REVERB", "GAIN/MIX" };
    for (int i = 0; i < NumTabs; ++i)
    {
        tabButtons[(size_t)i].setButtonText(tabNames[i]);
        tabButtons[(size_t)i].addListener(this);
        tabButtons[(size_t)i].setClickingTogglesState(false);
        addAndMakeVisible(tabButtons[(size_t)i]);
    }

    // Create panels
    eqPanel = std::make_unique<EQPanel>(apvts,
        processorRef.getPreEQAnalyzer(), processorRef.getPostEQAnalyzer(),
        processorRef.getParametricEQ());
    addAndMakeVisible(eqPanel.get());

    compPanel = std::make_unique<CompressorPanel>(apvts, processorRef.getCompressor());
    addAndMakeVisible(compPanel.get());

    satPanel = std::make_unique<SaturationPanel>(apvts,
        processorRef.getPreSatBuffer(), processorRef.getPostSatBuffer());
    addAndMakeVisible(satPanel.get());

    reverbPanel = std::make_unique<ReverbPanel>(apvts,
        processorRef.getDryRevBuffer(), processorRef.getWetRevBuffer());
    addAndMakeVisible(reverbPanel.get());

    gainMixPanel = std::make_unique<GainMixPanel>(apvts,
        processorRef.getInputMeter(), processorRef.getOutputMeter());
    addAndMakeVisible(gainMixPanel.get());

    showTab(TabEQ);

    setSize(900, 650);
}

AssistedMixingEditor::~AssistedMixingEditor()
{
    setLookAndFeel(nullptr);
}

void AssistedMixingEditor::showTab(int index)
{
    activeTab = index;

    eqPanel->setVisible(index == TabEQ);
    compPanel->setVisible(index == TabComp);
    satPanel->setVisible(index == TabSat);
    reverbPanel->setVisible(index == TabReverb);
    gainMixPanel->setVisible(index == TabGainMix);

    for (int i = 0; i < NumTabs; ++i)
        tabButtons[(size_t)i].setToggleState(i == index, juce::dontSendNotification);

    repaint();
}

void AssistedMixingEditor::buttonClicked(juce::Button* button)
{
    if (button == &applyRuleButton)
    {
        auto genreIdx = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("genre")->load());
        auto instrIdx = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("instrument")->load());
        processorRef.applyRule(static_cast<Genre>(genreIdx), static_cast<Instrument>(instrIdx));
        return;
    }

    for (int i = 0; i < NumTabs; ++i)
    {
        if (button == &tabButtons[(size_t)i])
        {
            showTab(i);
            return;
        }
    }
}

void AssistedMixingEditor::paint(juce::Graphics& g)
{
    g.fillAll(KingMixerColours::background);

    // Header
    g.setColour(KingMixerColours::headerBg);
    g.fillRect(0, 0, getWidth(), kHeaderHeight);

    g.setColour(KingMixerColours::textBright);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("King Mixer", 12, 0, 180, kHeaderHeight, juce::Justification::centredLeft);

    // Tab bar background
    int tabY = kHeaderHeight;
    g.setColour(KingMixerColours::tabInactive);
    g.fillRect(0, tabY, getWidth(), kTabBarHeight);

    // Active tab highlight
    int tabW = getWidth() / NumTabs;
    g.setColour(KingMixerColours::tabActive);
    g.fillRect(activeTab * tabW, tabY, tabW, kTabBarHeight);

    // Active tab accent line
    g.setColour(KingMixerColours::accent);
    g.fillRect(activeTab * tabW, tabY + kTabBarHeight - 3, tabW, 3);
}

void AssistedMixingEditor::resized()
{
    auto area = getLocalBounds();

    // Header
    auto header = area.removeFromTop(kHeaderHeight);
    header.removeFromLeft(190);
    genreBox.setBounds(header.removeFromLeft(130).reduced(4, 12));
    instrumentBox.setBounds(header.removeFromLeft(155).reduced(4, 12));
    applyRuleButton.setBounds(header.removeFromLeft(110).reduced(4, 12));

    // Tab bar
    auto tabBar = area.removeFromTop(kTabBarHeight);
    int tabW = tabBar.getWidth() / NumTabs;
    for (int i = 0; i < NumTabs; ++i)
        tabButtons[(size_t)i].setBounds(tabBar.removeFromLeft(tabW));

    // Panel content
    auto panelArea = area;
    eqPanel->setBounds(panelArea);
    compPanel->setBounds(panelArea);
    satPanel->setBounds(panelArea);
    reverbPanel->setBounds(panelArea);
    gainMixPanel->setBounds(panelArea);
}
