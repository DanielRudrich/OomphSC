

#pragma once

#include "EditableText.hpp"
#include "Fonts.hpp"
#include <JuceHeader.h>

class IpAndPortComponent : public juce::Component
{
    static constexpr int portWidth = 50;

public:
    static constexpr int spacing = 7;

    IpAndPortComponent() :
        ip ("IP/HOST", juce::Colour (0xFFFF7E58)), port ("PORT", juce::Colour (0xFFFF7E58))
    {
        addAndMakeVisible (ip);

        port.setInputRestrictions (5, "0123456789");
        addAndMakeVisible (port);

        ip.onValueChanged = [&]()
        {
            if (itemsChanged)
                itemsChanged();
        };
        port.onValueChanged = [&]()
        {
            if (itemsChanged)
                itemsChanged();
        };
    }

    ~IpAndPortComponent() override = default;

    void resized() override
    {
        auto bounds = getLocalBounds();

        port.setBounds (bounds.removeFromRight (portWidth));
        bounds.removeFromRight (spacing);
        ip.setBounds (bounds);
    }

    void setIP (const juce::String& ipToSet) { ip.setText (ipToSet); }
    juce::String getIP() const { return ip.getText(); }

    void setPort (int portToSet) { port.setText (portToSet == -1 ? "" : juce::String (portToSet)); }
    int getPort() const { return port.getText().getIntValue(); }

    void setTextColour (const juce::Colour& colourToSet)
    {
        ip.setTextColour (colourToSet);
        port.setTextColour (colourToSet);
    }

    std::function<void (void)> itemsChanged;

private:
    EditableText ip;
    EditableText port;
};
