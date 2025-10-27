#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"

//==============================================================================
// Costruttore
FlangerAudioProcessor::FlangerAudioProcessor()
    : parameters(*this, &undoManager, "FLG", Parameters::createParameterLayout()),
    drywetter(Parameters::defaultDryWet),
    delay(Parameters::defaultFeedback),
    LFO(Parameters::defaultModFrequency, static_cast<NaiveOscillator::Waveform>(Parameters::defaultWaveform)),
    timeModulation(Parameters::defaultDelay, Parameters::defaultModAmount, Parameters::defaultPhaseDelta),
    filter(Parameters::defaultFilterCutoff, Parameters::defaultQuality, Parameters::defaultFilterType) {

    Parameters::addListenerToAllParameters(parameters, this);

    // cache raw parameter pointers per uso in processBlock (RT-safe)
    modAmountParam = parameters.getRawParameterValue(Parameters::nameModAmount);
    phaseDeltaParam = parameters.getRawParameterValue(Parameters::namePhaseDelta);
    delayTimeParam = parameters.getRawParameterValue(Parameters::nameDelayTime);
    feedbackParam = parameters.getRawParameterValue(Parameters::nameFeedback);
    dryWetParam = parameters.getRawParameterValue(Parameters::nameDryWet);
    filterActiveParam = parameters.getRawParameterValue(Parameters::nameFilterActive);
    outputGainParam = parameters.getRawParameterValue(Parameters::nameOutputGain);

    // init modulation buffer piccolo, sarà ridimensionato in prepareToPlay
    modulation.setSize(getTotalNumOutputChannels(), 128);
    modulation.clear();
}

//==============================================================================
// Distruttore
FlangerAudioProcessor::~FlangerAudioProcessor()
{
    Parameters::removeListenerFromAllParameters(parameters, this);
}

//==============================================================================
// Informazioni base
const juce::String FlangerAudioProcessor::getName() const { return JucePlugin_Name; }
bool FlangerAudioProcessor::acceptsMidi() const { return JucePlugin_WantsMidiInput; }
bool FlangerAudioProcessor::producesMidi() const { return JucePlugin_ProducesMidiOutput; }
bool FlangerAudioProcessor::isMidiEffect() const { return JucePlugin_IsMidiEffect; }
double FlangerAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int FlangerAudioProcessor::getNumPrograms() { return 1; }
int FlangerAudioProcessor::getCurrentProgram() { return 0; }
void FlangerAudioProcessor::setCurrentProgram([[maybe_unused]] int index) {}
const juce::String FlangerAudioProcessor::getProgramName([[maybe_unused]] int index) { return {}; }
void FlangerAudioProcessor::changeProgramName([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) {}

//==============================================================================
// Preparazione audio
void FlangerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    delay.prepareToPlay(sampleRate, static_cast<float>(samplesPerBlock));
    drywetter.prepareToPlay(sampleRate, getTotalNumOutputChannels(), samplesPerBlock);
    LFO.prepareToPlay(sampleRate);
    timeModulation.prepareToPlay(sampleRate);
    filter.prepareToPlay(sampleRate, getTotalNumOutputChannels());

    modulation.setSize(getTotalNumOutputChannels(), samplesPerBlock, false, false, true);
    modulation.clear();
}

void FlangerAudioProcessor::releaseResources()
{
    delay.releaseResources();
    drywetter.releaseResources();
    filter.reset();
    modulation.setSize(0, 0);
}

//==============================================================================
// Processamento audio
void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Clear canali extra
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, numSamples);

    // Resize modulation buffer se necessario
    if (modulation.getNumChannels() != numChannels || modulation.getNumSamples() != numSamples)
        modulation.setSize(numChannels, numSamples, false, false, true);
    modulation.clear();

    // 1) copia DRY
    drywetter.copyDrySignal(buffer);

    // 2) calcola modulazione con LFO
    timeModulation.process(modulation, LFO);

    // 3) delay/flanger
    delay.processBlock(buffer, modulation);

    // 4) filtro opzionale
    if (filterActiveParam && (*filterActiveParam) > 0.5f)
        filter.processBlock(buffer);

    // 5) mix dry/wet
    drywetter.mixDrySignal(buffer);

    // 6) output gain
    const float outputGainDb = (outputGainParam != nullptr) ? outputGainParam->load() : 0.0f;
    buffer.applyGain(juce::Decibels::decibelsToGain(outputGainDb));

}

//==============================================================================
// Parametri
void FlangerAudioProcessor::parameterChanged(const juce::String& paramID, float newValue)
{
    using namespace Parameters;

    if (paramID == nameDelayTime)     timeModulation.setParameter(newValue);
    else if (paramID == nameFeedback)      delay.setFeedback(newValue);
    else if (paramID == nameDryWet)        drywetter.setDryWetRatio(newValue);
    else if (paramID == nameWaveform)      LFO.setWaveform(static_cast<NaiveOscillator::Waveform>(juce::roundToInt(newValue)));
    else if (paramID == nameModFrequency)  LFO.setFrequency(newValue);
    else if (paramID == nameModAmount)     timeModulation.setModAmount(newValue);
    else if (paramID == namePhaseDelta)    timeModulation.setPhaseDelta(newValue);
    else if (paramID == nameFilterType)    filter.setFilterType(juce::roundToInt(newValue));
    else if (paramID == nameFilterCutoff)  filter.setFrequency(newValue);
    else if (paramID == nameQuality)       filter.setQuality(newValue);
}

//==============================================================================
// Editor
bool FlangerAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor()
{
    return new FlangerAudioProcessorEditor(*this, parameters);
}

//==============================================================================
// Stato
void FlangerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void FlangerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// Factory
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}
