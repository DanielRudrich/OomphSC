

#pragma once

#include <JuceHeader.h>
#include <atomic>

class OSCSenderPlus
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
        if (sender.connect (targetHostName, port))
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
        if (sender.disconnect())
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

        sendingFailed = false;
    }

    bool send (const juce::OSCMessage& message)
    {
        auto success = sender.send (message);
        sendingFailed = ! success;
        return success;
    }

    bool send (const juce::OSCBundle& bundle)
    {
        auto success = sender.send (bundle);
        sendingFailed = ! success;
        return success;
    }

    int getPortNumber() const noexcept { return port; }
    juce::String getHostName() const noexcept { return hostName; }
    bool isConnected() const noexcept { return connected.load (std::memory_order_relaxed); }
    bool hasFailedSendingMessages() const noexcept { return sendingFailed; }

private:
    juce::OSCSender sender;
    bool sendingFailed = false;

    juce::String hostName;
    int port = -1;
    std::atomic<bool> connected;
};
