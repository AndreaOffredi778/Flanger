#pragma once
#include <JuceHeader.h>

class DryWet
{
public:
    DryWet(float defaultDryWetRatio = 0.5f)
    {
        dryWetRatio.setCurrentAndTargetValue(defaultDryWetRatio);
    }

    ~DryWet() = default;

    // Inizializza buffer e smoothing
    void prepareToPlay(double sampleRate, int maxNumChannels, int maxNumSamples)
    {
        drySignal.setSize(maxNumChannels, maxNumSamples);
        drySignal.clear();
        dryWetRatio.reset(sampleRate, 0.02); // 20 ms di smoothing
    }

    void releaseResources()
    {
        drySignal.setSize(0, 0);
    }

    // Copia il segnale dry in un buffer interno
    void copyDrySignal(const juce::AudioBuffer<float>& sourceBuffer)
    {
        jassert(drySignal.getNumChannels() == sourceBuffer.getNumChannels());
        jassert(drySignal.getNumSamples() >= sourceBuffer.getNumSamples());

        for (int ch = 0; ch < sourceBuffer.getNumChannels(); ++ch)
            drySignal.copyFrom(ch, 0, sourceBuffer, ch, 0, sourceBuffer.getNumSamples());
    }

    // Miscelazione Dry/Wet
    void mixDrySignal(juce::AudioBuffer<float>& destinationBuffer)
    {
        const int numCh = destinationBuffer.getNumChannels();
        const int numSamples = destinationBuffer.getNumSamples();

        for (int smp = 0; smp < numSamples; ++smp)
        {
            const float wetGain = dryWetRatio.getNextValue();
            const float dryGain = 1.0f - wetGain;

            for (int ch = 0; ch < numCh; ++ch)
            {
                auto* dest = destinationBuffer.getWritePointer(ch);
                const auto* dry = drySignal.getReadPointer(ch);

                dest[smp] = dest[smp] * wetGain + dry[smp] * dryGain;
            }
        }
    }

    void setDryWetRatio(float newValue)
    {
        dryWetRatio.setTargetValue(juce::jlimit(0.0f, 1.0f, newValue));
    }

private:
    juce::AudioBuffer<float> drySignal;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dryWetRatio;
};
