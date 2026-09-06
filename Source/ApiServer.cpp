#include "ApiServer.h"
#include <httplib.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace
{
    juce::var noteToVar(const SequencerNote& n)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", n.id);
        obj->setProperty("pitch", n.pitch);
        obj->setProperty("velocity", (double) n.velocity);
        obj->setProperty("startBeat", n.startBeat);
        obj->setProperty("lengthBeats", n.lengthBeats);
        return juce::var(obj);
    }

    void sendJson(httplib::Response& res, const juce::var& v, int status = 200)
    {
        res.status = status;
        res.set_content(juce::JSON::toString(v).toStdString(), "application/json");
    }

    void callOnMessageThreadSync(const std::function<void()>& fn)
    {
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            fn();
            return;
        }

        juce::WaitableEvent done;
        juce::MessageManager::callAsync([&fn, &done]
        {
            fn();
            done.signal();
        });
        done.wait();
    }
}

ApiServer::ApiServer(PluginHost& host, Sequencer& seq)
    : pluginHost(host), sequencer(seq)
{
}

ApiServer::~ApiServer()
{
    stop();
}

bool ApiServer::start(int portToUse)
{
    if (server != nullptr)
        return false;

    server = std::make_unique<httplib::Server>();
    port = portToUse;

    server->Get("/api/status", [this](const httplib::Request&, httplib::Response& res)
    {
        auto* obj = new juce::DynamicObject();

        auto* pluginObj = new juce::DynamicObject();
        pluginObj->setProperty("loaded", pluginHost.isPluginLoaded());
        pluginObj->setProperty("name", pluginHost.getPluginName());
        obj->setProperty("plugin", juce::var(pluginObj));

        auto* transportObj = new juce::DynamicObject();
        transportObj->setProperty("playing", sequencer.isPlaying());
        transportObj->setProperty("bpm", sequencer.getBpm());
        transportObj->setProperty("positionBeats", sequencer.getPositionBeats());
        obj->setProperty("transport", juce::var(transportObj));

        sendJson(res, juce::var(obj));
    });

    server->Post("/api/plugin/load", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto path = parsed.getProperty("path", juce::var()).toString();
        if (path.isEmpty())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("ok", false);
            obj->setProperty("error", "missing 'path'");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        juce::String errorMessage;
        callOnMessageThreadSync([this, path, &errorMessage]
        {
            pluginHost.loadPlugin(juce::File(path), [&errorMessage](juce::String err) { errorMessage = err; });
        });

        auto* obj = new juce::DynamicObject();
        if (errorMessage.isNotEmpty())
        {
            obj->setProperty("ok", false);
            obj->setProperty("error", errorMessage);
            sendJson(res, juce::var(obj), 500);
        }
        else
        {
            obj->setProperty("ok", true);
            obj->setProperty("name", pluginHost.getPluginName());
            sendJson(res, juce::var(obj));
        }
    });

    server->Get("/api/notes", [this](const httplib::Request&, httplib::Response& res)
    {
        juce::Array<juce::var> arr;
        for (const auto& n : sequencer.getNotes())
            arr.add(noteToVar(n));
        sendJson(res, juce::var(arr));
    });

    server->Post("/api/notes", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        if (!parsed.isObject())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("error", "invalid json body");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const int pitch = (int) parsed.getProperty("pitch", 60);
        const float velocity = (float) (double) parsed.getProperty("velocity", 0.8);
        const double startBeat = (double) parsed.getProperty("startBeat", 0.0);
        const double lengthBeats = (double) parsed.getProperty("lengthBeats", 1.0);

        const int id = sequencer.addNote(pitch, velocity, startBeat, lengthBeats);

        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", id);
        sendJson(res, juce::var(obj), 201);
    });

    server->Delete(R"(/api/notes/(\d+))", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int id = std::stoi(req.matches[1].str());
        const bool removed = sequencer.removeNote(id);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", removed);
        sendJson(res, juce::var(obj), removed ? 200 : 404);
    });

    server->Post("/api/notes/clear", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.clearNotes();
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        sendJson(res, juce::var(obj));
    });

    server->Post("/api/transport/play", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.play();
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        sendJson(res, juce::var(obj));
    });

    server->Post("/api/transport/stop", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.stop();
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        sendJson(res, juce::var(obj));
    });

    server->Post("/api/transport/bpm", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        const double bpm = (double) parsed.getProperty("bpm", 120.0);
        sequencer.setBpm(bpm);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        obj->setProperty("bpm", sequencer.getBpm());
        sendJson(res, juce::var(obj));
    });

    serverThread = std::thread([this]
    {
        server->listen("127.0.0.1", port);
    });

    return true;
}

void ApiServer::stop()
{
    if (server == nullptr)
        return;

    server->stop();
    if (serverThread.joinable())
        serverThread.join();
    server.reset();
}

bool ApiServer::isRunning() const
{
    return server != nullptr;
}
