
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "ConnectionButton.hpp"
#include "IpAndPortComponent.hpp"
#include "OSCSenderPlus.hpp"

class OSCComponent : public juce::Component, public juce::Timer
{
public:
    OSCComponent (OSCSenderPlus& o) : oscSender (o)
    {
        ipAndPort.itemsChanged = [&]() { disconnect(); };
        ipAndPort.setIP (o.getHostName());
        ipAndPort.setPort (o.getPortNumber());
        addAndMakeVisible (ipAndPort);

        connectionButton.onClick = [&]() { toggleConnection(); };
        addAndMakeVisible (connectionButton);

        updateConnectionStatus();

        startTimer (500);
    }

    ~OSCComponent() override = default;

    void timerCallback() override
    {
        // update OSC status
        if (oscSender.isConnected() && oscSender.hasFailedSendingMessages())
            ipAndPort.setTextColour (juce::Colours::red);
        else
            ipAndPort.setTextColour (juce::Colours::black);
    }

    void toggleConnection()
    {
        if (oscSender.isConnected())
            disconnect();
        else
            connect();
    }

    void connect() noexcept
    {
        oscSender.connect (ipAndPort.getIP(), ipAndPort.getPort());
        updateConnectionStatus();
    }

    void disconnect() noexcept
    {
        oscSender.disconnect();
        updateConnectionStatus();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        connectionButton.setBounds (
            bounds.removeFromRight (30).removeFromTop (bounds.getHeight() / 2));
        bounds.removeFromRight (IpAndPortComponent::spacing);
        ipAndPort.setBounds (bounds);
    }

private:
    void updateConnectionStatus() { connectionButton.setConnected (oscSender.isConnected()); }

    OSCSenderPlus& oscSender;

    IpAndPortComponent ipAndPort;
    ConnectionButton connectionButton;
};
