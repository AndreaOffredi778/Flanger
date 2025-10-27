#pragma once

#include <JuceHeader.h>

// ===== KNOB =====
class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel()
    {
        background = juce::Colour::fromRGB(45, 45, 48);
        knobFace = juce::Colour::fromRGB(205, 205, 195);
        knobEdge = juce::Colour::fromRGB(40, 40, 40);
        knobShadow = juce::Colour::fromRGB(15, 15, 15);
        knobArc = juce::Colour::fromRGB(240, 190, 60);
        textColour = juce::Colours::whitesmoke;

        setColour(juce::Slider::textBoxTextColourId, textColour);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::textColourId, textColour);
    }

    void fillBackground(juce::Graphics& g, juce::Component& c)
    {
        auto bounds = c.getLocalBounds().toFloat();
        juce::ColourGradient grad(
            background.brighter(0.1f), 0, 0,
            background.darker(0.25f), 0, bounds.getBottom(), false);
        g.setGradientFill(grad);
        g.fillAll();
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width, height) / 2.0f - 6.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto bounds = juce::Rectangle<float>(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        auto currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(knobShadow.withAlpha(0.6f));
        g.fillEllipse(bounds.translated(3, 3).expanded(2));

        juce::ColourGradient faceGrad(knobFace.darker(0.15f), bounds.getCentre(),
            knobFace.darker(0.35f), bounds.getBottomRight(), true);
        g.setGradientFill(faceGrad);
        g.fillEllipse(bounds);

        g.setColour(knobEdge.darker(0.2f));
        g.drawEllipse(bounds, 6.0f);

        g.setColour(knobShadow.withAlpha(0.2f));
        g.drawEllipse(bounds.reduced(3.0f), 4.0f);

        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius + 3.0f, radius + 3.0f,
            0.0f, rotaryStartAngle, currentAngle, true);
        g.setColour(knobArc);
        g.strokePath(arc, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        float dotRadius = 4.5f;
        float dotDistance = radius * 0.65f;
        auto dotAngle = currentAngle - juce::MathConstants<float>::halfPi;

        juce::Point<float> dot(
            centreX + dotDistance * std::cos(dotAngle),
            centreY + dotDistance * std::sin(dotAngle)
        );

        g.setColour(knobShadow.withAlpha(0.6f));
        g.fillEllipse(dot.x - dotRadius + 1, dot.y - dotRadius + 1, dotRadius * 2, dotRadius * 2);

        g.setColour(knobEdge.darker(0.1f));
        g.fillEllipse(dot.x - dotRadius, dot.y - dotRadius, dotRadius * 2, dotRadius * 2);
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        juce::Label* label = juce::LookAndFeel_V4::createSliderTextBox(slider);
        label->setFont(juce::Font("Helvetica", 14.0f, juce::Font::bold));
        label->setJustificationType(juce::Justification::centred);
        label->setEditable(true, true, false);
        return label;
    }

    static KnobLookAndFeel& getInstance()
    {
        static KnobLookAndFeel instance;
        return instance;
    }

private:
    juce::Colour background, knobFace, knobEdge, knobShadow, knobArc, textColour;
};

class CustomButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour&, bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

        // Ombra
        juce::DropShadow shadow(juce::Colours::black.withAlpha(0.5f), 4, { 2, 2 });
        shadow.drawForRectangle(g, bounds.getSmallestIntegerContainer());

        // Corpo pulsante
        juce::Colour base = button.getToggleState() ? juce::Colours::orange : juce::Colours::darkgrey;
        if (shouldDrawButtonAsHighlighted) base = base.brighter(0.2f);
        if (shouldDrawButtonAsDown) base = base.darker(0.2f);

        g.setColour(base);
        g.fillRoundedRectangle(bounds, 6.0f);

        // Bordo nero
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);
    }
};

// ===== TOGGLE BUTTON LOOK AND FEEL CON SIMBOLI (Waveform + Filter) =====
class CustomToggleButtonLookAndFeel : public CustomButtonLookAndFeel
{
public:
    CustomToggleButtonLookAndFeel() = default;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        CustomButtonLookAndFeel::drawButtonBackground(g, button, backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        auto bounds = button.getLocalBounds().toFloat();
        float squareSize = juce::jmin(bounds.getWidth(), bounds.getHeight());
        juce::Rectangle<float> square(bounds.getCentreX() - squareSize / 2.0f,
            bounds.getCentreY() - squareSize / 2.0f,
            squareSize, squareSize);
        square = square.reduced(2.0f);

        juce::Colour innerColor = button.getToggleState() ? juce::Colours::darkorange : juce::Colours::grey;
        if (shouldDrawButtonAsHighlighted) innerColor = innerColor.brighter(0.1f);
        if (shouldDrawButtonAsDown) innerColor = innerColor.darker(0.1f);

        g.setColour(innerColor);
        g.fillRoundedRectangle(square, 4.0f);
    }

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        juce::Rectangle<float> square(bounds.getCentreX() - size / 2.0f,
            bounds.getCentreY() - size / 2.0f,
            size, size);
        square = square.reduced(6.0f);

