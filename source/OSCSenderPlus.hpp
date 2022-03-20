

#pragma once

#include <JuceHeader.h>
#include <atomic>

class OSCSenderPlus : public juce::OSCSender
{
public:
    OSCSenderPlus() { connected.store (false, std::memory_order_relaxed); }

    bool connect (const juce::String& targetHostName, int portNumber)
    {
        hostName = targetHostName;
        port = portNumber;

        if (portNumber == -1 || targetHostName.isEmpty())
        {
            disconnect();
            return true;
        }

        disconnect();
        if (juce::OSCSender::connect (targetHostName, port))
        {
            connected.store (true, std::memory_order_relaxed);
            DBG ("OSC: connected.");
            return true;
        }
        else
        {
            DBG ("OSC: failed connection attempt.");
            return false;
        }
    }

    bool disconnect()
    {
        if (OSCSender::disconnect())
        {
            DBG ("OSC: disconnected.");
            connected.store (false, std::memory_order_relaxed);
            return true;
        }
        else
        {
            DBG ("OSC: failed to disconnet.");
            return false;
        }
    }

    int getPortNumber() const noexcept { return port; }

    juce::String getHostName() const noexcept { return hostName; }

    bool isConnected() const noexcept { return connected.load (std::memory_order_relaxed); }

private:
    juce::String hostName;
    int port = -1;
    std::atomic<bool> connected;
};
