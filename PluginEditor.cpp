#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Costruttore
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Costruttore
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor(FlangerAudioProcessor& p,
    juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts)
{
    setSize(750, 550);
    startTimerHz(10); //per aggiornare 10 volte al secondo

    // ====== AREE =======
    delayArea = { 20, 60, 320, 200 };
    modArea = { 370, 60, 360, 200 };
    filterArea = { 20, 300, 320, 200 };
    mixArea = { 370, 300, 360, 200 };

    // ====== DELAY GROUP ======
    setupSlider(delaySlider, "Delay", Parameters::nameDelayTime,
        delayArea.getX() + 45, delayArea.getCentreY() - 55);
    setupSlider(feedbackSlider, "Feedback", Parameters::nameFeedback,
        delayArea.getRight() - 155, delayArea.getCentreY() - 55);

    // ====== MODULATION GROUP ======
    setupSlider(modFrequencySlider, "Rate", Parameters::nameModFrequency,
        modArea.getX() + 20, modArea.getCentreY() - 85);
    setupSlider(phasedeltaSlider, "Phase", Parameters::namePhaseDelta,
        modArea.getCentreX() - 55, modArea.getCentreY() - 85);
    setupSlider(modAmountSlider, "Depth", Parameters::nameModAmount,
        modArea.getRight() - 130, modArea.getCentreY() - 85);

    // ====== WAVEFORM BUTTONS ======
    int waveformButtonY = modArea.getBottom() - 50;
    int waveformButtonX = modArea.getCentreX() - 170; // Allineamento centrato
    setupButton(sineWaveButton, "Sine", Parameters::nameWaveform, waveformButtonX + 0 * 68, waveformButtonY, true,true);
    setupButton(triangleWaveButton, "Triangle", Parameters::nameWaveform, waveformButtonX + 1 * 68, waveformButtonY, true);
    setupButton(sawUpWaveButton, "SawUp", Parameters::nameWaveform, waveformButtonX + 2 * 68, waveformButtonY, true);
    setupButton(sawDownWaveButton, "SawDown", Parameters::nameWaveform, waveformButtonX + 3 * 68, waveformButtonY, true);
    setupButton(squareWaveButton, "Square", Parameters::nameWaveform, waveformButtonX + 4 * 68, waveformButtonY, true);

    sineWaveButton.setName("Sine");
    triangleWaveButton.setName("Triangle");
    sawUpWaveButton.setName("SawUp");
    sawDownWaveButton.setName("SawDown");
    squareWaveButton.setName("Square");

    // ====== FILTER GROUP ======
    setupSlider(filterCutoffSlider, "Cutoff", Parameters::nameFilterCutoff,
        filterArea.getX() + 45, filterArea.getCentreY() - 85);
    setupSlider(filterQualitySlider, "Resonance", Parameters::nameQuality,
        filterArea.getRight() - 155, filterArea.getCentreY() - 85);

    int filterButtonY = filterArea.getBottom() - 50;
    int filterButtonX = filterArea.getCentreX() - 136; // Allineamento centrato
    setupButton(lowpassButton, "LowPass", Parameters::nameFilterType, filterButtonX + 0 * 68, filterButtonY, false,true);
    setupButton(highpassButton, "HighPass", Parameters::nameFilterType, filterButtonX + 1 * 68, filterButtonY, false);
    setupButton(bandpassButton, "BandPass", Parameters::nameFilterType, filterButtonX + 2 * 68, filterButtonY, false);
    setupButton(filterActiveButton, "Active", Parameters::nameFilterActive, filterButtonX + 3 * 68, filterButtonY, false);

	lowpassButton.setName("LowPass");
	highpassButton.setName("HighPass");
	bandpassButton.setName("BandPass");
    filterActiveButton.setName("Active");

    // ====== MIX GROUP ======
    setupSlider(dryWetSlider, "Mix", Parameters::nameDryWet,
        mixArea.getX() + 55, mixArea.getCentreY() - 55);
    setupSlider(outputGainSlider, "Gain", Parameters::nameOutputGain,
        mixArea.getRight() - 155, mixArea.getCentreY() - 55);

    // ====== SET LOOKANDFEEL sui knob ======
    for (auto* s : { &delaySlider, &feedbackSlider, &modFrequencySlider, &modAmountSlider,&phasedeltaSlider,
                     &filterCutoffSlider, &filterQualitySlider, &dryWetSlider, &outputGainSlider })
    {
        s->setLookAndFeel(&knobLNF);
    }

    // ====== SET LOOKANDFEEL sui pulsanti ======
    for (auto* b : { &sineWaveButton, &triangleWaveButton, &sawUpWaveButton, &sawDownWaveButton,
                     &squareWaveButton, &lowpassButton, &highpassButton, &bandpassButton, &filterActiveButton })
    {
        b->setLookAndFeel(&toggleButtonLNF);
    }

    // ====== SET LISTENER ======
    valueTreeState.addParameterListener(Parameters::nameWaveform, this);
    valueTreeState.addParameterListener(Parameters::nameFilterType, this);

    // ====== UNDO / REDO ======
    undoButton.setBounds(20, 20, 60, 25);
    redoButton.setBounds(90, 20, 60, 25);

    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);

    undoButton.onClick = [this]()
        {
            if (auto* um = valueTreeState.undoManager)
                if (um->canUndo())
                    um->undo();
        };

    redoButton.onClick = [this]()
        {
            if (auto* um = valueTreeState.undoManager)
                if (um->canRedo())
                    um->redo();
        };

}

