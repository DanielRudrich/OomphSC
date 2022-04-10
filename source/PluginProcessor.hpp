#pragma once

#include <JuceHeader.h>
#include <array>

#include "MonoAnalyzer.hpp"
#include "OSCSenderPlus.hpp"

//==============================================================================
/**
*/

class OomphSCProcessor : public juce::AudioProcessor,
                         public juce::AudioProcessorValueTreeState::Listener,
                         public juce::Timer
{
public:
    //==============================================================================
    OomphSCProcessor();
    ~OomphSCProcessor() override;

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

    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void timerCallback() override;

    std::array<std::array<std::atomic<float>, Settings::numRMS>, 2> rmsValues;

    OSCSenderPlus& getOSCSender() { return oscSender; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return params; }

    void updateCrossovers();

    bool isInStereoMode() const { return inputMode->load (std::memory_order_relaxed) > 0.0f; }

private:
    juce::AudioProcessorValueTreeState params;
    std::array<std::atomic<float>*, Settings::numCrossOvers> crossOver;

    std::atomic<float>* inputMode;

    OSCSenderPlus oscSender;

    juce::AudioBuffer<float> copyBuffer;
    std::array<MonoAnalyzer, 2> analyzers;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OomphSCProcessor)
};
