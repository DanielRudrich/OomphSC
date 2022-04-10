
#pragma once
#include "Fonts.hpp"
#include <JuceHeader.h>

class MonoStereoButton : public juce::Component
{
public:
    MonoStereoButton (juce::RangedAudioParameter& param) :
        attachment (param, [&] (float value) { parameterChanged (value); })
    {
        attachment.sendInitialUpdate();

        // set up paths
        mono.addEllipse (17, 0, 34.0f, 34.0f);

        stereo.addEllipse (2, 0, 34.0f, 34.0f);
        stereo.addEllipse (32, 0, 34.0f, 34.0f);

        setRepaintsOnMouseActivity (true);
    }

    ~MonoStereoButton() override = default;

    void paint (juce::Graphics& g) override
    {
        auto activeColour = juce::Colour (0xFF9013FE);
        auto inactiveColour = juce::Colour (0xFF414141).withAlpha (isMouseOver() ? 1.0f : 0.5f);

        auto bounds = getLocalBounds();
        auto textBounds = bounds.reduced (5, 4).toFloat();
        auto graphicBounds = bounds.removeFromTop (bounds.getHeight() / 2).toFloat();

        g.setColour (activeColour.withAlpha (0.14f));
        g.fillRect (graphicBounds);

        graphicBounds.reduce (4, 3);

        juce::String parameterLabel;
        juce::Path* activePath;
        juce::Path* inactivePath;
        if (stereoMode)
        {
            parameterLabel = "STEREO";
            activePath = &stereo;
            inactivePath = &mono;
        }
        else
        {
            parameterLabel = "MONO";
            activePath = &mono;
            inactivePath = &stereo;
        }

        g.setColour (inactiveColour);
        g.strokePath (*inactivePath,
                      juce::PathStrokeType (1.0f),
                      inactivePath->getTransformToScaleToFit (graphicBounds, true));

        g.setColour (activeColour);
        g.strokePath (*activePath,
                      juce::PathStrokeType (3.0f),
                      activePath->getTransformToScaleToFit (graphicBounds, true));

        g.setFont (Fonts::getRegularFont (15));
        g.setColour (juce::Colours::black);
        g.drawText (parameterLabel, textBounds, juce::Justification::centredBottom);
    }

    void mouseUp ([[maybe_unused]] const juce::MouseEvent& event) override
    {
        attachment.setValueAsCompleteGesture (! stereoMode);
    }

    void parameterChanged (float value) noexcept
    {
        stereoMode = value > 0.5f;
        repaint();
    }

private:
    juce::Path mono;
    juce::Path stereo;

    bool stereoMode = false;
    juce::ParameterAttachment attachment;
};
