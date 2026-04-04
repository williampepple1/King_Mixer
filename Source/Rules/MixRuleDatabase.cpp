#include "MixRuleDatabase.h"

MixRule MixRuleDatabase::getRule(Genre genre, Instrument instrument)
{
    switch (genre)
    {
        case Genre::Rock:      return getRockRule(instrument);
        case Genre::Pop:       return getPopRule(instrument);
        case Genre::HipHop:    return getHipHopRule(instrument);
        case Genre::EDM:       return getEDMRule(instrument);
        case Genre::Jazz:      return getJazzRule(instrument);
        case Genre::RnB:       return getRnBRule(instrument);
        case Genre::Metal:     return getMetalRule(instrument);
        case Genre::Classical: return getClassicalRule(instrument);
        default:               return {};
    }
}

// ============================================================
// ROCK
// ============================================================
MixRule MixRuleDatabase::getRockRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -2.5f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -18.0f; r.compRatio = 3.0f; r.compAttack = 5.0f; r.compRelease = 80.0f; r.compMakeup = 3.0f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 80.0f;
            r.reverbSend = -18.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 3.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -3.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -15.0f; r.compRatio = 4.0f; r.compAttack = 2.0f; r.compRelease = 60.0f; r.compMakeup = 4.0f;
            r.satDrive = 15.0f; r.satMix = 50.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -24.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = 3.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 700.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = -1.0f;
            r.compThreshold = -16.0f; r.compRatio = 4.0f; r.compAttack = 8.0f; r.compRelease = 100.0f; r.compMakeup = 3.0f;
            r.satDrive = 20.0f; r.satMix = 50.0f;
            r.stereoWidth = 60.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -18.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 80.0f; r.compMakeup = 2.0f;
            r.satDrive = 25.0f; r.satMix = 60.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -20.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -20.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 120.0f; r.compMakeup = 2.0f;
            r.satDrive = 5.0f; r.satMix = 30.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -18.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -20.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.5f;
            r.satDrive = 8.0f; r.satMix = 35.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -16.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -22.0f; r.compRatio = 2.0f; r.compAttack = 25.0f; r.compRelease = 200.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 150.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -18.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -16.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// POP
// ============================================================
MixRule MixRuleDatabase::getPopRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 3.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.5f; r.compAttack = 3.0f; r.compRelease = 60.0f; r.compMakeup = 4.0f;
            r.satDrive = 8.0f; r.satMix = 35.0f;
            r.stereoWidth = 90.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 2.0f;
            r.eqLowMidFreq = 350.0f; r.eqLowMidGain = -2.5f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -14.0f; r.compRatio = 3.5f; r.compAttack = 3.0f; r.compRelease = 50.0f; r.compMakeup = 3.5f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -22.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 3.0f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 800.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 5000.0f; r.eqHighGain = -2.0f;
            r.compThreshold = -15.0f; r.compRatio = 4.5f; r.compAttack = 5.0f; r.compRelease = 80.0f; r.compMakeup = 3.0f;
            r.satDrive = 12.0f; r.satMix = 45.0f;
            r.stereoWidth = 50.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3500.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -18.0f; r.compRatio = 2.5f; r.compAttack = 12.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 15.0f; r.satMix = 45.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -18.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -2.5f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 14000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -18.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 3.0f; r.satMix = 25.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -16.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 350.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -18.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 30.0f;
            r.stereoWidth = 150.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -22.0f; r.compRatio = 1.8f; r.compAttack = 30.0f; r.compRelease = 200.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 160.0f;
            r.reverbSend = -10.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -18.0f; r.compRatio = 2.5f; r.compAttack = 10.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 8.0f; r.satMix = 35.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -14.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// HIP-HOP / TRAP
