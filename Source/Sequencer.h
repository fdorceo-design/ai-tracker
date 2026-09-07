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

// A one-shot MIDI CC (e.g. CC1 mod wheel/dynamics, CC11 expression,
// CC21 vibrato on the Sacconi library) fired at a given beat. Unlike a
// note there's no "off" -- the value just stays until the next CC on that
// controller/track.
struct SequencerCC
{
    int id = 0;
    int trackId = 0;
    int controller = 1; // raw MIDI CC number, 0-127
    int value = 0;      // raw MIDI CC value, 0-127
    double beat = 0.0;
};

// A scheduled tempo change. Not per-track -- affects the whole sequencer's
// playback speed from this beat onward, until the next tempo event (or none).
struct SequencerTempoEvent
{
    int id = 0;
    double beat = 0.0;
    double bpm = 120.0;
};

// A scheduled time-signature change. Assumed to land exactly on a bar
// boundary of whatever meter was active before it -- bar-index math (for
// the tracker grid's bar lines) walks the events in order and accumulates
// completed bars per segment, so a change placed mid-bar will misalign.
struct SequencerTimeSigEvent
{
    int id = 0;
    double beat = 0.0;
    int beatsPerBar = 4;
};

class Sequencer : private juce::HighResolutionTimer
{
public:
    explicit Sequencer(AudioEngine& engineToUse);
    ~Sequencer() override;

    int addNote(int trackId, int pitch, float velocity, double startBeat, double lengthBeats);
    bool removeNote(int id);
    bool setNotePitch(int id, int newPitch);
    bool setNoteBeat(int id, double newStartBeat);
    bool setNoteLength(int id, double newLengthBeats);
    bool setNoteVelocity(int id, float newVelocity);
    void clearNotes();
    std::vector<SequencerNote> getNotes() const;

    int addCC(int trackId, int controller, int value, double beat);
    bool removeCC(int id);
    void clearCC();
    std::vector<SequencerCC> getCCEvents() const;

    int addTempoEvent(double beat, double bpm);
    bool removeTempoEvent(int id);
    void clearTempoEvents();
    std::vector<SequencerTempoEvent> getTempoEvents() const;

    int addTimeSigEvent(double beat, int beatsPerBar);
    bool removeTimeSigEvent(int id);
    void clearTimeSigEvents();
    std::vector<SequencerTimeSigEvent> getTimeSigEvents() const;

    // Bar index for a beat, accounting for any scheduled time-signature
    // changes (see SequencerTimeSigEvent). Used for the tracker grid's bar
    // lines so they stay correct across a meter change.
    int getBarIndexForBeat(double beat) const;

    void play();
    void stop();
    // Halts playback (and silences sounding notes) without resetting the
    // position, unlike stop(). Calling play() again resumes from here.
    void pause();
    bool isPlaying() const { return playing.load(); }

    // Jumps the playhead to a given beat, silencing any currently-sounding
    // notes first. Safe to call whether playing or stopped.
    void setPositionBeats(double beat);

    void setBpm(double newBpm);
    double getBpm() const { return bpm.load(); }

    // Numerator only (quarter-note beat unit assumed, i.e. .../4). Affects
    // bar-line placement in the tracker grid and nothing else yet.
    void setBeatsPerBar(int newBeatsPerBar);
    int getBeatsPerBar() const { return beatsPerBar.load(); }

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
    // Append separate tracks, restoring names, instrument identities and
    // initial tempo. Plugin loading (loadPlugins=true) spawns a child
    // process per track and blocks waiting on it, so call this off the
    // JUCE message thread to avoid freezing the GUI during a slow/crashing
    // load.
    juce::var importMidiSession(const juce::File& file, bool loadPlugins = true);

private:
    void hiResTimerCallback() override;
    void allNotesOff();

    AudioEngine& engine;

    mutable std::mutex noteMutex;
    std::vector<SequencerNote> notes;
    int nextId = 1;

    mutable std::mutex ccMutex;
    std::vector<SequencerCC> ccEvents;
    int nextCcId = 1;

    mutable std::mutex tempoMutex;
    std::vector<SequencerTempoEvent> tempoEvents;
    int nextTempoId = 1;

    mutable std::mutex timeSigMutex;
    std::vector<SequencerTimeSigEvent> timeSigEvents;
    int nextTimeSigId = 1;

    std::atomic<bool> playing { false };
    std::atomic<double> bpm { 120.0 };
    std::atomic<int> beatsPerBar { 4 };
    std::atomic<double> positionBeats { 0.0 };

    bool loopEnabled = false;
    double loopStartBeat = 0.0;
    double loopEndBeat = 4.0;

    double lastTimerMs = 0.0;
    std::set<int> soundingNoteIds;
};
