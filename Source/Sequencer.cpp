#include "Sequencer.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>

namespace
{
juce::MidiMessage textEvent(int type, const juce::String& text)
{
    juce::MemoryOutputStream bytes;
    bytes.writeByte((char) -1);
    bytes.writeByte((char) type);
    auto size = (unsigned int) text.getNumBytesAsUTF8();
    unsigned char vlq[5];
    int count = 0;
    vlq[count++] = (unsigned char) (size & 127);
    while ((size >>= 7) != 0) vlq[count++] = (unsigned char) ((size & 127) | 128);
    while (count > 0) bytes.writeByte((char) vlq[--count]);
    bytes.write(text.toRawUTF8(), text.getNumBytesAsUTF8());
    return juce::MidiMessage(bytes.getData(), (int) bytes.getDataSize());
}

juce::String metaText(const juce::MidiMessage& msg)
{
    return juce::String::fromUTF8((const char*) msg.getMetaEventData(), msg.getMetaEventLength());
}
}

Sequencer::Sequencer(AudioEngine& engineToUse) : engine(engineToUse)
{
}

Sequencer::~Sequencer()
{
    stop();
}

int Sequencer::addNote(int trackId, int pitch, float velocity, double startBeat, double lengthBeats)
{
    std::lock_guard<std::mutex> lock(noteMutex);
    SequencerNote n;
    n.id = nextId++;
    n.trackId = trackId;
    n.pitch = pitch;
    n.velocity = velocity;
    n.startBeat = startBeat;
    n.lengthBeats = juce::jmax(0.001, lengthBeats);
    notes.push_back(n);
    return n.id;
}

bool Sequencer::removeNote(int id)
{
    std::lock_guard<std::mutex> lock(noteMutex);
    auto it = std::find_if(notes.begin(), notes.end(), [id](const SequencerNote& n) { return n.id == id; });
    if (it == notes.end())
        return false;
    notes.erase(it);
    return true;
}

void Sequencer::clearNotes()
{
    std::lock_guard<std::mutex> lock(noteMutex);
    notes.clear();
}

std::vector<SequencerNote> Sequencer::getNotes() const
{
    std::lock_guard<std::mutex> lock(noteMutex);
    return notes;
}

void Sequencer::play()
{
    if (playing.exchange(true))
        return;
    lastTimerMs = juce::Time::getMillisecondCounterHiRes();
    startTimer(3);
}

void Sequencer::stop()
{
    playing = false;
    stopTimer();
    allNotesOff();
    positionBeats = 0.0;
}

void Sequencer::setBpm(double newBpm)
{
    bpm = juce::jmax(1.0, newBpm);
}

void Sequencer::setLoop(bool enabled, double startBeat, double endBeat)
{
    loopEnabled = enabled;
    loopStartBeat = startBeat;
    loopEndBeat = juce::jmax(startBeat + 0.001, endBeat);
}

void Sequencer::allNotesOff()
{
    const auto snapshot = getNotes();
    for (auto id : soundingNoteIds)
    {
        auto it = std::find_if(snapshot.begin(), snapshot.end(), [id](const SequencerNote& n) { return n.id == id; });
        if (it != snapshot.end())
            engine.sendNoteOff(it->trackId, 1, it->pitch);
    }
    soundingNoteIds.clear();
}

void Sequencer::hiResTimerCallback()
{
    const auto nowMs = juce::Time::getMillisecondCounterHiRes();
    const auto dtSeconds = (nowMs - lastTimerMs) * 0.001;
    lastTimerMs = nowMs;

    const auto currentBpm = bpm.load();
    const auto dtBeats = dtSeconds * (currentBpm / 60.0);
    const double prevPos = positionBeats.load();
    double newPos = prevPos + dtBeats;

    const auto snapshot = getNotes();

    for (const auto& n : snapshot)
    {
        const bool startsInWindow = n.startBeat >= prevPos && n.startBeat < newPos;
        const double endBeat = n.startBeat + n.lengthBeats;
        const bool endsInWindow = endBeat >= prevPos && endBeat < newPos;

        if (startsInWindow)
        {
            engine.sendNoteOn(n.trackId, 1, n.pitch, n.velocity);
            soundingNoteIds.insert(n.id);
        }
        if (endsInWindow && soundingNoteIds.count(n.id) > 0)
        {
            engine.sendNoteOff(n.trackId, 1, n.pitch);
            soundingNoteIds.erase(n.id);
        }
    }

    if (loopEnabled && newPos >= loopEndBeat)
    {
        allNotesOff();
        const double loopLength = loopEndBeat - loopStartBeat;
        newPos = loopStartBeat + std::fmod(newPos - loopEndBeat, loopLength);
    }

    positionBeats = newPos;
}

