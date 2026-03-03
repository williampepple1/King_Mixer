#include "Saturation.h"
#include <cmath>

void Saturation::prepare(const juce::dsp::ProcessSpec&)
{
}

void Saturation::process(juce::AudioBuffer<float>& buffer)
{
    const float driveVal = drive.load();
    const float mixVal = mix.load();

    if (driveVal <= 0.001f)
        return;

    const float driveGain = 1.0f + driveVal * 9.0f;
    float denom = std::tanh(driveGain);
    if (std::abs(denom) < 1e-6f) return;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = data[i];
            float wet = std::tanh(dry * driveGain) / denom;
            data[i] = dry * (1.0f - mixVal) + wet * mixVal;
        }
    }
}

void Saturation::setDrive(float drivePercent)
{
    drive.store(juce::jlimit(0.0f, 100.0f, drivePercent) / 100.0f);
}

void Saturation::setMix(float mixPercent)
{
    mix.store(juce::jlimit(0.0f, 100.0f, mixPercent) / 100.0f);
}
