
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "OSCSenderPlus.hpp"
#include "IpAndPortComponent.hpp"


class OSCComponent : public juce::Component
{

public:
    OSCComponent (OSCSenderPlus& o) : oscSender (o)
    {
        ipAndPort.setIP (o.getHostName());
        ipAndPort.setPort (o.getPortNumber());
        addAndMakeVisible (ipAndPort);

        connectButton.setButtonText ("connect");
        connectButton.onClick = [&] () { toggleConnection(); };
        addAndMakeVisible (connectButton);
    }

    ~OSCComponent() override = default;

    void toggleConnection()
    {
        if (oscSender.isConnected())
            oscSender.disconnect();
        else
            oscSender.connect (ipAndPort.getIP(), ipAndPort.getPort());

        ipAndPort.setState (oscSender.isConnected() ? IpAndPortComponent::State::connected : IpAndPortComponent::State::disconnected);
        connectButton.setButtonText (oscSender.isConnected() ? "disconnect" : "connect");
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (5);
        connectButton.setBounds (bounds.removeFromRight (80));
        bounds.removeFromRight (5);
        ipAndPort.setBounds (bounds);
    }


private:
    OSCSenderPlus& oscSender;

    IpAndPortComponent ipAndPort;
    juce::TextButton connectButton;
};