//==============================================================================
// Distruttore
FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
    // Rimuove listener
    valueTreeState.removeParameterListener(Parameters::nameWaveform, this);
    valueTreeState.removeParameterListener(Parameters::nameFilterType, this);

    // Scollega LookAndFeel dai pulsanti
    for (auto* b : { &sineWaveButton, &triangleWaveButton, &sawUpWaveButton, &sawDownWaveButton,
                     &squareWaveButton, &lowpassButton, &highpassButton, &bandpassButton, &filterActiveButton })
    {
        b->setLookAndFeel(nullptr);
    }

    // Scollega LookAndFeel dai knob
	for (auto* s : { &delaySlider, &feedbackSlider, &modFrequencySlider, &modAmountSlider, &phasedeltaSlider,
                     &filterCutoffSlider, &filterQualitySlider, &dryWetSlider, &outputGainSlider })
    {
        s->setLookAndFeel(nullptr);
    }
}


//==============================================================================
// Disegno sfondo e gruppi
void FlangerAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // === Background vintage ===
    juce::ColourGradient backgroundGradient(
        juce::Colour(60, 40, 30), 0, 0,
        juce::Colour(30, 20, 10), 0, (float)bounds.getHeight(), false);
    g.setGradientFill(backgroundGradient);
    g.fillRect(bounds);

    // Texture analogica (righe leggere)
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    for (int i = 0; i < bounds.getHeight(); i += 4)
        g.drawLine(0, (float)i, (float)bounds.getWidth(), (float)i);

    // === Titolo retrò ===
    g.setFont(juce::Font("Courier New", 28.0f, juce::Font::bold));
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawText("FlangeFlicker",
        juce::Rectangle<int>(0, 20, getWidth(), 40).translated(2, 2),
        juce::Justification::centredTop, true);
    g.setColour(juce::Colours::antiquewhite);
    g.drawText("FlangeFlicker",
        juce::Rectangle<int>(0, 20, getWidth(), 40),
        juce::Justification::centredTop, true);

    // === GroupBox ===
    drawGroupBox(g, delayArea, "Delay");
    drawGroupBox(g, modArea, "Modulation");
    drawGroupBox(g, filterArea, "Filter");
    drawGroupBox(g, mixArea, "Mix");

    // === Label sotto ai knob ===
    auto drawLabel = [&](juce::Slider& s)
        {
            auto r = s.getBounds();
            juce::Rectangle<int> labelRect(r.getX(), r.getBottom() + 2, r.getWidth(), 20);

            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.drawFittedText(s.getName(),
                labelRect.translated(1, 1),
                juce::Justification::centred, 1);

            g.setColour(juce::Colours::antiquewhite);
            g.drawFittedText(s.getName(),
                labelRect,
                juce::Justification::centred, 1);
        };

    for (auto* s : { &delaySlider, &feedbackSlider,
					 & modFrequencySlider,& modAmountSlider,& phasedeltaSlider,
                     &filterCutoffSlider, &filterQualitySlider,
                     &dryWetSlider, &outputGainSlider })
        drawLabel(*s);
}


void FlangerAudioProcessorEditor::drawGroupBox(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title)
{
    auto floatArea = area.toFloat();
    float cornerRadius = 8.0f;

    // Sfondo semi-trasparente
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(floatArea, cornerRadius);

    // Bordo completo (linea superiore passa attraverso il titolo)
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(floatArea, cornerRadius, 1.5f);

    // Titolo centrato sulla linea superiore
    juce::Rectangle<int> titleRect = area.removeFromTop(20).reduced(4, 0);
    g.setColour(juce::Colours::antiquewhite);
    g.setFont(16.0f);
    g.drawText(title, titleRect, juce::Justification::centred, false);

    // Viti agli angoli
    auto screw = [&](int cx, int cy, float size = 10.0f)
        {
            float halfSize = size / 2.0f;

            // Cerchio base
            g.setColour(juce::Colours::darkgrey);
            g.fillEllipse((float)cx - halfSize, (float)cy - halfSize, size, size);

            g.setColour(juce::Colours::black);
            g.drawEllipse((float)cx - halfSize, (float)cy - halfSize, size, size, 1.0f);

            // Croce X
            float crossInset = size * 0.3f;
            g.drawLine(cx - halfSize + crossInset, cy - halfSize + crossInset,
                cx + halfSize - crossInset, cy + halfSize - crossInset, 1.0f);
            g.drawLine(cx - halfSize + crossInset, cy + halfSize - crossInset,
                cx + halfSize - crossInset, cy - halfSize + crossInset, 1.0f);
        };

    screw(area.getX() + 8, titleRect.getY() + 8);
    screw(area.getRight() - 8, titleRect.getY() + 8);
    screw(area.getX() + 8, area.getBottom() - 8);
    screw(area.getRight() - 8, area.getBottom() - 8);
}



