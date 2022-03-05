

#pragma once
#include "Fonts.hpp"
#include <JuceHeader.h>

class Slider : public juce::Component
{
public:
    Slider (juce::RangedAudioParameter& param, juce::String label, juce::Colour sliderColour) :
        parameter (param),
        text (label),
        colour (sliderColour),
        attachment (param, [&] (float value) { parameterChanged (value); })
    {
        attachment.sendInitialUpdate();
    }

    ~Slider() override = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom (bounds.getHeight() / 2);

        g.setColour (colour.withAlpha (0.10f));
        g.fillRect (bounds);
        g.setColour (colour);
        g.fillRect (bounds.removeFromLeft (juce::roundToInt (normalizedValue * bounds.getWidth())));

        g.setFont (Fonts::getRegularFont());
        g.setFont (15);
        g.setColour (juce::Colours::black);

        auto textBounds = getLocalBounds().reduced (5, 4).toFloat();
        auto value = juce::roundToInt (parameter.convertFrom0to1 (parameter.getValue()));
        g.drawText (juce::String (value) + " " + parameter.getLabel(),
                    textBounds,
                    juce::Justification::centredTop);
        g.drawText (text, textBounds, juce::Justification::centredBottom);
    }

    void mouseDown ([[maybe_unused]] const juce::MouseEvent& e) override
    {
        mouseDownValue = normalizedValue;
        attachment.beginGesture();
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto diff = (e.position.x - e.mouseDownPosition.x) + (e.mouseDownPosition.y - e.position.y);

        normalizedValue = std::clamp (mouseDownValue + 0.004f * diff, 0.0f, 1.0f);
        attachment.setValueAsPartOfGesture (parameter.convertFrom0to1 (normalizedValue));
    }

    void mouseUp ([[maybe_unused]] const juce::MouseEvent& event) override
    {
        attachment.endGesture();
    }

    void mouseDoubleClick ([[maybe_unused]] const juce::MouseEvent& event) override
    {
        if (! editor)
        {
            auto bounds = juce::Rectangle<int> (0, 0, getWidth(), getHeight() / 2);
            auto value = juce::roundToInt (parameter.convertFrom0to1 (parameter.getValue()));
            auto textValue = juce::String (value);

            editor = std::make_unique<juce::TextEditor>();
            editor->setBounds (bounds);
            editor->setKeyboardType (juce::TextInputTarget::VirtualKeyboardType::decimalKeyboard);
            editor->setText (textValue);
            editor->setFont (Fonts::getRegularFont());
            editor->setJustification (juce::Justification::centred);
            editor->setColour (juce::TextEditor::backgroundColourId,
                               juce::Colours::white.overlaidWith (colour.withAlpha (0.1f)));
            editor->setColour (juce::TextEditor::textColourId, juce::Colours::black);
            editor->setColour (juce::TextEditor::highlightedTextColourId, juce::Colours::black);
            editor->setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
            editor->setColour (juce::TextEditor::focusedOutlineColourId,
                               juce::Colours::transparentBlack);

            addAndMakeVisible (*editor);

            editor->grabKeyboardFocus();
            editor->setHighlightedRegion (juce::Range<int> (0, textValue.length()));

            editor->onReturnKey = [&]()
            {
                const auto newValue = static_cast<float> (editor->getText().getIntValue());
                attachment.setValueAsCompleteGesture (newValue);
                hideEditor();
            };

            editor->onEscapeKey = [&]() { hideEditor(); };
            editor->onFocusLost = [&]() { hideEditor(); };

            enterModalState (false);
        }
    }

    void inputAttemptWhenModal() override { hideEditor(); }

    void hideEditor()
    {
        if (editor)
        {
            editor.reset();
            exitModalState (0);
        }
    }

    void parameterChanged (float value) noexcept
    {
        normalizedValue = parameter.convertTo0to1 (value);
        repaint();
    }

private:
    juce::RangedAudioParameter& parameter;
    juce::String text;
    juce::Colour colour;

    float normalizedValue = 0.0f;
    float mouseDownValue;

    std::unique_ptr<juce::TextEditor> editor;
    juce::ParameterAttachment attachment;
};
