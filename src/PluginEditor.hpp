#pragma once

#include "PluginProcessor.hpp"

//==============================================================================
class PluginTemplateEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit PluginTemplateEditor (PluginTemplateProcessor&);
    ~PluginTemplateEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


    void timerCallback() override;
    
private:
    PluginTemplateProcessor& processorReference;

    std::array<juce::Slider, 5> slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginTemplateEditor)
};
