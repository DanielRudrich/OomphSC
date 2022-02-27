

#pragma once

#include <JuceHeader.h>


class IpAndPortComponent : public juce::Component
{
    static constexpr int portWidth = 50;
    static constexpr int spacing = 5;

public:
    enum class State
    {
        disconnected,
        connected,
        error
    };

    IpAndPortComponent()
    {
        juce::Colour emptyColour {110, 110, 110};
        ip.setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
        ip.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        ip.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        ip.setJustification (juce::Justification::centred);
        ip.setTextToShowWhenEmpty ("IP Address", emptyColour);
        ip.onReturnKey = [this] () { returnKeyPressed(); };
        addAndMakeVisible (ip);

        port.setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
        port.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        port.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        port.setJustification (juce::Justification::centred);
        port.setInputRestrictions (5, "0123456789");
        port.setTextToShowWhenEmpty ("Port", emptyColour);
        port.onReturnKey = [this] () { returnKeyPressed(); };
        addAndMakeVisible (port);
    }


    ~IpAndPortComponent() override = default;


    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().reduced (2).toFloat();

        switch (state)
        {
            case State::disconnected:
                g.setColour ({110, 110, 110});
                break;
            case State::connected:
                g.setColour (juce::Colours::limegreen);
                break;
            case State::error:
                g.setColour (juce::Colours::orangered);
                break;
        };

        g.drawRoundedRectangle (bounds, bounds.getHeight() / 2, 1.0f);

        bounds.removeFromRight (portWidth);
        auto b = bounds.removeFromRight (spacing);

        g.setColour ({110, 110, 110});
        g.drawText (":", b, juce::Justification::centred);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (2);
        port.setBounds (bounds.removeFromRight (portWidth));
        bounds.removeFromRight (spacing);
        ip.setBounds (bounds);
    }


    void setIP (const juce::String& ipToSet)
    {
        ip.setText (ipToSet);
    }

    juce::String getIP() const
    {
        return ip.getText();
    }


    void setPort (int portToSet)
    {
        port.setText (portToSet == -1 ? "" : juce::String (portToSet));
    }

    int getPort() const
    {
        return port.getText().getIntValue();
    }

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

    State state = State::disconnected;

    juce::TextEditor ip;
    juce::TextEditor port;
};
