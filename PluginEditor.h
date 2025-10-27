#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyTheme.h" 
//==============================================================================
class FlangerAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::AudioProcessorValueTreeState::Listener,
    private juce::Timer
{
public:
    FlangerAudioProcessorEditor(FlangerAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~FlangerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;


private:
    // === Helpers ===
    void setupSlider(juce::Slider& slider, const juce::String& name,
        const juce::String& paramID, int x, int y);
    void setupButton(juce::TextButton& button, const juce::String& text, const juce::String& paramID, int x, int y, bool isWaveform, bool defaultSelected=false);
    
    void drawGroupBox(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title);

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // === Riferimenti Processor / State ===
    FlangerAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    // === Sliders ===
    juce::Slider delaySlider;
    juce::Slider feedbackSlider;
    juce::Slider modFrequencySlider;
    juce::Slider modAmountSlider;
    juce::Slider filterCutoffSlider;
    juce::Slider filterQualitySlider;
    juce::Slider dryWetSlider;
    juce::Slider outputGainSlider;
    juce::Slider phasedeltaSlider;

    // === Buttons Waveform ===
    juce::TextButton sineWaveButton;
    juce::TextButton triangleWaveButton;
    juce::TextButton sawUpWaveButton;
    juce::TextButton sawDownWaveButton;
    juce::TextButton squareWaveButton;


    // === Buttons Filter ===
    juce::TextButton lowpassButton;
    juce::TextButton highpassButton;
    juce::TextButton bandpassButton;
    juce::TextButton filterActiveButton;

    // === Attachments ===
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modFrequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseDeltatAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterQualityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> filterActiveAttachment;

    // === LookAndFeel instances ===
    KnobLookAndFeel knobLNF;
    CustomToggleButtonLookAndFeel toggleButtonLNF;


    juce::TextButton undoButton{ "Undo" };
    juce::TextButton redoButton{ "Redo" };

    // === Aree gruppi ===
    juce::Rectangle<int> delayArea;
    juce::Rectangle<int> modArea;
    juce::Rectangle<int> filterArea;
    juce::Rectangle<int> mixArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessorEditor)
};
