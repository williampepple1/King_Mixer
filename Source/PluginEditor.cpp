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

    // Master bus toggle
    masterBusToggle.setToggleState(processorRef.isMasterBus(), juce::dontSendNotification);
    masterBusToggle.addListener(this);
    addAndMakeVisible(masterBusToggle);

    // Track name label (editable)
    trackNameLabel.setText(processorRef.getTrackName(), juce::dontSendNotification);
    trackNameLabel.setEditable(true);
    trackNameLabel.setJustificationType(juce::Justification::centred);
    trackNameLabel.setFont(juce::Font(11.0f));
    trackNameLabel.addListener(this);
    addAndMakeVisible(trackNameLabel);

    // Tab buttons
    juce::StringArray tabNames = { "EQ", "COMP", "SAT", "REVERB", "GAIN/MIX", "MASTER" };
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

    masterBusPanel = std::make_unique<MasterBusPanel>(apvts);
    addAndMakeVisible(masterBusPanel.get());

    rebuildTabBar();
    showTab(TabEQ);

    setSize(900, 650);
    startTimerHz(30);
}

AssistedMixingEditor::~AssistedMixingEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void AssistedMixingEditor::timerCallback()
{
    processorRef.consumePendingPush();
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

void AssistedMixingEditor::labelTextChanged(juce::Label* label)
{
    if (label == &trackNameLabel)
    {
        processorRef.setTrackName(trackNameLabel.getText());
    }
}

void AssistedMixingEditor::rebuildTabBar()
{
    bool isMaster = processorRef.isMasterBus();
    tabButtons[TabMaster].setVisible(isMaster);

    if (!isMaster && activeTab == TabMaster)
        showTab(TabEQ);
}

void AssistedMixingEditor::showTab(int index)
{
    activeTab = index;

    eqPanel->setVisible(index == TabEQ);
    compPanel->setVisible(index == TabComp);
    satPanel->setVisible(index == TabSat);
    reverbPanel->setVisible(index == TabReverb);
    gainMixPanel->setVisible(index == TabGainMix);
    masterBusPanel->setVisible(index == TabMaster);

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

    if (button == &masterBusToggle)
    {
        processorRef.setMasterBusMode(masterBusToggle.getToggleState());
        rebuildTabBar();
        if (masterBusToggle.getToggleState())
            showTab(TabMaster);
        resized();
        repaint();
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

    // Count visible tabs for layout
    int visibleTabs = 0;
    for (int i = 0; i < NumTabs; ++i)
        if (tabButtons[i].isVisible()) visibleTabs++;

    if (visibleTabs > 0)
    {
        int tabW = getWidth() / visibleTabs;

        // Find which visible tab position the active tab is at
        int activeVisiblePos = 0;
        int pos = 0;
        for (int i = 0; i < NumTabs; ++i)
        {
            if (tabButtons[i].isVisible())
            {
                if (i == activeTab) activeVisiblePos = pos;
                pos++;
            }
        }

        g.setColour(t.tabActive);
        g.fillRect(activeVisiblePos * tabW, tabY, tabW, kTabBarHeight);

        g.setColour(t.accent);
        g.fillRect(activeVisiblePos * tabW, tabY + kTabBarHeight - 3, tabW, 3);
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

    // Track name (editable label)
    trackNameLabel.setBounds(header.removeFromLeft(90).reduced(4, 14));

    // Master toggle
    masterBusToggle.setBounds(header.removeFromLeft(70).reduced(2, 14));

    // Theme selector on the right side of the header
    auto themeArea = header.removeFromRight(120).reduced(4, 12);
    themeBox.setBounds(themeArea);

    // Tab bar
    auto tabBar = area.removeFromTop(kTabBarHeight);

    int visibleTabs = 0;
    for (int i = 0; i < NumTabs; ++i)
        if (tabButtons[i].isVisible()) visibleTabs++;

    if (visibleTabs > 0)
    {
        int tabW = tabBar.getWidth() / visibleTabs;
        for (int i = 0; i < NumTabs; ++i)
        {
            if (tabButtons[i].isVisible())
                tabButtons[(size_t)i].setBounds(tabBar.removeFromLeft(tabW));
        }
    }

    // Panel content
    auto panelArea = area;
    eqPanel->setBounds(panelArea);
    compPanel->setBounds(panelArea);
    satPanel->setBounds(panelArea);
    reverbPanel->setBounds(panelArea);
    gainMixPanel->setBounds(panelArea);
    masterBusPanel->setBounds(panelArea);
}
