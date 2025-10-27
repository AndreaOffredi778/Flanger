#pragma once
#include <JuceHeader.h>

#ifndef MAX_DELAY_TIME
#define MAX_DELAY_TIME 10.0 // secondi
#endif

#ifndef DEFAULT_FEEDBACK
#define DEFAULT_FEEDBACK 0.3f
#endif

#ifndef DEFAULT_DELAY_TIME
#define DEFAULT_DELAY_TIME 5.0 // ms
#endif

class Delays
{
public:
    Delays(double defaultDelayTime = DEFAULT_DELAY_TIME, float defaultFeedback = DEFAULT_FEEDBACK)
    {
        delayTime.setCurrentAndTargetValue(defaultDelayTime);
        feedback.setCurrentAndTargetValue(defaultFeedback);
    }

    ~Delays() {}

    void prepareToPlay(double newSampleRate, int maxNumSamples)
    {
        sampleRate = newSampleRate;
        memorySize = static_cast<int>(MAX_DELAY_TIME * sampleRate) + maxNumSamples;

        delayMemory.setSize(2, memorySize);
        delayMemory.clear();

        delayTime.reset(sampleRate, 0.030);   // 30ms smoothing
        feedback.reset(sampleRate, 0.020);    // 20ms smoothing

        writeIndex = 0;
        oldSample[0] = oldSample[1] = 0.0f;
    }

    void releaseResources()
    {
        delayMemory.setSize(0, 0);
        memorySize = 0;
    }

    // Process con modulazione stereo
    void processBlock(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>& modulation)
    {
        const int numCh = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        jassert(modulation.getNumChannels() == numCh);
        jassert(modulation.getNumSamples() == numSamples);

        auto bufferData = buffer.getArrayOfWritePointers();
        auto delayData = delayMemory.getArrayOfWritePointers();

        for (int s = 0; s < numSamples; ++s)
        {
            for (int ch = 0; ch < numCh; ++ch)
            {
                // Delay modulato (ms -> samples)
                double dtSamples = delayTime.getNextValue() * 0.001 * sampleRate
                    + modulation.getReadPointer(ch)[s] * sampleRate * 0.001;
                dtSamples = juce::jlimit(0.0, static_cast<double>(memorySize - 2), dtSamples);

                double readIndex = writeIndex - dtSamples;
                if (readIndex < 0.0)
                    readIndex += memorySize;

                int idx0 = static_cast<int>(readIndex);
                int idx1 = (idx0 + 1) % memorySize;
                double frac = readIndex - idx0;

                // Scrittura input nel buffer delay
                delayData[ch][writeIndex] = bufferData[ch][s];

                // Interpolazione lineare
                float delayedSample = static_cast<float>(
                    delayData[ch][idx0] * (1.0 - frac) +
                    delayData[ch][idx1] * frac
                    );

                // Feedback
                delayData[ch][writeIndex] += delayedSample * feedback.getNextValue();

                // Aggiorna output
                bufferData[ch][s] = delayedSample;

                // Salva per eventuale uso
                oldSample[ch] = delayedSample;
            }

            writeIndex = (writeIndex + 1) % memorySize;
        }
    }

    void setDelayTime(double newValue) { delayTime.setTargetValue(newValue); }
    void setFeedback(float newValue) { feedback.setTargetValue(newValue); }

private:
    double sampleRate = 44100.0;
    int memorySize = 0;
    int writeIndex = 0;

    float oldSample[2] = { 0.0f, 0.0f };
    juce::AudioBuffer<float> delayMemory;

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> delayTime;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> feedback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delays)
};
