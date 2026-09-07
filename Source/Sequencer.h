#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioEngine.h"
#include <vector>
#include <mutex>
#include <set>

// One note event in beats, tagged with the track it belongs to.
// Timing is driven by a HighResolutionTimer polling loop rather than the
// audio callback, so it is not sample-accurate, but is good enough for an
// AI to sketch and audition ideas; a sample-accurate path can replace this
// later without changing the API shape.
struct SequencerNote
{
    int id = 0;
    int trackId = 0;
    int pitch = 60;
    float velocity = 0.8f;
    double startBeat = 0.0;
    double lengthBeats = 1.0;
};

class Sequencer : private juce::HighResolutionTimer
{
public:
    explicit Sequencer(AudioEngine& engineToUse);
    ~Sequencer() override;

    int addNote(int trackId, int pitch, float velocity, double startBeat, double lengthBeats);
    bool removeNote(int id);
    bool setNotePitch(int id, int newPitch);
    void clearNotes();
    std::vector<SequencerNote> getNotes() const;

    void play();
    void stop();
    bool isPlaying() const { return playing.load(); }

    void setBpm(double newBpm);
    double getBpm() const { return bpm.load(); }

    void setLoop(bool enabled, double startBeat, double endBeat);
    double getPositionBeats() const { return positionBeats.load(); }

    // One MIDI track per distinct trackId present in `notes`, plus a tempo
    // track set to the current BPM. Ticks-based (not SMPTE).
    bool exportToMidiFile(const juce::File& file) const;

    // Reads every track in the file and appends all of its notes, tagged
    // with the given trackId. Absolute tempo from the file is ignored --
    // notes land at the same beat positions they were written at, played
    // back at this sequencer's own BPM. SMPTE-timed files are not supported.
    bool importFromMidiFile(const juce::File& file, int trackId);
    // Append separate tracks, restoring names, instrument identities and initial tempo.
    // Must run on the JUCE message thread when loadPlugins is true.
    juce::var importMidiSession(const juce::File& file, bool loadPlugins = true);

private:
    void hiResTimerCallback() override;
    void allNotesOff();

    AudioEngine& engine;

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