//==============================================================================
// Ridisegno
void FlangerAudioProcessorEditor::resized()
{
}

//==============================================================================
// Gestione parametri (sync GUI <-> host)
void FlangerAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == Parameters::nameWaveform)
    {
        int selectedIndex = static_cast<int>(std::round(newValue));
        sineWaveButton.setToggleState(selectedIndex == 0, juce::dontSendNotification);
        triangleWaveButton.setToggleState(selectedIndex == 1, juce::dontSendNotification);
        sawUpWaveButton.setToggleState(selectedIndex == 2, juce::dontSendNotification);
        sawDownWaveButton.setToggleState(selectedIndex == 3, juce::dontSendNotification);
        squareWaveButton.setToggleState(selectedIndex == 4, juce::dontSendNotification);
    }
    else if (parameterID == Parameters::nameFilterType)
    {
        int selectedIndex = static_cast<int>(std::round(newValue));
        lowpassButton.setToggleState(selectedIndex == 0, juce::dontSendNotification);
        highpassButton.setToggleState(selectedIndex == 1, juce::dontSendNotification);
        bandpassButton.setToggleState(selectedIndex == 2, juce::dontSendNotification);
    }
}
// ================== Setup Slider ==================
void FlangerAudioProcessorEditor::setupSlider(juce::Slider& slider,
    const juce::String& name,
    const juce::String& paramID,
    int x, int y)
{
    addAndMakeVisible(slider);
    slider.setName(name);
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 60, 25);
    slider.setLookAndFeel(&knobLNF);

    // Dimensione knob ottimizzata
    slider.setBounds(x, y, 110, 110);

    // Attach al parametro
    if (paramID == Parameters::nameDelayTime)
        delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameFeedback)
        feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameModFrequency)
        modFrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::namePhaseDelta)
        phaseDeltatAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameModAmount)
        modAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameFilterCutoff)
        filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameQuality)
        filterQualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameDryWet)
        dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
    else if (paramID == Parameters::nameOutputGain)
        outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, paramID, slider);
}

// ================== Setup Button ==================
void FlangerAudioProcessorEditor::setupButton(juce::TextButton& button,
    const juce::String& text,
    const juce::String& paramID,
    int x, int y,
    bool isWaveform,
    bool defaultSelected)   // aggiungi flag
{
    addAndMakeVisible(button);
    button.setButtonText(text);
    button.setClickingTogglesState(true);
    button.setBounds(x, y, 65, 28);

    // Imposta LookAndFeel uniforme
    button.setLookAndFeel(&toggleButtonLNF);

    // Imposta lo stato iniziale se è il pulsante di default
    if (defaultSelected)
        button.setToggleState(true, juce::dontSendNotification);

    // Solo il pulsante "Active" ha attachment diretto
    if (paramID == Parameters::nameFilterActive)
    {
        filterActiveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            valueTreeState, paramID, button);
    }

    // OnClick → selezione esclusiva per waveform e filtro
    button.onClick = [this, &button, paramID, isWaveform]()
        {
            if (isWaveform)
            {
                for (auto* b : { &sineWaveButton, &triangleWaveButton, &sawUpWaveButton, &sawDownWaveButton, &squareWaveButton })
                    b->setToggleState(b == &button, juce::dontSendNotification);

                int index = 0;
                if (&button == &triangleWaveButton) index = 1;
                else if (&button == &sawUpWaveButton) index = 2;
                else if (&button == &sawDownWaveButton) index = 3;
                else if (&button == &squareWaveButton) index = 4;

                if (auto* p = valueTreeState.getParameter(paramID))
                    p->setValueNotifyingHost(p->convertTo0to1((float)index));
            }
            else if (paramID == Parameters::nameFilterType)
            {
                for (auto* b : { &lowpassButton, &highpassButton, &bandpassButton })
                    b->setToggleState(b == &button, juce::dontSendNotification);

                int index = 0;
                if (&button == &highpassButton) index = 1;
                else if (&button == &bandpassButton) index = 2;

                if (auto* p = valueTreeState.getParameter(paramID))
                    p->setValueNotifyingHost(p->convertTo0to1((float)index));
            }
        };
}

void FlangerAudioProcessorEditor::timerCallback()
{
    if (auto* um = valueTreeState.undoManager)
    {
        undoButton.setEnabled(um->canUndo());
        redoButton.setEnabled(um->canRedo());
    }
}
