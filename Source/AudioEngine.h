#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include "Track.h"
#include <vector>

// Owns the audio device and every Track, and mixes their output each block.
// Track add/remove is guarded by a CriticalSection also taken in the audio
// callback -- not hard-realtime-safe, but a pragmatic tradeoff for a v0
// scaffold (matches the mutex-in-timer approach already used by Sequencer).
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine() override;

    int addTrack(juce::String name);
    bool removeTrack(int trackId);
    std::vector<int> getTrackIds() const;
    juce::String getTrackName(int trackId);
    bool setTrackName(int trackId, juce::String name);
    int getMidiChannel(int trackId);
    bool setMidiChannel(int trackId, int channel);
    juce::String getPluginPath(int trackId);
    void setInstrumentIdentity(int trackId, juce::String name, juce::String path);

    void loadPlugin(int trackId, const juce::File& file, const std::function<void(juce::String)>& onError);
    bool isPluginLoaded(int trackId);
    juce::String getPluginName(int trackId);
    void showEditorWindow(int trackId);

    // Fallback path for plugins that reliably fail to instantiate however
    // they're hosted (Kontakt 8, UJAM BM-* and others -- root cause
    // undiagnosed): route the track's notes to a named external MIDI
    // output device instead (e.g. a loopMIDI virtual cable feeding a
    // separately-running standalone synth). Returns false if no such
    // device is currently available.
    bool routeToExternalMidi(int trackId, const juce::String& deviceName);
    bool isExternalMidiRouted(int trackId);
    static juce::StringArray getAvailableMidiOutputDevices();

    // Launches a completely independent process (e.g. a standalone synth
    // app) -- not tracked or crash-isolated by this engine at all, just a
    // convenience so the API can start e.g. Kontakt 8 Standalone.
    static bool launchExternalApp(const juce::File& executable);

    void sendNoteOn(int trackId, int channel, int noteNumber, float velocity);
    void sendNoteOff(int trackId, int channel, int noteNumber);
    void sendCC(int trackId, int channel, int controllerNumber, int value);

    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                           float* const* outputChannelData, int numOutputChannels,
                                           int numSamples, const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    Track* findTrack(int trackId);

    juce::AudioDeviceManager deviceManager;

    juce::CriticalSection tracksLock;
    std::vector<std::unique_ptr<Track>> tracks;
    int nextTrackId = 1;

    double sampleRate = 44100.0;
    int blockSize = 512;
};
