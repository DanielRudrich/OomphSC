
#pragma once
#include "Fonts.hpp"
#include <JuceHeader.h>

class ConnectionButton : public juce::Button
{
public:
    ConnectionButton() : juce::Button ("")
    {
        using namespace juce;

        // set up paths - connected
        connected.startNewSubPath ({ 8.0f, 8.0f });
        connected.lineTo ({ 4.0f, 8.0f });
        connected.cubicTo ({ 1.79f, 8.0f }, { 0.0f, 6.21f }, { 0.0f, 4.0f });
        connected.cubicTo ({ 0.0f, 1.79f }, { 1.79f, 0.0f }, { 4.0f, 0.0f });
        connected.lineTo ({ 8.0f, 0.0f });

        // copy
        auto copy = connected;
        connected.addPath (copy, AffineTransform::rotation (3.1415f, 9.0f, 4.0f));
        // line
        connected.startNewSubPath ({ 3.0f, 4.0f });
        connected.lineTo ({ 15.0f, 4.0f });

        // set up paths - disconnected
        disconnected.startNewSubPath ({ 4.0f, 8.0f });
        disconnected.cubicTo ({ 1.79f, 8.0f }, { 0.0f, 6.21f }, { 0.0f, 4.0f });
        disconnected.cubicTo ({ 0.0f, 1.79f }, { 1.79f, 0.0f }, { 4.0f, 0.0f });
        disconnected.lineTo ({ 8.0f, 0.0f });

        // copy
        copy = disconnected;
        disconnected.addPath (copy, AffineTransform::rotation (3.1415f, 9.0f, 4.0f));
        // line
        disconnected.startNewSubPath ({ 6.0f, 9.2f });
        disconnected.lineTo ({ 12.0f, -1.2f });
    }

    ~ConnectionButton() override = default;

    void paintButton (juce::Graphics& g,
                      bool shouldDrawButtonAsHighlighted,
                      bool /* shouldDrawButtonAsDown */) override
    {
        const auto activeColour = juce::Colour (0xFFFF7E58);

        auto bounds = getLocalBounds();
        auto graphicBounds = bounds.toFloat();

        g.setColour (activeColour.withAlpha (isConnected ? 1.0f : 0.14f));
        g.fillRect (graphicBounds);

        graphicBounds.reduce (4, 5);

        g.setColour (juce::Colours::black);
        g.strokePath (isConnected ? connected : disconnected,
                      juce::PathStrokeType (shouldDrawButtonAsHighlighted ? 1.3f : 1.1f),
                      connected.getTransformToScaleToFit (graphicBounds, false));
    }

    void setConnected (bool shouldBeConnected) noexcept
    {
        isConnected = shouldBeConnected;
        repaint();
    }

private:
    juce::Path connected;
    juce::Path disconnected;

    bool isConnected = false;
};
