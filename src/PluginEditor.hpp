#pragma once

#include "OSCComponent.hpp"
#include "PluginProcessor.hpp"

#include "Visualizer.hpp"

//==============================================================================
class OomphSCEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit OomphSCEditor (OomphSCProcessor&);
    ~OomphSCEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    juce::LookAndFeel_V4 laf;
    OomphSCProcessor& processorReference;
    OSCComponent oscComponent;

    // parameter controls
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider attack;
    std::unique_ptr<SliderAttachment> attackAttachment;
    juce::Slider release;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    std::array<juce::Slider, 3> crossOverFrequencies;
    std::array<std::unique_ptr<SliderAttachment>, 3> crossOverFrequenciesAttachments;

    // rms visuals
    Visualizer visualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OomphSCEditor)
};
