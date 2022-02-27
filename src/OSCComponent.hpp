
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
        ipAndPort.onReturnKey = [&]() { connect(); };
        ipAndPort.setIP (o.getHostName());
        ipAndPort.setPort (o.getPortNumber());
        addAndMakeVisible (ipAndPort);

        updateButtonText();
        connectButton.onClick = [&] () { toggleConnection(); };
        addAndMakeVisible (connectButton);
    }

    ~OSCComponent() override = default;

    void toggleConnection()
    {
        if (oscSender.isConnected())
        {
            oscSender.disconnect();
            ipAndPort.setState (IpAndPortComponent::State::disconnected);
        }
        else
            connect();

        ipAndPort.setState (oscSender.isConnected() ? IpAndPortComponent::State::connected : IpAndPortComponent::State::disconnected);
        updateButtonText();
    }

    void connect() noexcept
    {
        if (oscSender.connect (ipAndPort.getIP(), ipAndPort.getPort()))
            ipAndPort.setState (IpAndPortComponent::State::connected);
        else
            ipAndPort.setState (IpAndPortComponent::State::error);

        updateButtonText();
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (5);
        connectButton.setBounds (bounds.removeFromRight (80));
        bounds.removeFromRight (5);
        ipAndPort.setBounds (bounds);
    }


private:
    void updateButtonText()
    {
        connectButton.setButtonText (oscSender.isConnected() ? "disconnect" : "connect");
    }

    OSCSenderPlus& oscSender;

    IpAndPortComponent ipAndPort;
    juce::TextButton connectButton;
};

