
#pragma once

#include <BinaryFonts.hpp>
#include <JuceHeader.h>

struct Fonts
{
    static juce::Typeface::Ptr getRegularFont()
    {
        // I know... static variable in a plug-in... let's see if it blows up
        static auto font =
            juce::Typeface::createSystemTypefaceFor (BinaryFonts::MontserratRegular_ttf,
                                                     BinaryFonts::MontserratRegular_ttfSize);
        return font;
    }

    static juce::Font getRegularFont (float fontHeight)
    {
        juce::Font font (getRegularFont());
        font.setHeight (fontHeight);
        return font;
    }
};
