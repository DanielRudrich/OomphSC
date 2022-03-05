

#pragma once

#include "Fonts.hpp"
#include <JuceHeader.h>

class IpAndPortComponent : public juce::Component
{
    static constexpr int portWidth = 50;
    static constexpr int spacing = 7;

public:
    enum class State
    {
        disconnected,
        connected,
        error
    };

    IpAndPortComponent()
    {
        colour = juce::Colour (0xFF180DFF);

        auto emptyColour = juce::Colours::black;
        auto backgroundColour = colour.withAlpha (0.1f);

        ip.setFont (Fonts::getRegularFont());
        ip.setColour (juce::TextEditor::backgroundColourId, backgroundColour);
        ip.setColour (juce::TextEditor::textColourId, juce::Colours::black);
        ip.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        ip.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        ip.setJustification (juce::Justification::centred);
        ip.setTextToShowWhenEmpty ("IP ADDRESS", emptyColour);
        ip.onReturnKey = [this]() { returnKeyPressed(); };
        addAndMakeVisible (ip);

        port.setFont (Fonts::getRegularFont());
        port.setColour (juce::TextEditor::backgroundColourId, backgroundColour);
        port.setColour (juce::TextEditor::textColourId, juce::Colours::black);
        port.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        port.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        port.setJustification (juce::Justification::centred);
        port.setInputRestrictions (5, "0123456789");
        port.setTextToShowWhenEmpty ("PORT", emptyColour);
        port.onReturnKey = [this]() { returnKeyPressed(); };
        addAndMakeVisible (port);
    }

    ~IpAndPortComponent() override = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = juce::Rectangle<int> (0, 0, getWidth(), getHeight() / 2);
        bounds.removeFromRight (portWidth);
        auto b = bounds.removeFromRight (spacing);

        g.setFont (Fonts::getRegularFont());
        g.setFont (15);
        g.setColour ({ 110, 110, 110 });
        g.drawText (":", b, juce::Justification::centred);

        auto textBounds = getLocalBounds().reduced (5, 4).toFloat();
        g.setColour (juce::Colours::black);
        g.drawText ("OSC CONNECTION", textBounds, juce::Justification::centredBottom);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom (bounds.getHeight() / 2);

        port.setBounds (bounds.removeFromRight (portWidth));
        bounds.removeFromRight (spacing);
        ip.setBounds (bounds);
    }

    void setIP (const juce::String& ipToSet) { ip.setText (ipToSet); }

    juce::String getIP() const { return ip.getText(); }

    void setPort (int portToSet) { port.setText (portToSet == -1 ? "" : juce::String (portToSet)); }

    int getPort() const { return port.getText().getIntValue(); }

    void setState (State newState)
    {
        state = newState;
        repaint();
    }

    std::function<void()> onReturnKey;

private:
    void returnKeyPressed() const noexcept
    {
        if (onReturnKey)
            onReturnKey();
    }

    juce::Colour colour;
    State state = State::disconnected;

    juce::TextEditor ip;
    juce::TextEditor port;
};
