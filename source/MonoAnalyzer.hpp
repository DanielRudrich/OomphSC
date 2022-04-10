
#pragma once

#include "Settings.hpp"
#include <JuceHeader.h>

class MonoAnalyzer
{
public:
    MonoAnalyzer() = default;

    void prepare (const juce::dsp::ProcessSpec& specs)
    {
        for (auto& e : rms)
            e.prepare (specs);

        for (auto& e : crossOvers)
            e.prepare (specs);
    }

    void setCutoffFrequency (size_t band, float frequency)
    {
        crossOvers[band].setCutoffFrequency (frequency);
    }

    void setAttackTime (float attackTimeInMs)
    {
        for (auto& e : rms)
            e.setAttackTime (attackTimeInMs);
    }

    void setReleaseTime (float releaseTimeInMs)
    {
        for (auto& e : rms)
            e.setReleaseTime (releaseTimeInMs);
    }

    void process (const float* data,
                  int numSamples,
                  std::array<std::atomic<float>, Settings::numRMS>& rmsValues)
    {
        std::array<float, Settings::numRMS> rmsTemp;
        for (size_t i = 0; i < Settings::numRMS; ++i)
            rmsTemp[i] = rmsValues[i].load (std::memory_order_relaxed);

        // RMS fullband
        for (int i = 0; i < numSamples; ++i)
            rmsTemp[Settings::numBands] = rms[Settings::numBands].processSample (0, data[i]);

        // RMS per band
        for (int i = 0; i < numSamples; ++i)
        {
            auto x = data[i];
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

    void reset()
    {
        for (auto& e : rms)
            e.reset();

        for (auto& e : crossOvers)
            e.reset();
    }

private:
    std::array<juce::dsp::BallisticsFilter<float>, Settings::numRMS> rms;
    using CrossOver = juce::dsp::LinkwitzRileyFilter<float>;
    std::array<CrossOver, Settings::numCrossOvers> crossOvers;
};
