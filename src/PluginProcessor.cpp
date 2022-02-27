#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

#include "Settings.hpp"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    using namespace Settings;

    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<APF> (Parameters::CrossOver1::id,
                                             Parameters::CrossOver1::name,
                                             NormalisableRange<float> (Parameters::CrossOver1::min,
                                                                       Parameters::CrossOver1::max, 1.0f,
                                                                       Parameters::CrossOver1::skew),
                                             Parameters::CrossOver1::defaultValue,
                                             Parameters::CrossOver1::unit));

    params.push_back (std::make_unique<APF> (Parameters::CrossOver2::id,
                                             Parameters::CrossOver2::name,
                                             NormalisableRange<float> (Parameters::CrossOver2::min,
                                                                       Parameters::CrossOver2::max, 1.0f,
                                                                       Parameters::CrossOver2::skew),
                                             Parameters::CrossOver2::defaultValue,
                                             Parameters::CrossOver2::unit));

    params.push_back (std::make_unique<APF> (Parameters::CrossOver3::id,
                                             Parameters::CrossOver3::name,
                                             NormalisableRange<float> (Parameters::CrossOver3::min,
                                                                       Parameters::CrossOver3::max, 1.0f,
                                                                       Parameters::CrossOver3::skew),
                                             Parameters::CrossOver3::defaultValue,
                                             Parameters::CrossOver3::unit));

    params.push_back (std::make_unique<APF> (Parameters::Attack::id,
                                             Parameters::Attack::name,
                                             NormalisableRange<float> (Parameters::Attack::min,
                                                                       Parameters::Attack::max, 1.0f),
                                             Parameters::Attack::defaultValue,
                                             Parameters::Attack::unit));


    params.push_back (std::make_unique<APF> (Parameters::Release::id,
                                             Parameters::Release::name,
                                             NormalisableRange<float> (Parameters::Release::min,
                                                                       Parameters::Release::max, 1.0f),
                                             Parameters::Release::defaultValue,
                                             Parameters::Release::unit));


    params.push_back (std::make_unique<APC> (Parameters::LevelCalculationType::id,
                                             Parameters::LevelCalculationType::name,
                                             Parameters::LevelCalculationType::choices,
                                             Parameters::LevelCalculationType::defaultValue));

    return { params.begin(), params.end() };
}

//==============================================================================
PluginTemplateProcessor::PluginTemplateProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    :
    AudioProcessor (BusesProperties()
    #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
                        .withInput ("Input", juce::AudioChannelSet::mono(), true)
        #endif
                        .withOutput ("Output", juce::AudioChannelSet::mono(), true)
    #endif
                        ),
#endif
    params (*this, nullptr, "RMSOSC", createParameterLayout())
{
    using namespace juce::dsp;
    using namespace Settings;

    for (auto& e : crossOvers)
        e.setCutoffFrequency (1'000.0f);

    for (auto& e : rms)
    {
        e.setAttackTime (5.0f);
        e.setReleaseTime (100.0f);
        e.setLevelCalculationType (BallisticsFilterLevelCalculationType::RMS);
    }

    params.addParameterListener (Parameters::CrossOver1::id, this);
    params.addParameterListener (Parameters::CrossOver2::id, this);
    params.addParameterListener (Parameters::CrossOver3::id, this);
    params.addParameterListener (Parameters::Attack::id, this);
    params.addParameterListener (Parameters::Release::id, this);
    params.addParameterListener (Parameters::LevelCalculationType::id, this);

    for (auto& e : rmsValues)
        e.store (0.0f, std::memory_order_relaxed);
}

PluginTemplateProcessor::~PluginTemplateProcessor()
{
}

//==============================================================================
const juce::String PluginTemplateProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginTemplateProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginTemplateProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginTemplateProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginTemplateProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginTemplateProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginTemplateProcessor::getCurrentProgram()
{
    return 0;
}

void PluginTemplateProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginTemplateProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginTemplateProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index);
    juce::ignoreUnused (newName);
}

//==============================================================================
void PluginTemplateProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec specs {sampleRate, static_cast<juce::uint32> (samplesPerBlock), 1};

    for (auto& e : rms)
        e.prepare (specs);

    for (auto& e : crossOvers)
        e.prepare (specs);
}

void PluginTemplateProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginTemplateProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    juce::ignoreUnused (layouts);
    return true;
}
#endif

void PluginTemplateProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    auto* chPtr = buffer.getReadPointer (0);

    std::array<float, Settings::numRMS> rmsTemp;
    for (size_t i = 0; i < Settings::numRMS; ++i)
        rmsTemp[i] = rmsValues[i].load (std::memory_order_relaxed);

    // RMS fullband
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        rmsTemp[Settings::numBands] = rms[Settings::numBands].processSample (0, chPtr[i]);

    // RMS per band
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto x = chPtr[i];
        std::array<float, Settings::numBands> bands;
        crossOvers[1].processSample (0, x, bands[0], bands[2]);
        crossOvers[0].processSample (0, bands[0], bands[0], bands[1]);
        crossOvers[2].processSample (0, bands[2], bands[2], bands[3]);

        for (size_t b = 0; b < Settings::numBands; ++b)
            rmsTemp[b] = rms[b].processSample (0, bands[b]);
    }

    for (size_t i = 0; i < Settings::numRMS; ++i)
        rmsValues[i].store (rmsTemp[i], std::memory_order_relaxed);
}

//==============================================================================
bool PluginTemplateProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginTemplateProcessor::createEditor()
{
    return new PluginTemplateEditor (*this);
}

//==============================================================================
void PluginTemplateProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = params.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    xml->setAttribute (Settings::OSC::OSCHostname, oscSender.getHostName());
    xml->setAttribute (Settings::OSC::OSCPort, oscSender.getPortNumber());
    copyXmlToBinary (*xml, destData);
}

void PluginTemplateProcessor::setStateInformation (const void* data, int sizeInBytes)
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

void PluginTemplateProcessor::parameterChanged (const juce::String &parameterID, float newValue)
{
    using namespace juce::dsp;
    using namespace Settings;

    if (parameterID == Parameters::CrossOver1::id)
        crossOvers[0].setCutoffFrequency (newValue);

    else if (parameterID == Parameters::CrossOver2::id)
        crossOvers[1].setCutoffFrequency (newValue);

    else if (parameterID == Parameters::CrossOver3::id)
        crossOvers[2].setCutoffFrequency (newValue);

    else if (parameterID == Parameters::Attack::id)
        for (auto& e : rms)
            e.setAttackTime (newValue);

    else if (parameterID == Parameters::Release::id)
        for (auto& e : rms)
            e.setReleaseTime (newValue);

    else if (parameterID == Parameters::LevelCalculationType::id)
        for (auto& e : rms)
            e.setLevelCalculationType (BallisticsFilterLevelCalculationType (newValue));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginTemplateProcessor();
}
