
#pragma once

#include <JuceHeader.h>
#include <array>

#include "Settings.hpp"

class Bar : public juce::Component
{
public:
    Bar () = default;
    ~Bar() override = default;

    void setColour (juce::Colour colourToSet)
    {
        fillColour = colourToSet;
        repaint();
    }

    void setValue (float newValue)
    {
        if (value == newValue)
            return;

        value = newValue;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();

        g.setColour (fillColour.withAlpha (0.2f));
        g.fillRect (bounds);

        auto height = value * bounds.getHeight();

        g.setColour (fillColour);
        g.fillRect (bounds.removeFromBottom (height));
    }

private:
    juce::Colour fillColour;
    float value = 0.0f;
};

class Visualizer : public juce::Component
{
public:
    Visualizer()
    {
        const std::array<juce::Colour, Settings::numRMS> colours =
        {
            juce::Colours::cornflowerblue,
            juce::Colours::limegreen,
            juce::Colours::orange,
            juce::Colours::orangered,
            juce::Colours::black
        };

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            auto& b = bars[i];
            b.setColour (colours[i]);
            addAndMakeVisible (b);
        }
    }

    ~Visualizer() override = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        constexpr int spacing = 10;
        auto width = (bounds.getWidth() - (Settings::numBands - 1) * spacing) / Settings::numBands;

        for (auto& b : bars)
        {
            b.setBounds (bounds.removeFromLeft (width));
            bounds.removeFromLeft (spacing);
        }
    }

    void resized() override
    {

    }

    void setValues (const std::array<float, Settings::numRMS>& valuesToSet)
    {
        for (size_t i = 0; i < Settings::numBands; ++i)
            bars[i].setValue (valuesToSet[i]);
    }

private:
    std::array<Bar, Settings::numBands> bars;
};
