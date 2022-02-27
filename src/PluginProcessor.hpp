#pragma once

#include <JuceHeader.h>
#include <array>

#include "Settings.hpp"
#include "OSCSenderPlus.hpp"

//==============================================================================
/**
*/
class PluginTemplateProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    PluginTemplateProcessor();
    ~PluginTemplateProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged (const juce::String &parameterID, float newValue) override;

    std::array<std::atomic<float>, Settings::numRMS> rmsValues;

    OSCSenderPlus& getOSCSender() { return oscSender; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return params; }
    
private:
    juce::AudioProcessorValueTreeState params;
    OSCSenderPlus oscSender;


    std::array<juce::dsp::BallisticsFilter<float>, Settings::numRMS> rms;

    using CrossOver = juce::dsp::LinkwitzRileyFilter<float>;
    std::array<CrossOver, Settings::numCrossOvers> crossOvers;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginTemplateProcessor)
};
