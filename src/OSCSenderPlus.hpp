

#pragma once

#include <JuceHeader.h>
#include <atomic>

class OSCSenderPlus : public juce::OSCSender
{
public:
    OSCSenderPlus()
    {
        connected.store (false, std::memory_order_relaxed);
    }

    bool connect (const juce::String& targetHostName, int portNumber)
    {
        hostName = targetHostName;
        port = portNumber;

        if (portNumber == -1 || targetHostName.isEmpty())
        {
            disconnect();
            connected.store (false, std::memory_order_relaxed);
            return true;
        }

        if (juce::OSCSender::connect (targetHostName, port))
        {
            connected.store (true, std::memory_order_relaxed);
            return true;
        }
        else
            return false;
    }

    bool disconnect()
    {
        if (OSCSender::disconnect())
        {
            connected.store (false, std::memory_order_relaxed);
            return true;
        }
        else
            return false;
    }

    int getPortNumber() const noexcept
    {
        return port;
    }

    juce::String getHostName() const noexcept
    {
        return hostName;
    }

    bool isConnected() const noexcept
    {
        return connected.load (std::memory_order_relaxed);
    }


private:
    juce::String hostName;
    int port = -1;
    std::atomic<bool> connected;
};

