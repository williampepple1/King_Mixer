#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>

class Saturation
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void setDrive(float drivePercent);
    void setMix(float mixPercent);

private:
    std::atomic<float> drive{ 0.0f };
    std::atomic<float> mix{ 0.5f };
};