bool Sequencer::exportToMidiFile(const juce::File& file) const
{
    constexpr short ticksPerQuarterNote = 960;

    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);

    juce::MidiMessageSequence tempoTrack;
    tempoTrack.addEvent(textEvent(3, file.getFileNameWithoutExtension()));
    tempoTrack.addEvent(juce::MidiMessage::tempoMetaEvent((int) (60000000.0 / bpm.load())), 0.0);
    midiFile.addTrack(tempoTrack);

    const auto snapshot = getNotes();

    std::set<int> trackIds;
    for (auto id : engine.getTrackIds())
        trackIds.insert(id);

    for (auto trackId : trackIds)
    {
        juce::MidiMessageSequence trackSeq;
        const auto partName = engine.getTrackName(trackId);
        const auto instrument = engine.getPluginName(trackId);
        trackSeq.addEvent(textEvent(3, partName + (instrument.isEmpty() ? "" : " | " + instrument)));
        if (instrument.isNotEmpty()) trackSeq.addEvent(textEvent(4, instrument));
        auto* metadata = new juce::DynamicObject();
        metadata->setProperty("version", 1);
        metadata->setProperty("partName", partName);
        metadata->setProperty("instrument", instrument);
        metadata->setProperty("pluginPath", engine.getPluginPath(trackId));
        trackSeq.addEvent(textEvent(1, "AITracker:" + juce::JSON::toString(juce::var(metadata), true)));
        for (const auto& n : snapshot)
        {
            if (n.trackId != trackId)
                continue;

            const double startTicks = n.startBeat * ticksPerQuarterNote;
            const double endTicks = (n.startBeat + n.lengthBeats) * ticksPerQuarterNote;

            trackSeq.addEvent(juce::MidiMessage::noteOn(1, n.pitch, n.velocity), startTicks);
            trackSeq.addEvent(juce::MidiMessage::noteOff(1, n.pitch), endTicks);
        }
        trackSeq.updateMatchedPairs();
        trackSeq.sort();
        midiFile.addTrack(trackSeq);
    }

    juce::TemporaryFile temporary(file);
    std::unique_ptr<juce::FileOutputStream> stream(temporary.getFile().createOutputStream());
    if (stream == nullptr)
        return false;

    const bool written = midiFile.writeTo(*stream);
    stream->flush();
    const bool streamOk = stream->getStatus().wasOk();
    stream.reset();
    return written && streamOk && temporary.overwriteTargetFileWithTemporary();
}

