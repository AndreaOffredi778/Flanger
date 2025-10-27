#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "Modulation.h"
#include "Delays.h"
#include "DryWet.h"
#include "Filters.h"

//==============================================================================
class FlangerAudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener,
    public juce::UndoManager
{
public:
    //==============================================================================
    FlangerAudioProcessor();
    ~FlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(const juce::String& paramID, float newValue) override;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    juce::UndoManager undoManager;

    // Audio modules
    Delays delay;
    DryWet drywetter;
    NaiveOscillator LFO;
    ParameterModulation timeModulation;
    StereoFilter filter;

    // Buffer per modulazione
    juce::AudioBuffer<float> modulation;

    // Caching dei parametri (RT-safe)
    std::atomic<float>* modAmountParam{ nullptr };
    std::atomic<float>* phaseDeltaParam{ nullptr };
    std::atomic<float>* delayTimeParam{ nullptr };
    std::atomic<float>* feedbackParam{ nullptr };
    std::atomic<float>* dryWetParam{ nullptr };
    std::atomic<float>* filterActiveParam{ nullptr };
    std::atomic<float>* outputGainParam{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessor)
};
