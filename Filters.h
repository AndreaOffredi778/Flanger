#pragma once
#include <JuceHeader.h>
#include "PluginParameters.h"

class StereoFilter
{
public:
    enum FilterType
    {
        LowPass = 0,
        HighPass,
        BandPass
    };

    StereoFilter(float initialFrequency = Parameters::defaultFilterCutoff,
        float initialQuality = Parameters::defaultQuality,
        int initialType = Parameters::defaultFilterType)
        : frequency(initialFrequency),
        quality(initialQuality),
        filterType(initialType)
    {
    }

    ~StereoFilter() = default;

    void prepareToPlay(double sr, int numChannels)
    {
        sampleRate = sr;
        iirFilters.clear();
        iirFilters.resize(numChannels);
        updateCoefficients(true);
        reset();
    }

    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto channelBlock = block.getSingleChannelBlock(ch);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            iirFilters[ch].process(context);
        }
    }

    void setFrequency(float newFrequency)
    {
        if (!juce::approximatelyEqual(frequency, newFrequency))
        {
            frequency = newFrequency;
            updateCoefficients();
        }
    }

    void setQuality(float newQuality)
    {
        if (!juce::approximatelyEqual(quality, newQuality))
        {
            quality = newQuality;
            updateCoefficients();
        }
    }

    void setFilterType(int newType)
    {
        if (filterType != newType)
        {
            filterType = newType;
            updateCoefficients();
        }
    }

    void reset()
    {
        for (auto& f : iirFilters)
            f.reset();
    }

private:
    void updateCoefficients(bool forceUpdate = false)
    {
        using Coeff = juce::dsp::IIR::Coefficients<float>;
        typename Coeff::Ptr coeffs = nullptr;

        switch (filterType)
        {
        case LowPass:  coeffs = Coeff::makeLowPass(sampleRate, frequency, quality); break;
        case HighPass: coeffs = Coeff::makeHighPass(sampleRate, frequency, quality); break;
        case BandPass: coeffs = Coeff::makeBandPass(sampleRate, frequency, quality); break;
        default:
            jassertfalse;
            coeffs = Coeff::makeLowPass(sampleRate, frequency, quality);
            break;
        }

        if (coeffs != nullptr || forceUpdate)
        {
            for (auto& f : iirFilters)
                f.coefficients = coeffs;
        }
    }

    float frequency = 0.0f;
    float quality = 0.0f;
    int filterType = 0;
    double sampleRate = 44100.0;

    std::vector<juce::dsp::IIR::Filter<float>> iirFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoFilter)
};
