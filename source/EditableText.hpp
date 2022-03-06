

#pragma once
#include "Fonts.hpp"
#include <JuceHeader.h>

class EditableText : public juce::Component
{
public:
    EditableText (juce::String labelText, juce::Colour mainColour) : label (labelText), colour (mainColour)
    {
    }

    ~EditableText() override = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom (bounds.getHeight() / 2);

        g.setColour (colour.withAlpha (0.10f));
        g.fillRect (bounds);

        g.setFont (Fonts::getRegularFont (15));
        g.setColour (juce::Colours::black);

        auto textBounds = getLocalBounds().reduced (5, 4).toFloat();

        g.drawText (text,
                    textBounds,
                    juce::Justification::centredTop);
        g.drawText (label, textBounds, juce::Justification::centredBottom);
    }

    void setText (const juce::String& newText) { text = newText; repaint(); }
    juce::String getText() const noexcept { return text; }

    void mouseDown ([[maybe_unused]] const juce::MouseEvent& e) override {}

    void mouseDoubleClick ([[maybe_unused]] const juce::MouseEvent& event) override
    {
        if (! editor)
        {
            auto bounds = juce::Rectangle<int> (0, 0, getWidth(), getHeight() / 2);

            editor = std::make_unique<juce::TextEditor>();
            editor->setBounds (bounds);
            editor->setKeyboardType (juce::TextInputTarget::VirtualKeyboardType::decimalKeyboard);
            editor->setFont (Fonts::getRegularFont (15));

            editor->setJustification (juce::Justification::centred);
            editor->setColour (juce::TextEditor::backgroundColourId,
                               juce::Colours::white.overlaidWith (colour.withAlpha (0.1f)));
            editor->setColour (juce::TextEditor::textColourId, juce::Colours::black);
            editor->setColour (juce::TextEditor::highlightedTextColourId, juce::Colours::black);
            editor->setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
            editor->setColour (juce::TextEditor::focusedOutlineColourId,
                               juce::Colours::transparentBlack);
            editor->setText (text);

            addAndMakeVisible (*editor);
            editor->setInputRestrictions (maxLength, allowedCharacters);
            editor->grabKeyboardFocus();
            editor->setHighlightedRegion (juce::Range<int> (0, text.length()));

            editor->onReturnKey = [&]()
            {
                const auto newValue = editor->getText();

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

    void setInputRestrictions (int length, const juce::String& characters)
    {
        maxLength = length;
        allowedCharacters = characters;
    }

private:
    juce::String text;
    juce::String label;
    juce::Colour colour;

    int maxLength = 0;
    juce::String allowedCharacters = juce::String();

    std::unique_ptr<juce::TextEditor> editor;
};
