#include "Sequencer.h"
#include <algorithm>
#include <cmath>
#include <set>

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
    if (!playing.exchange(false))
        return;
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
    tempoTrack.addEvent(juce::MidiMessage::tempoMetaEvent((int) (60000000.0 / bpm.load())), 0.0);
    midiFile.addTrack(tempoTrack);

    const auto snapshot = getNotes();

    std::set<int> trackIds;
    for (const auto& n : snapshot)
        trackIds.insert(n.trackId);

    for (auto trackId : trackIds)
    {
        juce::MidiMessageSequence trackSeq;
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

    file.deleteFile();
    std::unique_ptr<juce::FileOutputStream> stream(file.createOutputStream());
    if (stream == nullptr)
        return false;

    return midiFile.writeTo(*stream);
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
