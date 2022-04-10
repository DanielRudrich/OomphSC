#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"
#include "Settings.hpp"
#include <algorithm>

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    using namespace Settings;

    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (
        std::make_unique<APF> (Parameters::CrossOver1::id,
                               Parameters::CrossOver1::name,
                               NormalisableRange<float> (Parameters::CrossOver1::min,
                                                         Parameters::CrossOver1::max,
                                                         1.0f,
                                                         Parameters::CrossOver1::skew),
                               Parameters::CrossOver1::defaultValue,
                               Parameters::CrossOver1::unit));

    params.push_back (
        std::make_unique<APF> (Parameters::CrossOver2::id,
                               Parameters::CrossOver2::name,
                               NormalisableRange<float> (Parameters::CrossOver2::min,
                                                         Parameters::CrossOver2::max,
                                                         1.0f,
                                                         Parameters::CrossOver2::skew),
                               Parameters::CrossOver2::defaultValue,
                               Parameters::CrossOver2::unit));

    params.push_back (
        std::make_unique<APF> (Parameters::CrossOver3::id,
                               Parameters::CrossOver3::name,
                               NormalisableRange<float> (Parameters::CrossOver3::min,
                                                         Parameters::CrossOver3::max,
                                                         1.0f,
                                                         Parameters::CrossOver3::skew),
                               Parameters::CrossOver3::defaultValue,
                               Parameters::CrossOver3::unit));

    params.push_back (std::make_unique<APF> (
        Parameters::Attack::id,
        Parameters::Attack::name,
        NormalisableRange<float> (Parameters::Attack::min, Parameters::Attack::max, 1.0f),
        Parameters::Attack::defaultValue,
        Parameters::Attack::unit));

    params.push_back (std::make_unique<APF> (
        Parameters::Release::id,
        Parameters::Release::name,
        NormalisableRange<float> (Parameters::Release::min, Parameters::Release::max, 1.0f),
        Parameters::Release::defaultValue,
        Parameters::Release::unit));

    params.push_back (std::make_unique<APC> (Parameters::InputMode::id,
                                             Parameters::InputMode::name,
                                             Parameters::InputMode::choices,
                                             Parameters::InputMode::defaultValue));

    return { params.begin(), params.end() };
}

//==============================================================================
OomphSCProcessor::OomphSCProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    :
    AudioProcessor (BusesProperties()
    #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
                        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                        ),
#endif
    params (*this, nullptr, "OomphSC", createParameterLayout())
{
    using namespace Settings;

    for (auto& a : analyzers)
    {
        a.setAttackTime (Settings::Parameters::Attack::defaultValue);
        a.setReleaseTime (Settings::Parameters::Release::defaultValue);
    }

    params.addParameterListener (Parameters::CrossOver1::id, this);
    params.addParameterListener (Parameters::CrossOver2::id, this);
    params.addParameterListener (Parameters::CrossOver3::id, this);
    params.addParameterListener (Parameters::Attack::id, this);
    params.addParameterListener (Parameters::Release::id, this);
    params.addParameterListener (Parameters::InputMode::id, this);

    crossOver[0] = params.getRawParameterValue (Parameters::CrossOver1::id);
    crossOver[1] = params.getRawParameterValue (Parameters::CrossOver2::id);
    crossOver[2] = params.getRawParameterValue (Parameters::CrossOver3::id);
    inputMode = params.getRawParameterValue (Parameters::InputMode::id);

    for (auto& ch : rmsValues)
        for (auto& e : ch)
            e.store (0.0f, std::memory_order_relaxed);

    updateCrossovers();
    startTimer (50);
}

OomphSCProcessor::~OomphSCProcessor()
{
}

//==============================================================================
const juce::String OomphSCProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OomphSCProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool OomphSCProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool OomphSCProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double OomphSCProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OomphSCProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int OomphSCProcessor::getCurrentProgram()
{
    return 0;
}

void OomphSCProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String OomphSCProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void OomphSCProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index);
    juce::ignoreUnused (newName);
}

//==============================================================================
void OomphSCProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec specs { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 1 };

    copyBuffer.setSize (1, samplesPerBlock);

    for (auto& a : analyzers)
        a.prepare (specs);
}

void OomphSCProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OomphSCProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    juce::ignoreUnused (layouts);
    if (layouts.getMainInputChannels() <= 2)
        return true;

    return false;
}
#endif

void OomphSCProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = std::min (buffer.getNumChannels(), 2);

    if (isInStereoMode() && numChannels == 2)
    {
        for (size_t ch = 0; ch < static_cast<size_t> (numChannels); ++ch)
            analyzers[ch].process (buffer.getReadPointer ((int) ch), numSamples, rmsValues[ch]);
    }
    else // sum to mono
    {
        copyBuffer.copyFrom (0, 0, buffer.getReadPointer (0), numSamples);
        copyBuffer.addFrom (0, 0, buffer.getReadPointer (1), numSamples);

        analyzers[0].process (copyBuffer.getReadPointer (0), numSamples, rmsValues[0]);
    }
}

//==============================================================================
bool OomphSCProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OomphSCProcessor::createEditor()
{
    return new OomphSCEditor (*this);
}

//==============================================================================
void OomphSCProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = params.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    xml->setAttribute (Settings::OSC::OSCHostname, oscSender.getHostName());
    xml->setAttribute (Settings::OSC::OSCPort, oscSender.getPortNumber());
    copyXmlToBinary (*xml, destData);
}

void OomphSCProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (params.state.getType()))
        {
            auto hostname = xmlState->getStringAttribute (Settings::OSC::OSCHostname);
            auto port = xmlState->getIntAttribute (Settings::OSC::OSCPort, -1);

            oscSender.connect (hostname, port);

            xmlState->removeAttribute (Settings::OSC::OSCHostname);
            xmlState->removeAttribute (Settings::OSC::OSCPort);

            params.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
}

void OomphSCProcessor::updateCrossovers()
{
    std::array<float, Settings::numCrossOvers> frequencies;
    for (size_t i = 0; i < Settings::numCrossOvers; ++i)
        frequencies[i] = crossOver[i]->load (std::memory_order_relaxed);

    std::sort (frequencies.begin(), frequencies.end());

    for (auto& a : analyzers)
        for (size_t i = 0; i < Settings::numCrossOvers; ++i)
            a.setCutoffFrequency (i, frequencies[i]);
}

void OomphSCProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    using namespace juce::dsp;
    using namespace Settings;

    if (parameterID == Parameters::CrossOver1::id || parameterID == Parameters::CrossOver2::id
        || parameterID == Parameters::CrossOver3::id)
        updateCrossovers();

    else if (parameterID == Parameters::Attack::id)
        for (auto& a : analyzers)
            a.setAttackTime (newValue);

    else if (parameterID == Parameters::Release::id)
        for (auto& a : analyzers)
            a.setReleaseTime (newValue);
}

void OomphSCProcessor::timerCallback()
{
    if (oscSender.isConnected())
    {
        const auto stereoMode = isInStereoMode();

        auto fullMessage = juce::OSCMessage ("/rms/full/");
        fullMessage.addFloat32 (rmsValues[0][4].load (std::memory_order_relaxed));
        if (stereoMode)
            fullMessage.addFloat32 (rmsValues[1][4].load (std::memory_order_relaxed));

        oscSender.send (fullMessage);

        for (size_t i = 0; i < Settings::numBands; ++i)
        {
            auto message = juce::OSCMessage ("/rms/band/" + juce::String (i) + "/");
            message.addFloat32 (rmsValues[0][i].load (std::memory_order_relaxed));
            if (stereoMode)
                fullMessage.addFloat32 (rmsValues[1][i].load (std::memory_order_relaxed));

            oscSender.send (message);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OomphSCProcessor();
}
