
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "IpAndPortComponent.hpp"
#include "OSCSenderPlus.hpp"
#include "ConnectionButton.hpp"

class OSCComponent : public juce::Component
{
public:
    OSCComponent (OSCSenderPlus& o) : oscSender (o)
    {
        ipAndPort.onReturnKey = [&]() { connect(); };
        ipAndPort.setIP (o.getHostName());
        ipAndPort.setPort (o.getPortNumber());
        addAndMakeVisible (ipAndPort);

        addAndMakeVisible (connectionButton);

        updateButtonText();
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

        ipAndPort.setState (oscSender.isConnected() ? IpAndPortComponent::State::connected
                                                    : IpAndPortComponent::State::disconnected);
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
        auto bounds = getLocalBounds();
        connectionButton.setBounds (bounds.removeFromRight (30).removeFromTop (bounds.getHeight() / 2));
        bounds.removeFromRight (IpAndPortComponent::spacing);
        ipAndPort.setBounds (bounds);
    }

private:
    void updateButtonText()
    {
        if (oscSender.isConnected())
            DBG ("connected");
        else
            DBG ("not connected");
    }

    OSCSenderPlus& oscSender;

    IpAndPortComponent ipAndPort;
    ConnectionButton connectionButton;
};
