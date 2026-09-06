#pragma once

#include "PluginHost.h"
#include "Sequencer.h"
#include <memory>
#include <thread>

namespace httplib { class Server; }

// HTTP surface an AI (or any external process) can drive: load a VST3,
// read/write notes, and control the transport. Runs on its own thread;
// operations that touch the plugin are marshalled onto the JUCE message
// thread since plugin instantiation/UI expects to run there.
class ApiServer
{
public:
    ApiServer(PluginHost& host, Sequencer& seq);
    ~ApiServer();

    bool start(int portToUse);
    void stop();
    bool isRunning() const;
    int getPort() const { return port; }

private:
    PluginHost& pluginHost;
    Sequencer& sequencer;
    std::unique_ptr<httplib::Server> server;
    std::thread serverThread;
    int port = 0;
};