// ============================================================
MixRule MixRuleDatabase::getHipHopRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.5f;
            r.compThreshold = -14.0f; r.compRatio = 4.0f; r.compAttack = 2.0f; r.compRelease = 50.0f; r.compMakeup = 5.0f;
            r.satDrive = 12.0f; r.satMix = 45.0f;
            r.stereoWidth = 85.0f;
            r.reverbSend = -20.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 1.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 50.0f; r.eqLowGain = 5.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -4.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -12.0f; r.compRatio = 5.0f; r.compAttack = 1.0f; r.compRelease = 40.0f; r.compMakeup = 5.0f;
            r.satDrive = 20.0f; r.satMix = 55.0f;
            r.stereoWidth = 100.0f;
            r.reverbSend = -30.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 40.0f; r.eqLowGain = 5.0f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = -3.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 800.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 3000.0f; r.eqHighGain = -3.0f;
            r.compThreshold = -12.0f; r.compRatio = 6.0f; r.compAttack = 3.0f; r.compRelease = 60.0f; r.compMakeup = 5.0f;
            r.satDrive = 25.0f; r.satMix = 55.0f;
            r.stereoWidth = 40.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 80.0f; r.compMakeup = 2.0f;
            r.satDrive = 15.0f; r.satMix = 45.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -22.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -18.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 5.0f; r.satMix = 30.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -20.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = 2.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -15.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 80.0f; r.compMakeup = 2.0f;
            r.satDrive = 15.0f; r.satMix = 45.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -18.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -22.0f; r.compRatio = 2.0f; r.compAttack = 25.0f; r.compRelease = 200.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 150.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 8.0f; r.compRelease = 80.0f; r.compMakeup = 2.0f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 100.0f;
            r.reverbSend = -18.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// EDM / ELECTRONIC
// ============================================================
MixRule MixRuleDatabase::getEDMRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -5.0f;
            r.eqLowMidFreq = 350.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 14000.0f; r.eqHighGain = 4.0f;
            r.compThreshold = -14.0f; r.compRatio = 4.0f; r.compAttack = 2.0f; r.compRelease = 40.0f; r.compMakeup = 5.0f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 100.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 1.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 50.0f; r.eqLowGain = 4.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -4.0f; r.eqLowMidQ = 2.0f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 3.5f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -10.0f; r.compRatio = 6.0f; r.compAttack = 0.5f; r.compRelease = 30.0f; r.compMakeup = 6.0f;
            r.satDrive = 25.0f; r.satMix = 60.0f;
            r.stereoWidth = 90.0f;
            r.reverbSend = -28.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 40.0f; r.eqLowGain = 6.0f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = -4.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 1000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = -2.0f;
            r.compThreshold = -10.0f; r.compRatio = 8.0f; r.compAttack = 2.0f; r.compRelease = 40.0f; r.compMakeup = 6.0f;
            r.satDrive = 30.0f; r.satMix = 60.0f;
            r.stereoWidth = 30.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 8.0f; r.compRelease = 80.0f; r.compMakeup = 3.0f;
            r.satDrive = 20.0f; r.satMix = 50.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -16.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 14000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -16.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 5.0f; r.satMix = 30.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 3.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 6000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 14000.0f; r.eqHighGain = 4.0f;
            r.compThreshold = -14.0f; r.compRatio = 3.0f; r.compAttack = 5.0f; r.compRelease = 60.0f; r.compMakeup = 3.0f;
            r.satDrive = 20.0f; r.satMix = 50.0f;
            r.stereoWidth = 170.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -20.0f; r.compRatio = 2.0f; r.compAttack = 25.0f; r.compRelease = 180.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 25.0f;
            r.stereoWidth = 170.0f;
            r.reverbSend = -10.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 8.0f; r.compRelease = 80.0f; r.compMakeup = 2.5f;
            r.satDrive = 12.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -16.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// JAZZ
