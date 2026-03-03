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

    instanceSlotId = InstanceHub::getInstance().registerInstance(this, trackName, false);
}

AssistedMixingProcessor::~AssistedMixingProcessor()
{
    InstanceHub::getInstance().unregisterInstance(instanceSlotId);
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

    if (bypassParam->load() > 0.5f)
        return;

    // Check for master-pushed parameter updates
    if (!masterBusMode.load() && instanceSlotId >= 0)
    {
        auto& slot = InstanceHub::getInstance().getSlot(instanceSlotId);
        if (slot.hasMasterPush.load())
        {
            uint32_t ver = slot.masterPushVersion.load();
            if (ver != lastMasterPushVersion)
            {
                lastMasterPushVersion = ver;
                InstanceParamSnapshot pushed = slot.masterPushedParams;
                slot.hasMasterPush.store(false);

                juce::WeakReference<AssistedMixingProcessor> weakThis(this);
                juce::MessageManager::callAsync([weakThis, pushed]() {
                    if (auto* self = weakThis.get())
                        self->applyParamSnapshot(pushed);
                });
            }
        }

        // Solo/mute from master — mute this track's output
        auto& hub = InstanceHub::getInstance();
        bool anySoloed = hub.isAnySoloed();
        bool thisSoloed = slot.soloFromMaster.load();
        bool thisMuted = slot.muteFromMaster.load();

        if (thisMuted || (anySoloed && !thisSoloed))
        {
            buffer.clear();
            outputMeter.process(buffer);
            return;
        }
    }

    const float mix = mixAmountParam->load();

    juce::AudioBuffer<float> dryBuffer;
    if (mix < 1.0f)
        dryBuffer.makeCopyOf(buffer);

    inputMeter.process(buffer);

    gainStage.setGainDB(inputGainParam->load());
    gainStage.process(buffer);

    // Pre-EQ spectrum
    if (buffer.getNumChannels() > 0)
        preEQAnalyzer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    for (int i = 0; i < 8; ++i)
    {
        parametricEQ.updateBand(i,
            eqBandParams[i].freq->load(),
            eqBandParams[i].gain->load(),
            eqBandParams[i].q->load(),
            static_cast<int>(eqBandParams[i].type->load()),
            eqBandParams[i].enabled->load() > 0.5f,
            getSampleRate());
    }
    parametricEQ.process(buffer);

    // Post-EQ spectrum
    if (buffer.getNumChannels() > 0)
        postEQAnalyzer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    compressor.updateParameters(
        compThresholdParam->load(), compRatioParam->load(),
        compAttackParam->load(), compReleaseParam->load(),
        compMakeupParam->load());
    compressor.process(buffer);

    // Pre-saturation waveform
    if (buffer.getNumChannels() > 0)
        preSatBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    saturation.setDrive(satDriveParam->load());
    saturation.setMix(satMixParam->load());
    saturation.process(buffer);

    // Post-saturation waveform
    if (buffer.getNumChannels() > 0)
        postSatBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    stereoWidth.setWidth(stereoWidthParam->load());
    stereoWidth.process(buffer);

    // Pre-reverb waveform
    if (buffer.getNumChannels() > 0)
        dryRevBuffer.pushSamples(buffer.getReadPointer(0), buffer.getNumSamples());

    reverbSend.setMix(revMixParam->load());
    reverbSend.setPredelay(revPredelayParam->load());
    reverbSend.setDecay(revDecayParam->load());
    reverbSend.setDampHighFreq(revDampHiFreqParam->load());
    reverbSend.setDampHighShelf(revDampHiShelfParam->load());
    reverbSend.setDampBassFreq(revDampBassFreqParam->load());
    reverbSend.setDampBassMult(revDampBassMultParam->load());
    reverbSend.setSize(revSizeParam->load());
    reverbSend.setAttack(revAttackParam->load());
    reverbSend.setEarlyDiffusion(revEarlyDiffParam->load());
    reverbSend.setLateDiffusion(revLateDiffParam->load());
    reverbSend.setModRate(revModRateParam->load());
    reverbSend.setModDepth(revModDepthParam->load());
    reverbSend.setEqHighCut(revEqHighCutParam->load());
    reverbSend.setEqLowCut(revEqLowCutParam->load());
    reverbSend.setMode(static_cast<int>(revModeParam->load()));
    reverbSend.setColor(static_cast<int>(revColorParam->load()));
    reverbSend.process(buffer);

    // Post-reverb waveform
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

    // Push snapshots to the hub for master bus visibility
    if (instanceSlotId >= 0)
    {
        InstanceLevelSnapshot lvl;
        lvl.peakL = outputMeter.getPeakL();
        lvl.peakR = outputMeter.getPeakR();
        lvl.rmsL = outputMeter.getRmsL();
        lvl.rmsR = outputMeter.getRmsR();
        lvl.gainReductionDB = compressor.getGainReduction();
        InstanceHub::getInstance().pushLevelSnapshot(instanceSlotId, lvl);

        InstanceHub::getInstance().pushParamSnapshot(instanceSlotId, buildParamSnapshot());
    }
}

