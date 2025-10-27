#pragma once
#include <JuceHeader.h>

//==============================================================
//                       NaiveOscillator
//==============================================================
class NaiveOscillator
{
public:
    enum Waveform { Sine = 0, Triangle, SawUp, SawDown, Square };

    NaiveOscillator(double defaultFrequency = 440.0, Waveform defaultWaveform = Sine)
        : waveform(defaultWaveform)
    {
        frequency.setCurrentAndTargetValue(defaultFrequency);
    }

    void prepareToPlay(double sampleRate)
    {
        jassert(sampleRate > 0.0);
        samplingPeriod = 1.0 / sampleRate;
        frequency.reset(sampleRate, 0.02); // 20 ms smoothing
    }

    void reset(double startPhase = 0.0)
    {
        currentPhase = wrap01(startPhase);
    }

    void setFrequency(double newValue)
    {
        jassert(newValue > 0.0);
        frequency.setTargetValue(newValue);
    }

    void setWaveform(Waveform newWaveform)
    {
        waveform = newWaveform;
    }

    double getCurrentPhase() const noexcept { return currentPhase; }

    inline void advancePhase() noexcept
    {
        const double phaseIncrement = frequency.getNextValue() * samplingPeriod;
        currentPhase = wrap01(currentPhase + phaseIncrement);
    }

    inline double generateSample(double phase) const noexcept
    {
        switch (waveform)
        {
        case Sine:
            return std::sin(phase * juce::MathConstants<double>::twoPi);
        case Triangle:
            return 2.0 * std::abs(2.0 * phase - 1.0) - 1.0; // [-1,1]
        case SawUp:
            return (2.0 * phase) - 1.0; // [-1,1]
        case SawDown:
            return 1.0 - (2.0 * phase); // [-1,1]
        case Square:
            return (phase < 0.5) ? 1.0 : -1.0;
        default:
            jassertfalse;
            return 0.0;
        }
    }

private:
    static inline double wrap01(double x) noexcept
    {
        x -= std::floor(x);
        return x;
    }

    Waveform waveform;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Multiplicative> frequency;
    double currentPhase{ 0.0 };
    double samplingPeriod{ 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaiveOscillator)
};

//==============================================================
//                       ParameterModulation
//==============================================================

class ParameterModulation
{
public:
    ParameterModulation(double defaultParameter = 0.0,
        double defaultModAmount = 0.0,
        double defaultPhaseDelta = 0.0)
    {
        parameter.setCurrentAndTargetValue(defaultParameter);
        modAmount.setCurrentAndTargetValue(defaultModAmount);
        phaseDelta.setCurrentAndTargetValue(defaultPhaseDelta);
    }

    void prepareToPlay(double sampleRate)
    {
        parameter.reset(sampleRate, 0.02);
        modAmount.reset(sampleRate, 0.02);
        phaseDelta.reset(sampleRate, 0.02);
        samplingPeriod = 1.0 / sampleRate;
    }

    void setParameter(double newValue) { parameter.setTargetValue(newValue); }
    void setModAmount(double newValue) { modAmount.setTargetValue(newValue); }
    void setPhaseDelta(double newValue) { phaseDelta.setTargetValue(newValue); }

    double getModAmount() const noexcept { return modAmount.getTargetValue(); }

    // Riempie un buffer stereo di valori modulati
    void process(juce::AudioBuffer<float>& modulationBuffer, NaiveOscillator& lfo)
    {
        const int numCh = modulationBuffer.getNumChannels();
        const int numSamples = modulationBuffer.getNumSamples();

        jassert(numCh >= 1);

        for (int s = 0; s < numSamples; ++s)
        {
            const double phiMain = lfo.getCurrentPhase();
            const double phiOffset = wrap01(phiMain + phaseDelta.getNextValue());

            // LFO puro [-1..1]
            const double lfoL = lfo.generateSample(phiMain);
            const double lfoR = lfo.generateSample(phiOffset);

            // Parametri smoothed
            const double amt = modAmount.getNextValue();
            const double base = parameter.getNextValue();

            // Valore modulato: base + LFO * amount
            const double modulatedL = base + amt * lfoL;
            const double modulatedR = base + amt * lfoR;

            if (numCh >= 1)
                modulationBuffer.getWritePointer(0)[s] = static_cast<float>(modulatedL);

            if (numCh >= 2)
                modulationBuffer.getWritePointer(1)[s] = static_cast<float>(modulatedR);

            lfo.advancePhase();
        }
    }

private:
    static inline double wrap01(double x) noexcept
    {
        x -= std::floor(x);
        return x;
    }

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> parameter;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> modAmount;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> phaseDelta;
    double samplingPeriod{ 0.0 };
};
