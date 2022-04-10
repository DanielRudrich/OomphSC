#include "PluginEditor.hpp"
#include <BinaryLogo.hpp>

//==============================================================================
OomphSCEditor::OomphSCEditor (OomphSCProcessor& p) :
    AudioProcessorEditor (&p),
    processorReference (p),
    oscComponent (processorReference.getOSCSender()),
    monoStereoButton (
        *processorReference.getAPVTS().getParameter (Settings::Parameters::InputMode::id)),
    attack (*processorReference.getAPVTS().getParameter (Settings::Parameters::Attack::id),
            juce::String ("ATTACK"),
            juce::Colour (0xFF00C9FF)),
    release (*processorReference.getAPVTS().getParameter (Settings::Parameters::Release::id),
             juce::String ("RELEASE"),
             juce::Colour (0xFF50E3C2)),
    visualizer (processorReference.getAPVTS())
{
    using namespace juce;

    // logo
    logo = juce::Drawable::createFromImageData (BinaryLogo::logo_svg, BinaryLogo::logo_svgSize);

    // controls
    addAndMakeVisible (monoStereoButton);
    addAndMakeVisible (attack);
    addAndMakeVisible (release);

    auto ids = juce::StringArray (Settings::Parameters::CrossOver1::id,
                                  Settings::Parameters::CrossOver2::id,
                                  Settings::Parameters::CrossOver3::id);

    addAndMakeVisible (oscComponent);
    addAndMakeVisible (visualizer);

    setSize (450, 300);
    startTimerHz (50);
}

OomphSCEditor::~OomphSCEditor() = default;

//==============================================================================
void OomphSCEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);

    logo->drawWithin (g,
                      getLocalBounds().removeFromTop (74).toFloat(),
                      juce::RectanglePlacement::centred | juce::RectanglePlacement::doNotResize,
                      1.0f);

    juce::String versionString = "";
#if JUCE_DEBUG
    versionString = "DEBUG - ";
#endif
    versionString.append (ProjectInfo::versionString, 30);

    g.setColour (juce::Colours::black);
    g.setFont (Fonts::getRegularFont());
    g.setFont (12);
    g.drawText (versionString, getLocalBounds(), juce::Justification::bottomRight);
}

void OomphSCEditor::resized()
{
    constexpr auto spacing = 10;
    constexpr auto sliderWidth = 70;

    auto bounds = getLocalBounds();
    bounds.reduce (10, 10);

    bounds.removeFromTop (50);

    visualizer.setBounds (bounds.removeFromBottom (160));

    bounds.removeFromBottom (2 * 7);
    auto row = bounds.removeFromBottom (42);
    monoStereoButton.setBounds (row.removeFromLeft (sliderWidth));
    row.removeFromLeft (spacing);
    attack.setBounds (row.removeFromLeft (sliderWidth));
    row.removeFromLeft (spacing);
    release.setBounds (row.removeFromLeft (sliderWidth));
    row.removeFromLeft (spacing);
    oscComponent.setBounds (row);

    logo->setBounds (bounds);
}

void OomphSCEditor::timerCallback()
{
    if (processorReference.isInStereoMode())
    {
        std::array<float, 5> valuesLeft;
        std::array<float, 5> valuesRight;
        for (size_t i = 0; i < 5; ++i)
        {
            valuesLeft[i] = processorReference.rmsValues[0][i].load (std::memory_order_relaxed);
            valuesRight[i] = processorReference.rmsValues[1][i].load (std::memory_order_relaxed);
        }

        visualizer.setValues (valuesLeft, valuesRight);
    }
    else
    {
        std::array<float, 5> values;
        for (size_t i = 0; i < 5; ++i)
            values[i] = processorReference.rmsValues[0][i].load (std::memory_order_relaxed);

        visualizer.setValues (values);
    }
}
