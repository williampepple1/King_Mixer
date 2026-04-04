#include "PluginProcessor.h"
#include "PluginEditor.h"

AssistedMixingProcessor::AssistedMixingProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    inputGainParam    = apvts.getRawParameterValue("inputGain");
    outputGainParam   = apvts.getRawParameterValue("outputGain");
    for (int i = 0; i < 8; ++i)
    {
        auto si = juce::String(i);
        eqBandParams[i].freq    = apvts.getRawParameterValue("eqFreq" + si);
        eqBandParams[i].gain    = apvts.getRawParameterValue("eqGain" + si);
        eqBandParams[i].q       = apvts.getRawParameterValue("eqQ" + si);
        eqBandParams[i].type    = apvts.getRawParameterValue("eqType" + si);
        eqBandParams[i].enabled = apvts.getRawParameterValue("eqOn" + si);
    }
    compThresholdParam = apvts.getRawParameterValue("compThreshold");
    compRatioParam    = apvts.getRawParameterValue("compRatio");
    compAttackParam   = apvts.getRawParameterValue("compAttack");
    compReleaseParam  = apvts.getRawParameterValue("compRelease");
    compMakeupParam   = apvts.getRawParameterValue("compMakeup");
    satDriveParam     = apvts.getRawParameterValue("satDrive");
    satMixParam       = apvts.getRawParameterValue("satMix");
    stereoWidthParam  = apvts.getRawParameterValue("stereoWidth");
    revMixParam          = apvts.getRawParameterValue("revMix");
    revPredelayParam     = apvts.getRawParameterValue("revPredelay");
    revDecayParam        = apvts.getRawParameterValue("revDecay");
    revDampHiFreqParam   = apvts.getRawParameterValue("revDampHiFreq");
    revDampHiShelfParam  = apvts.getRawParameterValue("revDampHiShelf");
    revDampBassFreqParam = apvts.getRawParameterValue("revDampBassFreq");
    revDampBassMultParam = apvts.getRawParameterValue("revDampBassMult");
    revSizeParam         = apvts.getRawParameterValue("revSize");
    revAttackParam       = apvts.getRawParameterValue("revAttack");
    revEarlyDiffParam    = apvts.getRawParameterValue("revEarlyDiff");
    revLateDiffParam     = apvts.getRawParameterValue("revLateDiff");
    revModRateParam      = apvts.getRawParameterValue("revModRate");
    revModDepthParam     = apvts.getRawParameterValue("revModDepth");
    revEqHighCutParam    = apvts.getRawParameterValue("revEqHighCut");
    revEqLowCutParam     = apvts.getRawParameterValue("revEqLowCut");
    revModeParam         = apvts.getRawParameterValue("revMode");
    revColorParam        = apvts.getRawParameterValue("revColor");
    mixAmountParam    = apvts.getRawParameterValue("mixAmount");
    bypassParam       = apvts.getRawParameterValue("bypass");
}

AssistedMixingProcessor::~AssistedMixingProcessor()
{
}

const juce::String AssistedMixingProcessor::getName() const { return JucePlugin_Name; }
bool AssistedMixingProcessor::acceptsMidi() const { return false; }
bool AssistedMixingProcessor::producesMidi() const { return false; }
bool AssistedMixingProcessor::isMidiEffect() const { return false; }
double AssistedMixingProcessor::getTailLengthSeconds() const { return 0.0; }
int AssistedMixingProcessor::getNumPrograms() { return 1; }
int AssistedMixingProcessor::getCurrentProgram() { return 0; }
void AssistedMixingProcessor::setCurrentProgram(int) {}
const juce::String AssistedMixingProcessor::getProgramName(int) { return {}; }
void AssistedMixingProcessor::changeProgramName(int, const juce::String&) {}

void AssistedMixingProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    gainStage.prepare(spec);
    parametricEQ.prepare(spec);
    compressor.prepare(spec);
    saturation.prepare(spec);
    stereoWidth.prepare(spec);
    reverbSend.prepare(spec);

    dryBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    preEQAnalyzer.setSampleRate(sampleRate);
    postEQAnalyzer.setSampleRate(sampleRate);
}

void AssistedMixingProcessor::releaseResources() {}