        juce::Path p;
        auto name = button.getName();

        if (name == "Sine") {
            p.startNewSubPath(square.getX(), square.getCentreY());
            for (int x = 0; x < square.getWidth(); ++x) {
                float y = std::sin(juce::MathConstants<float>::twoPi * x / square.getWidth());
                p.lineTo(square.getX() + x, square.getCentreY() - y * square.getHeight() * 0.4f);
            }
        }
        else if (name == "Triangle") {
            float leftX = square.getX();
            float rightX = square.getRight();
            float centerX = square.getCentreX();
            float topY = square.getY();
            float bottomY = square.getBottom();

            // Triangolo senza base
            p.startNewSubPath(leftX, bottomY);
            p.lineTo(centerX, topY);
            p.lineTo(rightX, bottomY);

            
        }

        else if (name == "SawUp") {
            float leftX = square.getX();
            float rightX = square.getRight();
            float topY = square.getY();
            float bottomY = square.getBottom();

            // linea principale
            p.startNewSubPath(leftX, bottomY);
            p.lineTo(rightX, topY);

            // linee verticali ai lati
            p.startNewSubPath(leftX, topY);
            p.lineTo(leftX, bottomY);

            p.startNewSubPath(rightX, topY);
            p.lineTo(rightX, bottomY);
        }
        else if (name == "SawDown") {
            float leftX = square.getX();
            float rightX = square.getRight();
            float topY = square.getY();
            float bottomY = square.getBottom();

            // linea principale
            p.startNewSubPath(leftX, topY);
            p.lineTo(rightX, bottomY);

            // linee verticali ai lati
            p.startNewSubPath(leftX, topY);
            p.lineTo(leftX, bottomY);

            p.startNewSubPath(rightX, topY);
            p.lineTo(rightX, bottomY);
        }

        else if (name == "Square")
        {
            float midY = square.getCentreY();
            float topY = square.getY();
            float bottomY = square.getBottom();

            float stepW = square.getWidth() / 4.0f; // 4 step orizzontali

            p.startNewSubPath(square.getX(), bottomY);

            // primo gradino verso l'alto
            p.lineTo(square.getX(), topY);
            p.lineTo(square.getX() + stepW, topY);
            // scende
            p.lineTo(square.getX() + stepW, bottomY);
            p.lineTo(square.getX() + stepW * 2, bottomY);
            // sale
            p.lineTo(square.getX() + stepW * 2, topY);
            p.lineTo(square.getX() + stepW * 3, topY);
            // scende
            p.lineTo(square.getX() + stepW * 3, bottomY);
            p.lineTo(square.getRight(), bottomY);
        }

        else if (name == "LowPass") {
            // Linea orizzontale piatta poi calo (classico low-pass)
            float midY = square.getCentreY();
            float topY = square.getY() + square.getHeight() * 0.25f;

            p.startNewSubPath(square.getX(), midY);
            p.lineTo(square.getCentreX(), midY);
            p.lineTo(square.getRight(), topY);
        }

        else if (name == "HighPass") {
            // Crescita rapida da basso a piatto (classico high-pass)
            float midY = square.getCentreY();
            float bottomY = square.getBottom() - square.getHeight() * 0.25f;

            p.startNewSubPath(square.getX(), bottomY);
            p.lineTo(square.getCentreX(), midY);
            p.lineTo(square.getRight(), midY);
        }

        else if (name == "BandPass") {
            // Banda che sale al centro e poi scende (classico band-pass)
            float midY = square.getCentreY();
            float topY = square.getY() + square.getHeight() * 0.25f;

            p.startNewSubPath(square.getX(), midY);
            p.lineTo(square.getCentreX() - square.getWidth() * 0.25f, midY);
            p.lineTo(square.getCentreX(), topY);
            p.lineTo(square.getCentreX() + square.getWidth() * 0.25f, midY);
            p.lineTo(square.getRight(), midY);
        }

        else if(name == "Active"){
            juce::String label = button.getToggleState() ? "On" : "Off";

            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            g.drawFittedText(label, square.toNearestInt(), juce::Justification::centred, 1);
        }

        
        else {
            return;
        }

        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(p, juce::PathStrokeType(2.0f), juce::AffineTransform::translation(1.5f, 1.5f));
        g.setColour(juce::Colours::white);
        g.strokePath(p, juce::PathStrokeType(2.0f));
    }
};
