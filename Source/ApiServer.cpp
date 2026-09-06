#include "ApiServer.h"
#include <httplib.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <algorithm>

namespace
{
    juce::var noteToVar(const SequencerNote& n)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", n.id);
        obj->setProperty("trackId", n.trackId);
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

    void sendOk(httplib::Response& res, bool ok, int okStatus = 200, int failStatus = 404)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", ok);
        sendJson(res, juce::var(obj), ok ? okStatus : failStatus);
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

ApiServer::ApiServer(AudioEngine& engineToUse, Sequencer& seq)
    : engine(engineToUse), sequencer(seq)
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

        juce::Array<juce::var> trackArr;
        for (auto id : engine.getTrackIds())
        {
            auto* t = new juce::DynamicObject();
            t->setProperty("id", id);
            t->setProperty("name", engine.getTrackName(id));
            t->setProperty("pluginLoaded", engine.isPluginLoaded(id));
            t->setProperty("pluginName", engine.getPluginName(id));
            trackArr.add(juce::var(t));
        }
        obj->setProperty("tracks", trackArr);

        auto* transportObj = new juce::DynamicObject();
        transportObj->setProperty("playing", sequencer.isPlaying());
        transportObj->setProperty("bpm", sequencer.getBpm());
        transportObj->setProperty("positionBeats", sequencer.getPositionBeats());
        obj->setProperty("transport", juce::var(transportObj));

        sendJson(res, juce::var(obj));
    });

    server->Get("/api/tracks", [this](const httplib::Request&, httplib::Response& res)
    {
        juce::Array<juce::var> arr;
        for (auto id : engine.getTrackIds())
        {
            auto* t = new juce::DynamicObject();
            t->setProperty("id", id);
            t->setProperty("name", engine.getTrackName(id));
            t->setProperty("pluginLoaded", engine.isPluginLoaded(id));
            t->setProperty("pluginName", engine.getPluginName(id));
            arr.add(juce::var(t));
        }
        sendJson(res, juce::var(arr));
    });

    server->Post("/api/tracks", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto name = parsed.getProperty("name", juce::var()).toString();

        const int id = engine.addTrack(name);

        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", id);
        obj->setProperty("name", engine.getTrackName(id));
        sendJson(res, juce::var(obj), 201);
    });

    server->Delete(R"(/api/tracks/(\d+))", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int id = std::stoi(req.matches[1].str());
        sendOk(res, engine.removeTrack(id));
    });

    server->Post(R"(/api/tracks/(\d+)/plugin/load)", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int trackId = std::stoi(req.matches[1].str());
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
        callOnMessageThreadSync([this, trackId, path, &errorMessage]
        {
            engine.loadPlugin(trackId, juce::File(path), [&errorMessage](juce::String err) { errorMessage = err; });
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
            obj->setProperty("name", engine.getPluginName(trackId));
            sendJson(res, juce::var(obj));
        }
    });

    server->Post(R"(/api/tracks/(\d+)/editor)", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int trackId = std::stoi(req.matches[1].str());
        callOnMessageThreadSync([this, trackId] { engine.showEditorWindow(trackId); });
        sendOk(res, true);
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

        const int trackId = (int) parsed.getProperty("trackId", 0);
        const auto knownTracks = engine.getTrackIds();
        if (std::find(knownTracks.begin(), knownTracks.end(), trackId) == knownTracks.end())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("error", "unknown trackId");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const int pitch = (int) parsed.getProperty("pitch", 60);
        const float velocity = (float) (double) parsed.getProperty("velocity", 0.8);
        const double startBeat = (double) parsed.getProperty("startBeat", 0.0);
        const double lengthBeats = (double) parsed.getProperty("lengthBeats", 1.0);

        const int id = sequencer.addNote(trackId, pitch, velocity, startBeat, lengthBeats);

        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", id);
        sendJson(res, juce::var(obj), 201);
    });

    server->Delete(R"(/api/notes/(\d+))", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int id = std::stoi(req.matches[1].str());
        sendOk(res, sequencer.removeNote(id));
    });

    server->Post("/api/notes/clear", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.clearNotes();
        sendOk(res, true);
    });

    server->Post("/api/transport/play", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.play();
        sendOk(res, true);
    });

    server->Post("/api/transport/stop", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.stop();
        sendOk(res, true);
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

    server->Post("/api/transport/loop", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        const bool enabled = (bool) parsed.getProperty("enabled", true);
        const double startBeat = (double) parsed.getProperty("start", 0.0);
        const double endBeat = (double) parsed.getProperty("end", 4.0);
        sequencer.setLoop(enabled, startBeat, endBeat);
        sendOk(res, true);
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