// ============================================================
MixRule MixRuleDatabase::getJazzRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -24.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.5f;
            r.satDrive = 3.0f; r.satMix = 20.0f;
            r.stereoWidth = 90.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 1.0f;
            r.eqLowMidFreq = 350.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -24.0f; r.compRatio = 2.0f; r.compAttack = 15.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 3.0f; r.satMix = 20.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 2.0f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = 1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 800.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = 0.5f;
            r.compThreshold = -22.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 25.0f;
            r.stereoWidth = 70.0f;
            r.reverbSend = -30.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -22.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.0f;
            r.satDrive = 5.0f; r.satMix = 25.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -1.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -22.0f; r.compRatio = 2.0f; r.compAttack = 25.0f; r.compRelease = 180.0f; r.compMakeup = 1.0f;
            r.satDrive = 3.0f; r.satMix = 20.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -1.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -24.0f; r.compRatio = 1.8f; r.compAttack = 25.0f; r.compRelease = 180.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -10.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -26.0f; r.compRatio = 1.5f; r.compAttack = 30.0f; r.compRelease = 250.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 160.0f;
            r.reverbSend = -8.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -22.0f; r.compRatio = 2.0f; r.compAttack = 15.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 25.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -10.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// R&B / SOUL
// ============================================================
MixRule MixRuleDatabase::getRnBRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 5.0f; r.compRelease = 70.0f; r.compMakeup = 3.5f;
            r.satDrive = 8.0f; r.satMix = 35.0f;
            r.stereoWidth = 95.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 50.0f; r.eqLowGain = 3.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -3.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -14.0f; r.compRatio = 3.5f; r.compAttack = 3.0f; r.compRelease = 60.0f; r.compMakeup = 3.0f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -22.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 0.5f;
            r.eqLowFreq = 50.0f; r.eqLowGain = 4.0f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 800.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = -1.0f;
            r.compThreshold = -14.0f; r.compRatio = 4.0f; r.compAttack = 5.0f; r.compRelease = 80.0f; r.compMakeup = 3.0f;
            r.satDrive = 15.0f; r.satMix = 45.0f;
            r.stereoWidth = 50.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -2.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -20.0f; r.compRatio = 2.5f; r.compAttack = 15.0f; r.compRelease = 100.0f; r.compMakeup = 1.5f;
            r.satDrive = 8.0f; r.satMix = 35.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -16.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -20.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 3.0f; r.satMix = 20.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = 1.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -18.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 120.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 30.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -24.0f; r.compRatio = 1.8f; r.compAttack = 30.0f; r.compRelease = 200.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 160.0f;
            r.reverbSend = -10.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -20.0f; r.compRatio = 2.5f; r.compAttack = 10.0f; r.compRelease = 100.0f; r.compMakeup = 2.0f;
            r.satDrive = 8.0f; r.satMix = 30.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -14.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// METAL
// ============================================================
MixRule MixRuleDatabase::getMetalRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 3500.0f; r.eqHighMidGain = 4.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.5f;
            r.compThreshold = -14.0f; r.compRatio = 4.0f; r.compAttack = 3.0f; r.compRelease = 50.0f; r.compMakeup = 5.0f;
            r.satDrive = 20.0f; r.satMix = 55.0f;
            r.stereoWidth = 80.0f;
            r.reverbSend = -22.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 1.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 4.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -4.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 4.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 3.0f;
            r.compThreshold = -12.0f; r.compRatio = 5.0f; r.compAttack = 1.0f; r.compRelease = 40.0f; r.compMakeup = 5.0f;
            r.satDrive = 25.0f; r.satMix = 60.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -28.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 1.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = 4.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = -3.0f; r.eqLowMidQ = 1.5f;
            r.eqHighMidFreq = 1000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.2f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -12.0f; r.compRatio = 6.0f; r.compAttack = 3.0f; r.compRelease = 60.0f; r.compMakeup = 5.0f;
            r.satDrive = 30.0f; r.satMix = 60.0f;
            r.stereoWidth = 50.0f;
            r.reverbSend = -60.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -4.0f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.2f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.5f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.5f; r.compAttack = 5.0f; r.compRelease = 60.0f; r.compMakeup = 3.0f;
            r.satDrive = 35.0f; r.satMix = 65.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -24.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -2.0f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 5000.0f; r.eqHighMidGain = 3.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -18.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 80.0f; r.compMakeup = 2.5f;
            r.satDrive = 10.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -20.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -3.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 2.0f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 10.0f; r.compRelease = 80.0f; r.compMakeup = 2.0f;
            r.satDrive = 15.0f; r.satMix = 45.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -18.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 120.0f; r.eqLowGain = -2.0f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.5f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 2.0f;
            r.compThreshold = -20.0f; r.compRatio = 2.0f; r.compAttack = 25.0f; r.compRelease = 180.0f; r.compMakeup = 1.5f;
            r.satDrive = 5.0f; r.satMix = 25.0f;
            r.stereoWidth = 150.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 150.0f; r.eqLowGain = -3.5f;
            r.eqLowMidFreq = 500.0f; r.eqLowMidGain = -1.5f; r.eqLowMidQ = 1.0f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 2.5f; r.eqHighMidQ = 1.0f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -16.0f; r.compRatio = 3.0f; r.compAttack = 8.0f; r.compRelease = 80.0f; r.compMakeup = 2.5f;
            r.satDrive = 12.0f; r.satMix = 40.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -18.0f;
            break;

        default: break;
    }
    return r;
}

