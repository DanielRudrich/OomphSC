
#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>
#include <vector>

#include "Settings.hpp"

class Separator : public juce::Component
{
public:
    Separator (juce::ComponentBoundsConstrainer& constrainerToUse) : constrainer (constrainerToUse)
    {
    }
    ~Separator() override = default;

    void paint (juce::Graphics& g) override
    {
        constexpr auto height = 20.0f;

        g.setColour (juce::Colours::black.withAlpha (isMouseOver() ? 0.6f : 0.2f));

        auto bounds = getLocalBounds();
        auto centre = bounds.getCentre().toFloat();

        g.drawLine ({ centre.translated (0, -height / 2), centre.translated (0, height / 2) },
                    1.0f);
    }

    void mouseEnter (const juce::MouseEvent& e) override
    {
        juce::ignoreUnused (e);
        repaint();
    }

    void mouseExit (const juce::MouseEvent& e) override
    {
        juce::ignoreUnused (e);
        repaint();
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        dragger.startDraggingComponent (this, e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        dragger.dragComponent (this, e, &constrainer);
    }

private:
    juce::ComponentBoundsConstrainer& constrainer;
    juce::ComponentDragger dragger;
};

class Bar : public juce::Component
{
public:
    Bar() = default;
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
        g.fillRect (bounds.removeFromBottom (static_cast<int> (height)));
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
        const std::array<juce::Colour, Settings::numRMS> colours = { juce::Colours::cornflowerblue,
                                                                     juce::Colours::limegreen,
                                                                     juce::Colours::orange,
                                                                     juce::Colours::orangered,
                                                                     juce::Colours::black };

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            auto& b = bars[i];
            b.setColour (colours[i]);
            addAndMakeVisible (b);
        }

        separators.reserve (Settings::numBands - 1);
        for (int i = 0; i < Settings::numBands - 1; ++i)
        {
            separators.push_back (std::make_unique<Separator> (constrainer));
            addAndMakeVisible (*separators.back());
        }

        constrainer.setMinimumOnscreenAmounts (0xffffff, 0xffffff, 0xffffff, 0xffffff);
    }

    ~Visualizer() override = default;

    void resized() override
    {
        auto bounds = getLocalBounds();
        constexpr int spacing = 7;
        auto width = (bounds.getWidth() - (Settings::numBands - 1) * spacing) / Settings::numBands;

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            auto& b = bars[i];
            b.setBounds (bounds.removeFromLeft (width));

            if (i < Settings::numBands - 1)
                separators[i]->setBounds (bounds.removeFromLeft (spacing));
        }
    }

    void setValues (const std::array<float, Settings::numRMS>& valuesToSet)
    {
        for (size_t i = 0; i < Settings::numBands; ++i)
            bars[i].setValue (valuesToSet[i]);
    }

private:
    juce::ComponentBoundsConstrainer constrainer;

    std::array<Bar, Settings::numBands> bars;
    std::vector<std::unique_ptr<Separator>> separators;
};
