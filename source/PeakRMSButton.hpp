
#pragma once
#include <JuceHeader.h>
#include "Fonts.hpp"


class PeakRMSButton : public juce::Component
{
public:
    PeakRMSButton (juce::RangedAudioParameter& param):
    attachment (param, [&] (float value) { parameterChanged (value); })
    {
        attachment.sendInitialUpdate();

        // set up paths
        peak.startNewSubPath ({0.0f, 34.0f});
        peak.cubicTo ({0.0f, 34.0f},  {20.0f, 0.0f}, {34.0f, 0.0f});
        peak.cubicTo ({54.0f, 0.0f},  {68.0f, 34.0f}, {68.0f, 34.0f});

        rms.startNewSubPath ({0.0f, 34.0f});
        rms.cubicTo ({18.0f, 34.0f},  {25.0f, 0.0f}, {34.0f, 0.0f});
        rms.cubicTo ({49.0f, 0.0f}, {50.0f, 34.0f}, {68.0f, 34.0f});

        setRepaintsOnMouseActivity (true);
    }

    ~PeakRMSButton() override = default;

    void paint (juce::Graphics& g) override
    {
        auto activeColour = juce::Colour (0xFF9013FE);
        auto inactiveColour = juce::Colour (0xFF414141).withAlpha (isMouseOver() ? 1.0f : 0.5f);

        auto bounds = getLocalBounds();
        auto textBounds = bounds.reduced (5, 4).toFloat();
        auto graphicBounds = bounds.removeFromTop (bounds.getHeight() / 2).toFloat();

        g.setColour (activeColour.withAlpha (0.14f));
        g.fillRect (graphicBounds);

        graphicBounds.reduce (4, 2);

        juce::String parameterLabel;
        juce::Path* activePath;
        juce::Path* inactivePath;
        if (usesRMS)
        {
            parameterLabel = "RMS";
            activePath = &rms;
            inactivePath = &peak;
        }
        else
        {
            parameterLabel = "PEAK";
            activePath = &peak;
            inactivePath = &rms;
        }

        g.setColour (inactiveColour);
        g.strokePath (*inactivePath, juce::PathStrokeType (1.0f), inactivePath->getTransformToScaleToFit (graphicBounds, false));

        g.setColour (activeColour);
        g.strokePath (*activePath, juce::PathStrokeType (3.0f), activePath->getTransformToScaleToFit (graphicBounds, false));

        g.setFont (Fonts::getRegularFont());
        g.setFont (15);
        g.setColour (juce::Colours::black);
        g.drawText (parameterLabel, textBounds, juce::Justification::centredBottom);
    }

    void mouseUp ([[maybe_unused]]const juce::MouseEvent& event) override
    {
        attachment.setValueAsCompleteGesture (! usesRMS);
    }

    void parameterChanged (float value) noexcept
    {
        usesRMS = value > 0.5f;
        repaint();
    }

private:
    juce::Path peak;
    juce::Path rms;


    bool usesRMS = false;
    juce::ParameterAttachment attachment;
};
