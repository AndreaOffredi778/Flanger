#pragma once
#include <JuceHeader.h>

namespace Parameters
{
    // Param Names
    static constexpr auto nameDelayTime = "delayTime";
    static constexpr auto nameFeedback = "feedback";
    static constexpr auto nameDryWet = "dryWet";
    static constexpr auto nameWaveform = "waveform";
    static constexpr auto nameModFrequency = "modFrequency";
    static constexpr auto nameModAmount = "modAmount";
    static constexpr auto namePhaseDelta = "phaseDelta";
    static constexpr auto nameFilterActive = "filterActive";
    static constexpr auto nameQuality = "quality";
    static constexpr auto nameFilterType = "filterType";
    static constexpr auto nameFilterCutoff = "filterCutoff";
    static constexpr auto nameOutputGain = "outputGain";

    // Defaults for Flanger
    static constexpr float defaultDelay = 5.0f;   // ms, tipico flanger corto
    static constexpr float defaultFeedback = 0.3f;   // 0..1
    static constexpr float defaultDryWet = 0.0f;   
    static constexpr int   defaultWaveform = 0;      // Sine
    static constexpr float defaultModFrequency = 0.25f; // Hz, lento per flanger
    static constexpr float defaultModAmount = 0.5f;   // 0..1
    static constexpr float defaultPhaseDelta = 0.0f;   // 0..1
    static constexpr bool  defaultFilterActive = false;
    static constexpr float defaultQuality = 0.707f; // 1/sqrt(2)
    static constexpr int   defaultFilterType = 0;      // LowPass
    static constexpr float defaultFilterCutoff = 2000.0f; // Hz
    static constexpr float defaultOutputGain = 0.0f;   // dB
    static constexpr float dbFloor = -48.0f;

    // Parameter Layout
    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        using APF = juce::AudioParameterFloat;
        using APC = juce::AudioParameterChoice;
        using APB = juce::AudioParameterBool;

        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // ====== Flanger parameters ======
        params.emplace_back(std::make_unique<APF>(Parameters::nameDelayTime, "Delay Time (ms)",
            juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f, 0.5f), Parameters::defaultDelay));

        params.emplace_back(std::make_unique<APF>(Parameters::nameFeedback, "Feedback",
            juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), Parameters::defaultFeedback));

        params.emplace_back(std::make_unique<APF>(Parameters::nameDryWet, "Dry/Wet",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), Parameters::defaultDryWet));

        params.emplace_back(std::make_unique<APC>(Parameters::nameWaveform, "Waveform",
            juce::StringArray{ "Sine", "Triangle", "Saw Up", "Saw Down", "Square" },
            Parameters::defaultWaveform));

        params.emplace_back(std::make_unique<APF>(Parameters::nameModFrequency, "Mod Frequency (Hz)",
            juce::NormalisableRange<float>(0.01f, 5.0f, 0.01f, 0.3f), Parameters::defaultModFrequency));

        params.emplace_back(std::make_unique<APF>(Parameters::nameModAmount, "Mod Amount",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), Parameters::defaultModAmount));

        params.emplace_back(std::make_unique<APF>(Parameters::namePhaseDelta, "Phase Delta",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), Parameters::defaultPhaseDelta));

        // ====== Filter parameters ======
        params.emplace_back(std::make_unique<APB>(Parameters::nameFilterActive, "Filter Active",
            Parameters::defaultFilterActive));

        params.emplace_back(std::make_unique<APC>(Parameters::nameFilterType, "Filter Type",
            juce::StringArray{ "LowPass", "HighPass", "BandPass" }, Parameters::defaultFilterType));

        params.emplace_back(std::make_unique<APF>(Parameters::nameFilterCutoff, "Filter Cutoff (Hz)",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), Parameters::defaultFilterCutoff));

        params.emplace_back(std::make_unique<APF>(Parameters::nameQuality, "Filter Quality (Q)",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), Parameters::defaultQuality));

        // ====== Output parameters ======
        params.emplace_back(std::make_unique<APF>(Parameters::nameOutputGain, "Output Gain (dB)",
            juce::NormalisableRange<float>(Parameters::dbFloor, 12.0f, 0.5f), Parameters::defaultOutputGain));

        return { params.begin(), params.end() };
    }

    // Utility per aggiungere/rimuovere listener
    inline void addListenerToAllParameters(juce::AudioProcessorValueTreeState& vts, juce::AudioProcessorValueTreeState::Listener* listener)
    {
        for (auto* id : { nameDelayTime, nameFeedback, nameDryWet, nameWaveform, nameModFrequency,
                          nameModAmount, namePhaseDelta, nameFilterActive, nameQuality,
                          nameFilterType, nameFilterCutoff, nameOutputGain })
        {
            vts.addParameterListener(id, listener);
        }
    }

    inline void removeListenerFromAllParameters(juce::AudioProcessorValueTreeState& vts, juce::AudioProcessorValueTreeState::Listener* listener)
    {
        for (auto* id : { nameDelayTime, nameFeedback, nameDryWet, nameWaveform, nameModFrequency,
                          nameModAmount, namePhaseDelta, nameFilterActive, nameQuality,
                          nameFilterType, nameFilterCutoff, nameOutputGain })
        {
            vts.removeParameterListener(id, listener);
        }
    }
}
