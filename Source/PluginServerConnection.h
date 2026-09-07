#pragma once

#include <juce_events/juce_events.h>

// Thin InterprocessConnection used identically by both the main app and
// AiTrackerPluginServer -- just forwards to std::function callbacks so each
// side can plug in its own message handling without a class hierarchy.
// Callbacks fire on the connection's own background thread, not the message
// thread.
class PluginServerConnection : public juce::InterprocessConnection
{
public:
    // false is required on the main-app side: PluginServerProxy blocks its
    // caller (the JUCE message thread, via ApiServer's synchronous-load
    // wrapper, or the real audio thread for renderNextBlock) waiting on a
    // WaitableEvent that this connection's own message handling signals --
    // if that handling were also dispatched through the message thread, a
    // blocked message thread could never process the very message it's
    // waiting on. The child process (PluginServerMain) uses the default
    // (true) since it creates GUI windows on message receipt and never
    // blocks its own message thread waiting on this connection.
    explicit PluginServerConnection(bool callbacksOnMessageThread = true)
        : InterprocessConnection(callbacksOnMessageThread)
    {
    }

    std::function<void(const juce::MemoryBlock&)> onMessage;
    std::function<void()> onDisconnect;

    void messageReceived(const juce::MemoryBlock& message) override
    {
        if (onMessage)
            onMessage(message);
    }

    void connectionMade() override {}

    void connectionLost() override
    {
        if (onDisconnect)
            onDisconnect();
    }
};