// ============================================================
// CLASSICAL / ORCHESTRAL
// ============================================================
MixRule MixRuleDatabase::getClassicalRule(Instrument inst)
{
    MixRule r;
    switch (inst)
    {
        case Instrument::Vocals:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -1.0f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -28.0f; r.compRatio = 1.5f; r.compAttack = 30.0f; r.compRelease = 250.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 100.0f;
            r.reverbSend = -8.0f;
            break;

        case Instrument::Drums:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 0.5f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = -1.0f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 0.5f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -28.0f; r.compRatio = 1.5f; r.compAttack = 25.0f; r.compRelease = 200.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 140.0f;
            r.reverbSend = -10.0f;
            break;

        case Instrument::Bass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 60.0f; r.eqLowGain = 1.5f;
            r.eqLowMidFreq = 200.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 800.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 4000.0f; r.eqHighGain = 0.5f;
            r.compThreshold = -26.0f; r.compRatio = 2.0f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 80.0f;
            r.reverbSend = -14.0f;
            break;

        case Instrument::ElectricGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.0f; r.eqLowMidQ = 0.8f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.8f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -24.0f; r.compRatio = 1.8f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.0f;
            r.satDrive = 3.0f; r.satMix = 20.0f;
            r.stereoWidth = 110.0f;
            r.reverbSend = -12.0f;
            break;

        case Instrument::AcousticGuitar:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -0.5f;
            r.eqLowMidFreq = 250.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 4000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 12000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -26.0f; r.compRatio = 1.5f; r.compAttack = 30.0f; r.compRelease = 200.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 120.0f;
            r.reverbSend = -8.0f;
            break;

        case Instrument::KeysSynths:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -0.5f;
            r.eqLowMidFreq = 300.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -26.0f; r.compRatio = 1.5f; r.compAttack = 30.0f; r.compRelease = 200.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -8.0f;
            break;

        case Instrument::Strings:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 80.0f; r.eqLowGain = -0.5f;
            r.eqLowMidFreq = 350.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 3000.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 10000.0f; r.eqHighGain = 1.5f;
            r.compThreshold = -30.0f; r.compRatio = 1.3f; r.compAttack = 40.0f; r.compRelease = 300.0f; r.compMakeup = 0.5f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 180.0f;
            r.reverbSend = -6.0f;
            break;

        case Instrument::Brass:
            r.inputGain = 0.0f;  r.outputGain = 0.0f;
            r.eqLowFreq = 100.0f; r.eqLowGain = -1.5f;
            r.eqLowMidFreq = 400.0f; r.eqLowMidGain = 0.5f; r.eqLowMidQ = 0.7f;
            r.eqHighMidFreq = 2500.0f; r.eqHighMidGain = 1.0f; r.eqHighMidQ = 0.7f;
            r.eqHighFreq = 8000.0f; r.eqHighGain = 1.0f;
            r.compThreshold = -26.0f; r.compRatio = 1.5f; r.compAttack = 20.0f; r.compRelease = 150.0f; r.compMakeup = 1.0f;
            r.satDrive = 0.0f; r.satMix = 0.0f;
            r.stereoWidth = 130.0f;
            r.reverbSend = -8.0f;
            break;

        default: break;
    }
    return r;
}
