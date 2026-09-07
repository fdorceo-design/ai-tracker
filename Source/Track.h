#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "PluginServerProxy.h"

// One instrument slot. The plugin runs out-of-process (PluginServerProxy)
// so a crash in it can't take the host down; this class just forwards
// load/note/render calls and mixes the proxy's rendered audio in.
class Track
{
public:
    Track(int idIn, juce::String nameIn);

    int getId() const { return id; }
    juce::String getName() const { return name; }
    void setName(juce::String value) { name = std::move(value); }
    juce::String getPluginPath() const { return pluginPath; }
    void setInstrumentIdentity(juce::String label, juce::String path)
    {
        instrumentName = std::move(label);
        pluginPath = std::move(path);
    }

    void loadPlugin(const juce::File& file, double sampleRate, int blockSize,
                     const std::function<void(juce::String)>& onError);
    bool isPluginLoaded() const { return proxy.isPluginLoaded(); }
    juce::String getPluginName() const;

    void showEditorWindow();

    void prepareToPlay(double sampleRate, int blockSize);
    void releaseResources();

    // Audio-thread only: pulls queued MIDI, runs the plugin, and adds its
    // output into `outputBuffer` (does not clear it).
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples);

    void sendNoteOn(int channel, int noteNumber, float velocity);
    void sendNoteOff(int channel, int noteNumber);

private:
    int id;
    juce::String name;
    juce::String instrumentName, pluginPath;
    PluginServerProxy proxy;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
};
