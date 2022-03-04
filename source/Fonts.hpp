
#pragma once

#include <BinaryData.h>
#include <JuceHeader.h>

struct Fonts
{
    static juce::Typeface::Ptr getRegularFont()
    {
        // I know... static variable in a plug-in... let's see if it blows up
        static auto font =
            juce::Typeface::createSystemTypefaceFor (BinaryData::MontserratRegular_ttf,
                                                     BinaryData::MontserratRegular_ttfSize);
        return font;
    }
};
