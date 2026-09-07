#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include "PluginServerProxy.h"

// One instrument slot. Normally the plugin runs out-of-process
// (PluginServerProxy) so a crash in it can't take the host down. Some
// plugins (Kontakt 8, UJAM BM-* and others were found to reliably fail to
// instantiate no matter how they're hosted -- root cause undiagnosed after
// extensive investigation) can instead be routed to an external MIDI
// device: run the real standalone app separately (see
// AudioEngine::launchExternalApp) and send it notes over a virtual MIDI
// cable (e.g. loopMIDI). Audio in that mode plays directly from the
// external app to the real output device, not through this host, so
// renderNextBlock is a no-op for an externally-routed track.
class Track
{
public:
    Track(int idIn, juce::String nameIn);

    int getId() const { return id; }
    juce::String getName() const { return name; }
    void setName(juce::String value) { name = std::move(value); }
    juce::String getPluginPath() const { return pluginPath; }

    // 1-16. Lets several tracks share one external MIDI device/port (e.g.
    // one multitimbral standalone instance with 16 slots, each on its own
    // channel) and stay addressable independently.
    int getMidiChannel() const { return midiChannel; }
    void setMidiChannel(int channel) { midiChannel = juce::jlimit(1, 16, channel); }
    void setInstrumentIdentity(juce::String label, juce::String path)
    {
        instrumentName = std::move(label);
        pluginPath = std::move(path);
    }

    void loadPlugin(const juce::File& file, double sampleRate, int blockSize,
                     const std::function<void(juce::String)>& onError);
    bool isPluginLoaded() const { return externalMidiOutput != nullptr || proxy.isPluginLoaded(); }
    juce::String getPluginName() const;

    void showEditorWindow();

    // Switches this track to send notes to a named external MIDI output
    // device instead of the in-process/child-process plugin path. Closes
    // any currently loaded VST3 first. Returns false if no MIDI output
    // device with that name is currently available.
    bool routeToExternalMidi(const juce::String& deviceName);
    bool isExternalMidiRouted() const { return externalMidiOutput != nullptr; }

    void prepareToPlay(double sampleRate, int blockSize);
    void releaseResources();

    // Audio-thread only: pulls queued MIDI, runs the plugin, and adds its
    // output into `outputBuffer` (does not clear it). No-op when routed to
    // an external MIDI device.
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples);

    void sendNoteOn(int channel, int noteNumber, float velocity);
    void sendNoteOff(int channel, int noteNumber);

private:
    int id;
    juce::String name;
    juce::String instrumentName, pluginPath;
    PluginServerProxy proxy;
    std::unique_ptr<juce::MidiOutput> externalMidiOutput;
    juce::String externalMidiDeviceName;
    int midiChannel = 1;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
};
