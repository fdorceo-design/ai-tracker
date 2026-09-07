#include "ApiServer.h"
#include <httplib.h>
#include <juce_core/juce_core.h>
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
            t->setProperty("midiChannel", engine.getMidiChannel(id));
            trackArr.add(juce::var(t));
        }
        obj->setProperty("tracks", trackArr);

        auto* transportObj = new juce::DynamicObject();
        transportObj->setProperty("playing", sequencer.isPlaying());
        transportObj->setProperty("bpm", sequencer.getBpm());
        transportObj->setProperty("beatsPerBar", sequencer.getBeatsPerBar());
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
            t->setProperty("midiChannel", engine.getMidiChannel(id));
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

    server->Post(R"(/api/tracks/(\d+)/name)", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto name = parsed.getProperty("name", "").toString().trim();
        if (name.isEmpty()) { sendOk(res, false, 200, 400); return; }
        sendOk(res, engine.setTrackName(std::stoi(req.matches[1].str()), name));
    });

    server->Post(R"(/api/tracks/(\d+)/midi-channel)", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        const int channel = (int) parsed.getProperty("channel", 1);
        const int trackId = std::stoi(req.matches[1].str());
        const bool ok = engine.setMidiChannel(trackId, channel);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", ok);
        if (ok) obj->setProperty("channel", engine.getMidiChannel(trackId));
        sendJson(res, juce::var(obj), ok ? 200 : 404);
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

        // Loading now spawns a child process and waits on it (out-of-process
        // hosting -- see PluginServerProxy), which no longer needs to run on
        // the JUCE message thread; keeping this on the HTTP thread avoids
        // blocking the GUI (and previously made the whole app appear to
        // hang / show "Not Responding" for the duration of a slow or
        // crashing load).
        juce::String errorMessage;
        engine.loadPlugin(trackId, juce::File(path), [&errorMessage](juce::String err) { errorMessage = err; });

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
        engine.showEditorWindow(trackId);
        sendOk(res, true);
    });

    server->Get("/api/midi-outputs", [this](const httplib::Request&, httplib::Response& res)
    {
        juce::Array<juce::var> arr;
        for (const auto& name : AudioEngine::getAvailableMidiOutputDevices())
            arr.add(juce::var(name));
        sendJson(res, juce::var(arr));
    });

    server->Post(R"(/api/tracks/(\d+)/midi-output)", [this](const httplib::Request& req, httplib::Response& res)
    {
        const int trackId = std::stoi(req.matches[1].str());
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto device = parsed.getProperty("device", juce::var()).toString();
        if (device.isEmpty())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("ok", false);
            obj->setProperty("error", "missing 'device'");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const bool ok = engine.routeToExternalMidi(trackId, device);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", ok);
        if (!ok)
            obj->setProperty("error", "MIDI output device not found: " + device);
        sendJson(res, juce::var(obj), ok ? 200 : 400);
    });

    server->Post("/api/launch-app", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto path = parsed.getProperty("path", juce::var()).toString();
        if (path.isEmpty() || !juce::File::isAbsolutePath(path))
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("ok", false);
            obj->setProperty("error", "missing or invalid 'path'");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const bool ok = AudioEngine::launchExternalApp(juce::File(path));
        sendOk(res, ok);
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

    server->Post("/api/transport/pause", [this](const httplib::Request&, httplib::Response& res)
    {
        sequencer.pause();
        sendOk(res, true);
    });

    server->Post("/api/transport/seek", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        const double beat = (double) parsed.getProperty("beat", 0.0);
        sequencer.setPositionBeats(beat);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        obj->setProperty("positionBeats", sequencer.getPositionBeats());
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

    server->Post("/api/transport/timesignature", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        const int beatsPerBar = (int) parsed.getProperty("beatsPerBar", 4);
        sequencer.setBeatsPerBar(beatsPerBar);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", true);
        obj->setProperty("beatsPerBar", sequencer.getBeatsPerBar());
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

    server->Post("/api/export/midi", [this](const httplib::Request& req, httplib::Response& res)
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

        if (!juce::File::isAbsolutePath(path)) { sendOk(res, false, 200, 400); return; }
        const bool ok = sequencer.exportToMidiFile(juce::File(path));
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", ok);
        obj->setProperty("path", path);
        obj->setProperty("title", juce::File(path).getFileNameWithoutExtension());
        if (!ok)
            obj->setProperty("error", "failed to write MIDI file");
        sendJson(res, juce::var(obj), ok ? 200 : 500);
    });

    server->Post("/api/import/midi", [this](const httplib::Request& req, httplib::Response& res)
    {
        auto parsed = juce::JSON::parse(juce::String(req.body));
        auto path = parsed.getProperty("path", juce::var()).toString();
        const int trackId = (int) parsed.getProperty("trackId", 0);

        if (!path.isEmpty() && !juce::File::isAbsolutePath(path)) { sendOk(res, false, 200, 400); return; }
        if (path.isNotEmpty() && !parsed.hasProperty("trackId"))
        {
            const bool loadPlugins = (bool) parsed.getProperty("loadPlugins", true);
            const auto result = sequencer.importMidiSession(juce::File(path), loadPlugins);
            sendJson(res, result, (bool) result.getProperty("ok", false) ? 200 : 400);
            return;
        }

        if (path.isEmpty())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("ok", false);
            obj->setProperty("error", "missing 'path'");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const auto knownTracks = engine.getTrackIds();
        if (std::find(knownTracks.begin(), knownTracks.end(), trackId) == knownTracks.end())
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("ok", false);
            obj->setProperty("error", "unknown trackId");
            sendJson(res, juce::var(obj), 400);
            return;
        }

        const bool ok = sequencer.importFromMidiFile(juce::File(path), trackId);
        auto* obj = new juce::DynamicObject();
        obj->setProperty("ok", ok);
        if (!ok)
            obj->setProperty("error", "failed to read MIDI file (missing, unreadable, or SMPTE-based)");
        sendJson(res, juce::var(obj), ok ? 200 : 500);
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