bool AssistedMixingProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void AssistedMixingProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (!inputGainParam || !outputGainParam || !mixAmountParam)
        return;

    if (bypassParam && bypassParam->load() > 0.5f)
    {
        inputMeter.process(buffer);
        outputMeter.process(buffer);
        return;
    }

    const float mix = mixAmountParam->load();

    if (mix < 1.0f)
    {
        if (dryBuffer.getNumChannels() < buffer.getNumChannels() || dryBuffer.getNumSamples() < buffer.getNumSamples())
            dryBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            dryBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());
    }

    inputMeter.process(buffer);

    gainStage.setGainDB(inputGainParam->load());
    gainStage.process(buffer);

    if (buffer.getNumChannels() > 0)
        preEQAnalyzer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    for (int i = 0; i < 8; ++i)
    {
        auto& bp = eqBandParams[i];
        if (!bp.freq || !bp.gain || !bp.q || !bp.type || !bp.enabled)
            continue;

        auto* typeParam = apvts.getParameter("eqType" + juce::String(i));
        int typeIndex = typeParam ? juce::roundToInt(typeParam->convertFrom0to1(typeParam->getValue())) : 0;

        parametricEQ.updateBand(i,
            bp.freq->load(),
            bp.gain->load(),
            bp.q->load(),
            typeIndex,
            bp.enabled->load() > 0.5f,
            getSampleRate());
    }
    parametricEQ.process(buffer);

    if (buffer.getNumChannels() > 0)
        postEQAnalyzer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    if (compThresholdParam && compRatioParam && compAttackParam && compReleaseParam && compMakeupParam)
    {
        compressor.updateParameters(
            compThresholdParam->load(), compRatioParam->load(),
            compAttackParam->load(), compReleaseParam->load(),
            compMakeupParam->load());
    }
    compressor.process(buffer);

    if (buffer.getNumChannels() > 0)
        preSatBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    if (satDriveParam) saturation.setDrive(satDriveParam->load());
    if (satMixParam) saturation.setMix(satMixParam->load());
    saturation.process(buffer);

    if (buffer.getNumChannels() > 0)
        postSatBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    if (stereoWidthParam) stereoWidth.setWidth(stereoWidthParam->load());
    stereoWidth.process(buffer);

    if (buffer.getNumChannels() > 0)
        dryRevBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    if (revMixParam) reverbSend.setMix(revMixParam->load());
    if (revPredelayParam) reverbSend.setPredelay(revPredelayParam->load());
    if (revDecayParam) reverbSend.setDecay(revDecayParam->load());
    if (revDampHiFreqParam) reverbSend.setDampHighFreq(revDampHiFreqParam->load());
    if (revDampHiShelfParam) reverbSend.setDampHighShelf(revDampHiShelfParam->load());
    if (revDampBassFreqParam) reverbSend.setDampBassFreq(revDampBassFreqParam->load());
    if (revDampBassMultParam) reverbSend.setDampBassMult(revDampBassMultParam->load());
    if (revSizeParam) reverbSend.setSize(revSizeParam->load());
    if (revAttackParam) reverbSend.setAttack(revAttackParam->load());
    if (revEarlyDiffParam) reverbSend.setEarlyDiffusion(revEarlyDiffParam->load());
    if (revLateDiffParam) reverbSend.setLateDiffusion(revLateDiffParam->load());
    if (revModRateParam) reverbSend.setModRate(revModRateParam->load());
    if (revModDepthParam) reverbSend.setModDepth(revModDepthParam->load());
    if (revEqHighCutParam) reverbSend.setEqHighCut(revEqHighCutParam->load());
    if (revEqLowCutParam) reverbSend.setEqLowCut(revEqLowCutParam->load());
    if (revModeParam) reverbSend.setMode(static_cast<int>(revModeParam->load()));
    if (revColorParam) reverbSend.setColor(static_cast<int>(revColorParam->load()));
    reverbSend.process(buffer);

    if (buffer.getNumChannels() > 0)
        wetRevBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    float outGainLinear = juce::Decibels::decibelsToGain(outputGainParam->load());
    buffer.applyGain(outGainLinear);

    if (mix < 1.0f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* wet = buffer.getWritePointer(ch);
            const auto* dry = dryBuffer.getReadPointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                wet[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
        }
    }

    outputMeter.process(buffer);
}

