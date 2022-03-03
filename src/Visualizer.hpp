
#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>
#include <vector>

#include "Settings.hpp"

class Separator : public juce::Component
{
public:
    Separator (juce::ComponentBoundsConstrainer& constrainerToUse,
               juce::RangedAudioParameter& param) :
        constrainer (constrainerToUse),
        attachment (param, [&] (float value) { parameterChanged (value); })
    {
    }

    ~Separator() override = default;

    void init() { attachment.sendInitialUpdate(); }

    void paint (juce::Graphics& g) override
    {
        constexpr auto height = 20.0f;
        const auto mouseOver = isMouseOverOrDragging();

        if (mouseOver)
        {
            g.setColour (juce::Colours::black.withAlpha (0.1f));
            g.fillAll();
        }

        g.setColour (juce::Colours::black.withAlpha (mouseOver ? 0.6f : 0.2f));

        auto bounds = getLocalBounds();
        auto centre = bounds.getCentre().toFloat();

        g.drawLine ({ centre.translated (0, -height / 2), centre.translated (0, height / 2) },
                    1.0f);
    }

    void mouseEnter (const juce::MouseEvent& e) override
    {
        juce::ignoreUnused (e);
        repaint();
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
    }

    void mouseExit (const juce::MouseEvent& e) override
    {
        juce::ignoreUnused (e);
        repaint();
        setMouseCursor (juce::MouseCursor::NormalCursor);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        dragger.startDraggingComponent (this, e);
        attachment.beginGesture();
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        dragger.dragComponent (this, e, &constrainer);
        auto xPos = getBoundsInParent().getCentreX();
        attachment.setValueAsPartOfGesture (xToFrequency (xPos));
    }

    void mouseUp ([[maybe_unused]] const juce::MouseEvent& e) override { attachment.endGesture(); }

    void parameterChanged (float value) noexcept
    {
        const auto intValue = juce::roundToInt (value);
        frequency = intValue;

        auto bounds = getLocalBounds();
        auto c = bounds.getCentre();

        c.x = frequencyToX (intValue);

        setCentrePosition (c);
    }

    int frequencyToX (int frequencyInHz) const noexcept
    {
        const auto w = getParentWidth();
        return static_cast<int> (
            w * (std::log (frequencyInHz / 20.0f) / std::log (20'000.0f / 20.0f)));
    }

    int xToFrequency (int xPosition) const noexcept
    {
        const auto w = getParentWidth();
        return juce::roundToInt (
            20 * std::pow (20'000.0f / 20.0f, static_cast<float> (xPosition) / w));
    }

    int getFrequency() const noexcept { return frequency; }

private:
    int frequency = 0;
    juce::ComponentBoundsConstrainer& constrainer;
    juce::ComponentDragger dragger;
    juce::ParameterAttachment attachment;
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

class Visualizer : public juce::Component, public juce::ComponentListener
{
    static constexpr float relativeFullBandWidth = 0.14f;
    static constexpr int separatorWidth = 7;
    static constexpr int textRowHeight = 20;

public:
    Visualizer (juce::AudioProcessorValueTreeState& params)
    {
        const std::array<juce::Colour, Settings::numRMS> colours = { juce::Colours::cornflowerblue,
                                                                     juce::Colours::limegreen,
                                                                     juce::Colours::orange,
                                                                     juce::Colours::orangered,
                                                                     juce::Colours::black };

        fullBandBar.setColour (colours[Settings::numBands]);
        addAndMakeVisible (fullBandBar);

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            auto& b = bars[i];
            b.setColour (colours[i]);
            barsParent.addAndMakeVisible (b);
        }
        addAndMakeVisible (barsParent);

        separators.reserve (Settings::numBands - 1);

        separators.push_back (std::make_unique<Separator> (
            constrainer,
            *params.getParameter (Settings::Parameters::CrossOver1::id)));
        separators.push_back (std::make_unique<Separator> (
            constrainer,
            *params.getParameter (Settings::Parameters::CrossOver2::id)));
        separators.push_back (std::make_unique<Separator> (
            constrainer,
            *params.getParameter (Settings::Parameters::CrossOver3::id)));

        for (auto& c : separators)
        {
            barsParent.addAndMakeVisible (*c);
            c->addComponentListener (this);
        }

        constrainer.setMinimumOnscreenAmounts (0xffffff, 0xffffff, 0xffffff, 0xffffff);

        for (auto& f : frequencies)
        {
            f.setFontHeight (13);
            f.setColour (juce::Colours::grey);
        }
    }

    ~Visualizer() override = default;

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom (textRowHeight);

        const auto offset = static_cast<int> (relativeFullBandWidth * getWidth());

        fullBandBar.setBounds (bounds.removeFromLeft (offset));
        bounds.removeFromLeft (2 * separatorWidth);

        barsParent.setBounds (bounds);

        for (auto& c : separators)
        {
            c->setSize (separatorWidth, bounds.getHeight());
            c->init();
        }
    }

    void setValues (const std::array<float, Settings::numRMS>& valuesToSet)
    {
        for (size_t i = 0; i < Settings::numBands; ++i)
            bars[i].setValue (valuesToSet[i]);

        fullBandBar.setValue (valuesToSet[Settings::numBands]);
    }

    void componentMovedOrResized ([[maybe_unused]] juce::Component& component,
                                  bool wasMoved,
                                  [[maybe_unused]] bool wasResized) override
    {
        if (! wasMoved)
            return;

        const auto xOffset = barsParent.getPosition().getX();

        std::array<int, Settings::numBands> xPositions;
        for (size_t i = 0; i < Settings::numBands - 1; ++i)
        {
            xPositions[i] = separators[i]->getPosition().x;

            // labels
            const auto text = juce::String (separators[i]->getFrequency());
            frequencies[i].setText (text);
            const auto textWidth = frequencies[i].getFont().getStringWidth (text);

            auto origin =
                juce::Point<float> (xOffset + xPositions[i] - 0.5f * (textWidth - separatorWidth),
                                    static_cast<float> (getHeight() - textRowHeight));

            if (origin.x < xOffset)
                origin.x = xOffset;
            else if (auto excess = origin.x + textWidth - barsParent.getRight(); excess > 0)
                origin.x -= excess;

            frequencies[i].setOriginWithOriginalSize (origin);
        }

        xPositions[Settings::numBands - 1] = getWidth();

        std::sort (xPositions.begin(), xPositions.end());

        int xLast = 0;

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            bars[i].setBounds (xLast, 0, xPositions[i] - xLast, getHeight() - textRowHeight);
            xLast = xPositions[i] + separatorWidth; // magic number alert!
        }

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        for (auto& f : frequencies)
            f.draw (g, 1.0f);

        g.setFont (12);
        g.setColour (juce::Colours::grey);

        auto bounds = getLocalBounds();
        const auto offset = static_cast<int> (relativeFullBandWidth * getWidth());
        bounds = bounds.removeFromBottom (textRowHeight);
        g.drawText ("FULLBAND", bounds.removeFromLeft (offset), juce::Justification::centred);
    }

private:
    juce::ComponentBoundsConstrainer constrainer;

    Bar fullBandBar;
    Component barsParent;
    std::array<Bar, Settings::numBands> bars;
    std::vector<std::unique_ptr<Separator>> separators;

    std::array<juce::DrawableText, Settings::numBands - 1> frequencies;
};