juce::var Sequencer::importMidiSession(const juce::File& file, bool loadPlugins)
{
    auto* result = new juce::DynamicObject();
    juce::var response(result);
    result->setProperty("ok", false);
    juce::MidiFile midi;
    auto stream = file.createInputStream();
    if (stream == nullptr || !midi.readFrom(*stream) || midi.getTimeFormat() <= 0)
    {
        result->setProperty("error", "Unreadable MIDI or unsupported SMPTE timing");
        return response;
    }

    // Parse completely before changing the current session.
    struct Part { juce::String name, instrument, path; std::vector<SequencerNote> notes; };
    std::vector<Part> parts;
    double initialBpm = 0.0;
    const double ppq = midi.getTimeFormat();
    for (int t = 0; t < midi.getNumTracks(); ++t)
    {
        juce::MidiMessageSequence seq(*midi.getTrack(t));
        seq.updateMatchedPairs();
        Part part;
        juce::var metadata;
        bool named = false;
        for (int i = 0; i < seq.getNumEvents(); ++i)
        {
            const auto* event = seq.getEventPointer(i);
            const auto& msg = event->message;
            if (msg.isTempoMetaEvent() && msg.getTimeStamp() == 0.0 && initialBpm == 0.0)
                initialBpm = 60.0 / msg.getTempoSecondsPerQuarterNote();
            if (msg.isMetaEvent())
            {
                if (msg.getMetaEventType() == 3) { part.name = metaText(msg); named = true; }
                if (msg.getMetaEventType() == 4) part.instrument = metaText(msg);
                if (msg.getMetaEventType() == 1 && metaText(msg).startsWith("AITracker:"))
                    metadata = juce::JSON::parse(metaText(msg).substring(10));
            }
            if (msg.isNoteOn() && event->noteOffObject != nullptr)
            {
                SequencerNote note;
                note.pitch = msg.getNoteNumber();
                note.velocity = msg.getFloatVelocity();
                note.startBeat = msg.getTimeStamp() / ppq;
                note.lengthBeats = juce::jmax(0.001, (event->noteOffObject->message.getTimeStamp() - msg.getTimeStamp()) / ppq);
                part.notes.push_back(note);
            }
        }
        const bool hasMetadata = metadata.isObject() && (int) metadata.getProperty("version", 0) == 1;
        if (part.notes.empty() && !hasMetadata) continue;
        if (hasMetadata)
        {
            part.name = metadata.getProperty("partName", part.name).toString();
            part.instrument = metadata.getProperty("instrument", part.instrument).toString();
            part.path = metadata.getProperty("pluginPath", "").toString();
        }
        else if (named && part.name.contains(" | "))
        {
            if (part.instrument.isEmpty()) part.instrument = part.name.fromLastOccurrenceOf(" | ", false, false);
            part.name = part.name.upToLastOccurrenceOf(" | ", false, false);
        }
        if (part.name.isEmpty()) part.name = "Part " + juce::String(t + 1);
        parts.push_back(std::move(part));
    }
    if (parts.empty()) { result->setProperty("error", "No note tracks found"); return response; }

    stop();
    juce::Array<juce::var> imported;
    for (auto& part : parts)
    {
        const int id = engine.addTrack(part.name);
        // A stored path is preferred; the standard VST3 folder allows moving a file between PCs.
        auto pluginFile = juce::File();
        if (juce::File::isAbsolutePath(part.path)) pluginFile = juce::File(part.path);
        if (!pluginFile.exists() && part.path.isNotEmpty())
            pluginFile = juce::File("C:\\Program Files\\Common Files\\VST3").getChildFile(juce::File::createLegalFileName(part.path.replaceCharacter('\\', '/').fromLastOccurrenceOf("/", false, false)));
        // Human-readable-only MIDI from other DAWs can still resolve known local instruments.
        if (!pluginFile.exists())
        {
            const std::map<juce::String, juce::String> aliases {
                { "Ample Bass U", "ABU.vst3" }, { "Ample Guitar L", "AGL.vst3" },
                { "Ample Bass J", "ABJ.vst3" }, { "Ample Bass A", "ABA.vst3" }
            };
            auto found = aliases.find(part.instrument);
            if (found != aliases.end()) pluginFile = juce::File("C:\\Program Files\\Common Files\\VST3").getChildFile(found->second);
        }
        engine.setInstrumentIdentity(id, part.instrument, part.path);
        juce::String error;
        if (loadPlugins && part.instrument.isNotEmpty())
        {
            if (!pluginFile.exists() || !pluginFile.hasFileExtension("vst3")) error = "VST3 not found";
            else engine.loadPlugin(id, pluginFile, [&error](juce::String message) { error = message; });
        }
        for (const auto& note : part.notes) addNote(id, note.pitch, note.velocity, note.startBeat, note.lengthBeats);
        auto* item = new juce::DynamicObject();
        item->setProperty("id", id);
        item->setProperty("name", part.name);
        item->setProperty("instrument", part.instrument);
        item->setProperty("pluginLoaded", engine.isPluginLoaded(id));
        item->setProperty("noteCount", (int) part.notes.size());
        if (error.isNotEmpty()) item->setProperty("warning", error);
        imported.add(juce::var(item));
    }
    if (initialBpm > 0.0 && std::isfinite(initialBpm)) setBpm(initialBpm);
    double endBeat = 4.0;
    for (const auto& note : getNotes()) endBeat = juce::jmax(endBeat, note.startBeat + note.lengthBeats);
    setLoop(true, 0.0, std::ceil(endBeat / 4.0) * 4.0);
    result->setProperty("ok", true);
    result->setProperty("title", file.getFileNameWithoutExtension());
    result->setProperty("bpm", getBpm());
    result->setProperty("tracks", imported);
    return response;
}

bool Sequencer::importFromMidiFile(const juce::File& file, int trackId)
{
    juce::MidiFile midiFile;
    std::unique_ptr<juce::FileInputStream> stream(file.createInputStream());
    if (stream == nullptr || !midiFile.readFrom(*stream))
        return false;

    if (midiFile.getTimeFormat() <= 0)
        return false; // SMPTE-timed files are not supported in this v1 importer.

    const auto ticksPerQuarterNote = (double) midiFile.getTimeFormat();

    std::lock_guard<std::mutex> lock(noteMutex);

    for (int t = 0; t < midiFile.getNumTracks(); ++t)
    {
        juce::MidiMessageSequence seq(*midiFile.getTrack(t));
        seq.updateMatchedPairs();

        for (int i = 0; i < seq.getNumEvents(); ++i)
        {
            auto* holder = seq.getEventPointer(i);
            if (!holder->message.isNoteOn())
                continue;

            auto* offHolder = holder->noteOffObject;
            if (offHolder == nullptr)
                continue;

            const double startBeat = holder->message.getTimeStamp() / ticksPerQuarterNote;
            const double endBeat = offHolder->message.getTimeStamp() / ticksPerQuarterNote;

            SequencerNote n;
            n.id = nextId++;
            n.trackId = trackId;
            n.pitch = holder->message.getNoteNumber();
            n.velocity = holder->message.getFloatVelocity();
            n.startBeat = startBeat;
            n.lengthBeats = juce::jmax(0.001, endBeat - startBeat);
            notes.push_back(n);
        }
    }

    return true;
}