void AssistedMixingProcessor::applyRule(Genre genre, Instrument instrument)
{
    MixRule rule = MixRuleDatabase::getRule(genre, instrument);

    auto setParam = [&](const juce::String& id, float value) {
        auto* p = apvts.getParameter(id);
        p->setValueNotifyingHost(p->convertTo0to1(value));
    };

    setParam("inputGain", rule.inputGain);
    setParam("outputGain", rule.outputGain);
    // Map 4 legacy rule bands to first 4 of the 8 bands
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
    // Reset bands 4-7 to flat
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

    // 8-band EQ
    float defaultFreqs[] = { 80.0f, 250.0f, 700.0f, 1500.0f, 3000.0f, 5000.0f, 8000.0f, 12000.0f };
    int defaultTypes[] = { 1, 0, 0, 0, 0, 0, 0, 2 }; // LowShelf, Peak x6, HighShelf
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

    // Reverb — all defaults zeroed so user must set them
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

void AssistedMixingProcessor::setMasterBusMode(bool isMaster)
{
    masterBusMode.store(isMaster);
    if (instanceSlotId >= 0)
        InstanceHub::getInstance().setIsMaster(instanceSlotId, isMaster);
}

void AssistedMixingProcessor::setTrackName(const juce::String& name)
{
    trackName = name;
    if (instanceSlotId >= 0)
        InstanceHub::getInstance().updateTrackName(instanceSlotId, name);
}

InstanceParamSnapshot AssistedMixingProcessor::buildParamSnapshot() const
{
    InstanceParamSnapshot snap;
    snap.inputGain = inputGainParam->load();
    snap.outputGain = outputGainParam->load();

    for (int i = 0; i < 8; ++i)
    {
        snap.eqBands[i].freq = eqBandParams[i].freq->load();
        snap.eqBands[i].gain = eqBandParams[i].gain->load();
        snap.eqBands[i].q = eqBandParams[i].q->load();
        snap.eqBands[i].type = static_cast<int>(eqBandParams[i].type->load());
        snap.eqBands[i].enabled = eqBandParams[i].enabled->load() > 0.5f;
    }

    snap.compThreshold = compThresholdParam->load();
    snap.compRatio = compRatioParam->load();
    snap.compAttack = compAttackParam->load();
    snap.compRelease = compReleaseParam->load();
    snap.compMakeup = compMakeupParam->load();
    snap.satDrive = satDriveParam->load();
    snap.satMix = satMixParam->load();
    snap.stereoWidth = stereoWidthParam->load();
    snap.revMix = revMixParam->load();
    snap.revPredelay = revPredelayParam->load();
    snap.revDecay = revDecayParam->load();
    snap.revSize = revSizeParam->load();
    snap.revDampHiFreq = revDampHiFreqParam->load();
    snap.revDampHiShelf = revDampHiShelfParam->load();
    snap.revDampBassFreq = revDampBassFreqParam->load();
    snap.revDampBassMult = revDampBassMultParam->load();
    snap.revAttack = revAttackParam->load();
    snap.revEarlyDiff = revEarlyDiffParam->load();
    snap.revLateDiff = revLateDiffParam->load();
    snap.revModRate = revModRateParam->load();
    snap.revModDepth = revModDepthParam->load();
    snap.revEqHighCut = revEqHighCutParam->load();
    snap.revEqLowCut = revEqLowCutParam->load();
    snap.revMode = static_cast<int>(revModeParam->load());
    snap.revColor = static_cast<int>(revColorParam->load());
    snap.mixAmount = mixAmountParam->load();
    snap.bypass = bypassParam->load() > 0.5f;
    snap.genreIndex = static_cast<int>(apvts.getRawParameterValue("genre")->load());
    snap.instrumentIndex = static_cast<int>(apvts.getRawParameterValue("instrument")->load());
    return snap;
}

void AssistedMixingProcessor::applyParamSnapshot(const InstanceParamSnapshot& snap)
{
    auto setParam = [&](const juce::String& id, float value) {
        auto* p = apvts.getParameter(id);
        if (p) p->setValueNotifyingHost(p->convertTo0to1(value));
    };

    setParam("inputGain", snap.inputGain);
    setParam("outputGain", snap.outputGain);

    for (int i = 0; i < 8; ++i)
    {
        auto si = juce::String(i);
        setParam("eqFreq" + si, snap.eqBands[i].freq);
        setParam("eqGain" + si, snap.eqBands[i].gain);
        setParam("eqQ" + si, snap.eqBands[i].q);
    }

    setParam("compThreshold", snap.compThreshold);
    setParam("compRatio", snap.compRatio);
    setParam("compAttack", snap.compAttack);
    setParam("compRelease", snap.compRelease);
    setParam("compMakeup", snap.compMakeup);
    setParam("satDrive", snap.satDrive);
    setParam("satMix", snap.satMix);
    setParam("stereoWidth", snap.stereoWidth);
    setParam("revMix", snap.revMix);
    setParam("revPredelay", snap.revPredelay);
    setParam("revDecay", snap.revDecay);
    setParam("revSize", snap.revSize);
    setParam("revDampHiFreq", snap.revDampHiFreq);
    setParam("revDampHiShelf", snap.revDampHiShelf);
    setParam("revDampBassFreq", snap.revDampBassFreq);
    setParam("revDampBassMult", snap.revDampBassMult);
    setParam("revAttack", snap.revAttack);
    setParam("revEarlyDiff", snap.revEarlyDiff);
    setParam("revLateDiff", snap.revLateDiff);
    setParam("revModRate", snap.revModRate);
    setParam("revModDepth", snap.revModDepth);
    setParam("revEqHighCut", snap.revEqHighCut);
    setParam("revEqLowCut", snap.revEqLowCut);
    setParam("mixAmount", snap.mixAmount);
}

void AssistedMixingProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("uiTheme", themeIndex.load(), nullptr);
    state.setProperty("isMasterBus", masterBusMode.load() ? 1 : 0, nullptr);
    state.setProperty("trackName", trackName, nullptr);
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

        bool wasMaster = (int)newState.getProperty("isMasterBus", 0) != 0;
        setMasterBusMode(wasMaster);

        juce::String savedName = newState.getProperty("trackName", "Track").toString();
        setTrackName(savedName);

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
