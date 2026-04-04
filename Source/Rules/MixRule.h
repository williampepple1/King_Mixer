#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

struct MixRule
{
    float inputGain  = 0.0f;   // dB
    float outputGain = 0.0f;   // dB

    // EQ: 4-band (Low Shelf, Low-Mid Peak, High-Mid Peak, High Shelf)
    float eqLowFreq     = 80.0f;   // Hz
    float eqLowGain     = 0.0f;    // dB
    float eqLowMidFreq  = 400.0f;  // Hz
    float eqLowMidGain  = 0.0f;    // dB
    float eqLowMidQ     = 1.0f;
    float eqHighMidFreq = 2500.0f; // Hz
    float eqHighMidGain = 0.0f;    // dB
    float eqHighMidQ    = 1.0f;
    float eqHighFreq    = 8000.0f; // Hz
    float eqHighGain    = 0.0f;    // dB

    // Compressor
    float compThreshold = -20.0f;  // dB
    float compRatio     = 2.0f;    // :1
    float compAttack    = 10.0f;   // ms
    float compRelease   = 100.0f;  // ms
    float compMakeup    = 0.0f;    // dB

    // Saturation
    float satDrive = 0.0f;   // 0-100%
    float satMix   = 50.0f;  // 0-100%

    // Stereo
    float stereoWidth = 100.0f;  // 0-200%

    // Reverb send
    float reverbSend = -60.0f;   // dB (-60 = off)
};
