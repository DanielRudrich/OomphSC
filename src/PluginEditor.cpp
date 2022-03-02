#include "PluginEditor.hpp"

//==============================================================================
OomphSCEditor::OomphSCEditor (OomphSCProcessor& p) :
    AudioProcessorEditor (&p),
    laf (juce::LookAndFeel_V4::getLightColourScheme()),
    processorReference (p),
    oscComponent (processorReference.getOSCSender()),
    visualizer (processorReference.getAPVTS())
{
    using namespace juce;

    auto& params = processorReference.getAPVTS();

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

    addAndMakeVisible (oscComponent);

    addAndMakeVisible (visualizer);

    setSize (400, 300);
    setLookAndFeel (&laf);
    startTimerHz (50);
}

OomphSCEditor::~OomphSCEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void OomphSCEditor::paint (juce::Graphics& g)
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

void OomphSCEditor::resized()
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

    visualizer.setBounds (bounds);
}

void OomphSCEditor::timerCallback()
{
    std::array<float, 5> values;

    for (size_t i = 0; i < 5; ++i)
    {
        const auto value = processorReference.rmsValues[i].load (std::memory_order_relaxed);
        values[i] = value;
    }

    visualizer.setValues (values);
}
