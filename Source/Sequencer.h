#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginHost.h"
#include <vector>
#include <mutex>
#include <set>

// One note event in beats, resolved against a single MIDI channel.
// Timing is driven by a HighResolutionTimer polling loop rather than the
// audio callback, so it is not sample-accurate, but is good enough for an
// AI to sketch and audition ideas; a sample-accurate path can replace this
// later without changing the API shape.
struct SequencerNote
{
    int id = 0;
    int pitch = 60;
    float velocity = 0.8f;
    double startBeat = 0.0;
    double lengthBeats = 1.0;
};

class Sequencer : private juce::HighResolutionTimer
{
public:
    explicit Sequencer(PluginHost& hostToUse);
    ~Sequencer() override;

    int addNote(int pitch, float velocity, double startBeat, double lengthBeats);
    bool removeNote(int id);
    void clearNotes();
    std::vector<SequencerNote> getNotes() const;

    void play();
    void stop();
    bool isPlaying() const { return playing.load(); }

    void setBpm(double newBpm);
    double getBpm() const { return bpm.load(); }

    void setLoop(bool enabled, double startBeat, double endBeat);
    double getPositionBeats() const { return positionBeats.load(); }

private:
    void hiResTimerCallback() override;
    void allNotesOff();

    PluginHost& pluginHost;

    mutable std::mutex noteMutex;
    std::vector<SequencerNote> notes;
    int nextId = 1;

    std::atomic<bool> playing { false };
    std::atomic<double> bpm { 120.0 };
    std::atomic<double> positionBeats { 0.0 };

    bool loopEnabled = false;
    double loopStartBeat = 0.0;
    double loopEndBeat = 4.0;

    double lastTimerMs = 0.0;
    std::set<int> soundingNoteIds;
};
