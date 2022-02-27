
#pragma once

namespace Settings
{

static constexpr int numBands = 4;
static constexpr int numCrossOvers = numBands - 1;
static constexpr int numRMS = numBands + 1;

struct Parameters
{

    struct CrossOver1
    {
        static inline juce::String id = "crossOver1";
        static inline juce::String name = "Crossover Frequency #1";
        static inline juce::String unit = "Hz";
        static constexpr float min = 20.0f;
        static constexpr float max = 20000.0f;
        static constexpr float defaultValue = 200.0f;
        static constexpr float skew = 0.4f;
    };

    struct CrossOver2
    {
        static inline juce::String id = "crossOver2";
        static inline juce::String name = "Crossover Frequency #2";
        static inline juce::String unit = "Hz";
        static constexpr float min = 20.0f;
        static constexpr float max = 20000.0f;
        static constexpr float defaultValue = 1000.0f;
        static constexpr float skew = 0.4f;
    };

    struct CrossOver3
    {
        static inline juce::String id = "crossOver3";
        static inline juce::String name = "Crossover Frequency #3";
        static inline juce::String unit = "Hz";
        static constexpr float min = 20.0f;
        static constexpr float max = 20000.0f;
        static constexpr float defaultValue = 5000.0f;
        static constexpr float skew = 0.4f;
    };

    struct Attack
    {
        static inline juce::String id = "attack";
        static inline juce::String name = "Attack time in ms";
        static inline juce::String unit = "ms";
        static constexpr float min = 0.0f;
        static constexpr float max = 200.0f;
        static constexpr float defaultValue = 5.0f;
    };

    struct Release
    {
        static inline juce::String id = "release";
        static inline juce::String name = "Attack time in ms";
        static inline juce::String unit = "ms";
        static constexpr float min = 0.0f;
        static constexpr float max = 1000.0f;
        static constexpr float defaultValue = 50.0f;
    };

    struct LevelCalculationType
    {
        static inline juce::String id = "levelCalculation";
        static inline juce::String name = "Level Calculation Type";
        static inline juce::StringArray choices = {"Peak", "RMS"};
        static constexpr int defaultValue = 0;
    };



};
}
