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

    // Theme selector
    auto themeNames = KingMixerThemes::getNames();
    for (int i = 0; i < themeNames.size(); ++i)
        themeBox.addItem(themeNames[i], i + 1);
    themeBox.addListener(this);
    addAndMakeVisible(themeBox);

    int savedTheme = processorRef.getThemeIndex();
    themeBox.setSelectedId(savedTheme + 1, juce::dontSendNotification);
    applyTheme(savedTheme);

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
        processorRef.getDryRevBuffer(), processorRef.getWetRevBuffer(),
        processorRef.getReverbSend());
    addAndMakeVisible(reverbPanel.get());

    gainMixPanel = std::make_unique<GainMixPanel>(apvts,
        processorRef.getInputMeter(), processorRef.getOutputMeter());
    addAndMakeVisible(gainMixPanel.get());

    showTab(TabEQ);

    setSize(900, 650);
    startTimerHz(30);
}

AssistedMixingEditor::~AssistedMixingEditor()
{
    stopTimer();

    gainMixPanel.reset();
    reverbPanel.reset();
    satPanel.reset();
    compPanel.reset();
    eqPanel.reset();

    genreAttach.reset();
    instrumentAttach.reset();

    setLookAndFeel(nullptr);
}

void AssistedMixingEditor::timerCallback()
{
}

void AssistedMixingEditor::applyTheme(int themeIndex)
{
    customLnf.setTheme(KingMixerThemes::getByIndex(themeIndex));
    processorRef.setThemeIndex(themeIndex);
    repaint();
}

void AssistedMixingEditor::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &themeBox)
    {
        int idx = themeBox.getSelectedId() - 1;
        if (idx >= 0 && idx < KingMixerThemes::getCount())
            applyTheme(idx);
    }
}

void AssistedMixingEditor::showTab(int index)
{
    if (index < 0 || index >= NumTabs) return;
    activeTab = index;

    if (eqPanel) eqPanel->setVisible(index == TabEQ);
    if (compPanel) compPanel->setVisible(index == TabComp);
    if (satPanel) satPanel->setVisible(index == TabSat);
    if (reverbPanel) reverbPanel->setVisible(index == TabReverb);
    if (gainMixPanel) gainMixPanel->setVisible(index == TabGainMix);

    for (int i = 0; i < NumTabs; ++i)
        tabButtons[(size_t)i].setToggleState(i == index, juce::dontSendNotification);

    repaint();
}

void AssistedMixingEditor::buttonClicked(juce::Button* button)
{
    if (button == &applyRuleButton)
    {
        auto* genreParam = processorRef.getAPVTS().getRawParameterValue("genre");
        auto* instrParam = processorRef.getAPVTS().getRawParameterValue("instrument");
        if (genreParam && instrParam)
        {
            auto genreIdx = static_cast<int>(genreParam->load());
            auto instrIdx = static_cast<int>(instrParam->load());
            processorRef.applyRule(static_cast<Genre>(genreIdx), static_cast<Instrument>(instrIdx));
        }
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
    auto& t = customLnf.getTheme();

    g.fillAll(t.background);

    // Header
    g.setColour(t.headerBg);
    g.fillRect(0, 0, getWidth(), kHeaderHeight);

    g.setColour(t.textBright);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("King Mixer", 12, 0, 150, kHeaderHeight, juce::Justification::centredLeft);

    // Tab bar background
    int tabY = kHeaderHeight;
    g.setColour(t.tabInactive);
    g.fillRect(0, tabY, getWidth(), kTabBarHeight);

    if (NumTabs > 0)
    {
        int tabW = getWidth() / NumTabs;

        g.setColour(t.tabActive);
        g.fillRect(activeTab * tabW, tabY, tabW, kTabBarHeight);

        g.setColour(t.accent);
        g.fillRect(activeTab * tabW, tabY + kTabBarHeight - 3, tabW, 3);
    }
}

void AssistedMixingEditor::resized()
{
    auto area = getLocalBounds();

    // Header
    auto header = area.removeFromTop(kHeaderHeight);
    header.removeFromLeft(160);
    genreBox.setBounds(header.removeFromLeft(110).reduced(4, 12));
    instrumentBox.setBounds(header.removeFromLeft(120).reduced(4, 12));
    applyRuleButton.setBounds(header.removeFromLeft(80).reduced(4, 12));

    // Theme selector on the right side of the header
    auto themeArea = header.removeFromRight(120).reduced(4, 12);
    themeBox.setBounds(themeArea);

    // Tab bar
    auto tabBar = area.removeFromTop(kTabBarHeight);

    if (NumTabs > 0)
    {
        int tabW = tabBar.getWidth() / NumTabs;
        for (int i = 0; i < NumTabs; ++i)
            tabButtons[(size_t)i].setBounds(tabBar.removeFromLeft(tabW));
    }

    // Panel content
    auto panelArea = area;
    if (eqPanel) eqPanel->setBounds(panelArea);
    if (compPanel) compPanel->setBounds(panelArea);
    if (satPanel) satPanel->setBounds(panelArea);
    if (reverbPanel) reverbPanel->setBounds(panelArea);
    if (gainMixPanel) gainMixPanel->setBounds(panelArea);
}
