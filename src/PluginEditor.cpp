#include "PluginEditor.hpp"

//==============================================================================
PluginTemplateEditor::PluginTemplateEditor (PluginTemplateProcessor& p) :
    AudioProcessorEditor (&p), processorReference (p)
{
    using namespace juce;
    for (auto& s : slider)
    {
        s.setSliderStyle (Slider::LinearBarVertical);
        s.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        s.setRange (0, 1);
        addAndMakeVisible (s);
    }

    setSize (400, 300);
    startTimerHz (50);
}

PluginTemplateEditor::~PluginTemplateEditor()
{
}


//==============================================================================
void PluginTemplateEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    juce::String versionString = "";
#if JUCE_DEBUG
    versionString = "DEBUG - ";
#endif
    versionString.append (ProjectInfo::versionString, 30);

    g.setFont (12);
    g.setColour (juce::Colours::white);
    g.drawText (versionString, getLocalBounds(), juce::Justification::bottomRight);
}

void PluginTemplateEditor::resized()
{
    auto bounds = getLocalBounds();

    for (auto& s : slider)
        s.setBounds (bounds.removeFromLeft (20));

}


void PluginTemplateEditor::timerCallback()
{
    for (size_t i = 0; i < 5; ++i)
        slider[i].setValue (processorReference.rmsValues[i].load (std::memory_order_relaxed));
}
