#pragma once

#include "AudioEngine.h"
#include "Sequencer.h"
#include <memory>
#include <thread>

namespace httplib { class Server; }

// HTTP surface an AI (or any external process) can drive: create tracks,
// load a VST3 into one, read/write notes, and control the transport. Runs
// on its own thread; operations that touch a plugin are marshalled onto the
// JUCE message thread since plugin instantiation/UI expects to run there.
class ApiServer
{
public:
    ApiServer(AudioEngine& engineToUse, Sequencer& seq);
    ~ApiServer();

    bool start(int portToUse);
    void stop();
    bool isRunning() const;
    int getPort() const { return port; }

private:
    AudioEngine& engine;
    Sequencer& sequencer;
    std::unique_ptr<httplib::Server> server;
    std::thread serverThread;
    int port = 0;
};