void AssistedMixingProcessor::applyRule(Genre genre, Instrument instrument)
{
    MixRule rule = MixRuleDatabase::getRule(genre, instrument);

    auto setParam = [&](const juce::String& id, float value) {
        if (auto* p = apvts.getParameter(id))
            p->setValueNotifyingHost(p->convertTo0to1(value));
    };

    setParam("inputGain", rule.inputGain);
    setParam("outputGain", rule.outputGain);
    setParam("eqFreq0", rule.eqLowFreq);
    setParam("eqGain0", rule.eqLowGain);
    setParam("eqFreq1", rule.eqLowMidFreq);
    setParam("eqGain1", rule.eqLowMidGain);
    setParam("eqQ1", rule.eqLowMidQ);
    setParam("eqFreq2", rule.eqHighMidFreq);
    setParam("eqGain2", rule.eqHighMidGain);
    setParam("eqQ2", rule.eqHighMidQ);
    setParam("eqFreq3", rule.eqHighFreq);
    setParam("eqGain3", rule.eqHighGain);
    for (int i = 4; i < 8; ++i)
        setParam("eqGain" + juce::String(i), 0.0f);
    setParam("compThreshold", rule.compThreshold);
    setParam("compRatio", rule.compRatio);
    setParam("compAttack", rule.compAttack);
    setParam("compRelease", rule.compRelease);
    setParam("compMakeup", rule.compMakeup);
    setParam("satDrive", rule.satDrive);
    setParam("satMix", rule.satMix);
    setParam("stereoWidth", rule.stereoWidth);
    if (rule.reverbSend > -59.0f)
        setParam("revMix", juce::jlimit(0.0f, 100.0f, juce::Decibels::decibelsToGain(rule.reverbSend) * 100.0f));
    else
        setParam("revMix", 0.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout AssistedMixingProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "genre", "Genre", juce::StringArray{
            "Rock", "Pop", "Hip-Hop", "EDM", "Jazz", "R&B", "Metal", "Classical"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "instrument", "Instrument", juce::StringArray{
            "Vocals", "Drums", "Bass", "Electric Guitar", "Acoustic Guitar",
            "Keys/Synths", "Strings", "Brass"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mixAmount", "Mix Amount", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "inputGain", "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "outputGain", "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f, "dB"));

    float defaultFreqs[] = { 80.0f, 250.0f, 700.0f, 1500.0f, 3000.0f, 5000.0f, 8000.0f, 12000.0f };
    int defaultTypes[] = { 1, 0, 0, 0, 0, 0, 0, 2 };
    for (int i = 0; i < 8; ++i)
    {
        auto si = juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "eqFreq" + si, "EQ Freq " + si,
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.25f), defaultFreqs[i], "Hz"));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "eqGain" + si, "EQ Gain " + si,
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f, "dB"));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "eqQ" + si, "EQ Q " + si,
            juce::NormalisableRange<float>(0.05f, 30.0f, 0.01f, 0.4f), (i == 0 || i == 7) ? 0.707f : 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            "eqType" + si, "EQ Type " + si,
            juce::StringArray{ "Peak", "Low Shelf", "High Shelf", "Low Cut", "High Cut", "Band Pass", "Notch" },
            defaultTypes[i]));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            "eqOn" + si, "EQ On " + si, true));
    }

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "compThreshold", "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "compRatio", "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), 2.0f, ":1"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "compAttack", "Comp Attack",
        juce::NormalisableRange<float>(0.1f, 200.0f, 0.1f, 0.5f), 10.0f, "ms"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "compRelease", "Comp Release",
        juce::NormalisableRange<float>(5.0f, 1000.0f, 1.0f, 0.5f), 100.0f, "ms"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "compMakeup", "Comp Makeup",
        juce::NormalisableRange<float>(-6.0f, 24.0f, 0.1f), 0.0f, "dB"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "satDrive", "Sat Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "satMix", "Sat Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f, "%"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "stereoWidth", "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 0.0f, "%"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revMix", "Rev Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revPredelay", "Rev Predelay",
        juce::NormalisableRange<float>(0.0f, 1000.0f, 0.01f, 0.4f), 0.0f, "ms"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revDecay", "Rev Decay",
        juce::NormalisableRange<float>(0.1f, 30.0f, 0.01f, 0.4f), 0.1f, "s"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revDampHiFreq", "Damp HighFreq",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.3f), 20000.0f, "Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revDampHiShelf", "Damp HighShelf",
        juce::NormalisableRange<float>(-48.0f, 0.0f, 0.01f), 0.0f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revDampBassFreq", "Damp BassFreq",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.4f), 20.0f, "Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revDampBassMult", "Damp BassMult",
        juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f), 1.0f, "X"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revSize", "Rev Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revAttack", "Rev Attack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revEarlyDiff", "Rev Early Diff",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revLateDiff", "Rev Late Diff",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revModRate", "Rev Mod Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f, 0.4f), 0.01f, "Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revModDepth", "Rev Mod Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, "%"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revEqHighCut", "Rev EQ HighCut",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.3f), 20000.0f, "Hz"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "revEqLowCut", "Rev EQ LowCut",
        juce::NormalisableRange<float>(5.0f, 2000.0f, 0.1f, 0.4f), 5.0f, "Hz"));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "revMode", "Rev Mode", juce::StringArray{
            "Concert Hall", "Room", "Chamber", "Cathedral", "Plate"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "revColor", "Rev Color", juce::StringArray{
            "Clean", "1970s", "1980s", "Now"}, 0));

    return { params.begin(), params.end() };
}

void AssistedMixingProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("uiTheme", themeIndex.load(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AssistedMixingProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        auto newState = juce::ValueTree::fromXml(*xml);
        themeIndex.store((int)newState.getProperty("uiTheme", 0));
        apvts.replaceState(newState);
    }
}

juce::AudioProcessorEditor* AssistedMixingProcessor::createEditor()
{
    return new AssistedMixingEditor(*this);
}

bool AssistedMixingProcessor::hasEditor() const { return true; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AssistedMixingProcessor();
}
