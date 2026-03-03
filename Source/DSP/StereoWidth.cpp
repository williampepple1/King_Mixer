#include "StereoWidth.h"

void StereoWidthDSP::prepare(const juce::dsp::ProcessSpec&)
{
}

void StereoWidthDSP::process(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() < 2)
        return;

    auto* left  = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float mid  = (left[i] + right[i]) * 0.5f;
        float side = (left[i] - right[i]) * 0.5f;

        side *= width;

        left[i]  = mid + side;
        right[i] = mid - side;
    }
}

void StereoWidthDSP::setWidth(float widthPercent)
{
    width = juce::jlimit(0.0f, 200.0f, widthPercent) / 100.0f;
}
