#include "PluginEditor.hpp"

//==============================================================================
PluginTemplateEditor::PluginTemplateEditor (PluginTemplateProcessor& p) :
    AudioProcessorEditor (&p),
    laf (juce::LookAndFeel_V4::getLightColourScheme()),
    processorReference (p),
    oscComponent (processorReference.getOSCSender())
{
    using namespace juce;

    auto& params = processorReference.getAPVTS();

    std::array<juce::Colour, 5> colours = { juce::Colours::cornflowerblue,
                                            juce::Colours::limegreen,
                                            juce::Colours::orange,
                                            juce::Colours::orangered,
                                            juce::Colours::black };

    attack.setSliderStyle (Slider::LinearBar);
    attackAttachment =
        std::make_unique<SliderAttachment> (params, Settings::Parameters::Attack::id, attack);
    attack.setTextValueSuffix (" ms Attack");
    addAndMakeVisible (attack);

    release.setSliderStyle (Slider::LinearBar);
    releaseAttachment =
        std::make_unique<SliderAttachment> (params, Settings::Parameters::Release::id, release);
    release.setTextValueSuffix (" ms Release");
    addAndMakeVisible (release);

    auto ids = juce::StringArray (Settings::Parameters::CrossOver1::id,
                                  Settings::Parameters::CrossOver2::id,
                                  Settings::Parameters::CrossOver3::id);

    for (size_t i = 0; i < Settings::numCrossOvers; ++i)
    {
        auto& s = crossOverFrequencies[i];
        s.setSliderStyle (Slider::LinearBar);
        s.setTextValueSuffix (" Hz");
        addAndMakeVisible (s);

        crossOverFrequenciesAttachments[i] =
            std::make_unique<SliderAttachment> (params, ids[(int) i], s);
    }

    for (size_t i = 0; i < Settings::numRMS; ++i)
    {
        auto& s = slider[i];
        s.setColour (Slider::ColourIds::trackColourId, colours[i]);
        s.setSliderStyle (Slider::LinearBarVertical);
        s.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        s.setRange (0.0, 1.0);
        addAndMakeVisible (s);
    }

    addAndMakeVisible (oscComponent);

    setSize (400, 300);
    setLookAndFeel (&laf);
    startTimerHz (50);
}

PluginTemplateEditor::~PluginTemplateEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void PluginTemplateEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);

    juce::String versionString = "";
#if JUCE_DEBUG
    versionString = "DEBUG - ";
#endif
    versionString.append (ProjectInfo::versionString, 30);

    g.setColour (juce::Colours::black);
    g.setFont (30);
    g.drawText ("OomphSC", getLocalBounds().removeFromTop (50), juce::Justification::centred);

    g.setFont (12);
    g.drawText (versionString, getLocalBounds(), juce::Justification::bottomRight);
}

void PluginTemplateEditor::resized()
{
    auto bounds = getLocalBounds().reduced (12);

    bounds.removeFromTop (50);
    oscComponent.setBounds (bounds.removeFromTop (40));
    bounds.removeFromTop (20);

    const auto sliderWidth = bounds.getWidth() / 5;

    auto row = bounds.removeFromTop (20);
    row.removeFromLeft (sliderWidth / 2);
    attack.setBounds (row.removeFromLeft (2 * sliderWidth));
    release.setBounds (row.removeFromLeft (2 * sliderWidth));

    row = bounds.removeFromTop (20);
    row.removeFromLeft (sliderWidth / 2);
    for (auto& s : crossOverFrequencies)
        s.setBounds (row.removeFromLeft (sliderWidth));

    for (auto& s : slider)
        s.setBounds (bounds.removeFromLeft (sliderWidth));
}

void PluginTemplateEditor::timerCallback()
{
    auto& sender = processorReference.getOSCSender();

    std::array<float, 5> values;
    for (size_t i = 0; i < 5; ++i)
    {
        const auto value = processorReference.rmsValues[i].load (std::memory_order_relaxed);
        values[i] = value;
        slider[i].setValue (value);
    }

    if (sender.isConnected())
    {
        sender.send ({ "/rms/full/", values[4] });
        for (int i = 0; i < Settings::numBands; ++i)
            sender.send ({ "/rms/band/" + juce::String (i) + "/", values[(size_t) i] });
    }
}
