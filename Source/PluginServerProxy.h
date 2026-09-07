#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "PluginServerConnection.h"

// Owns one AiTrackerPluginServer child process and talks to it over a named
// pipe. The plugin itself lives entirely in that process, so a crash there
// (Kontakt et al. have been observed to crash on their own internal
// threads, which no in-process guard can catch) only kills the child --
// this proxy just reports "not loaded" / silence instead of taking the
// whole host down.
//
// renderNextBlock is called from AudioEngine's real audio callback and
// blocks briefly waiting for the child's response over the pipe -- an
// accepted latency/glitch-risk tradeoff in exchange for crash isolation.
class PluginServerProxy
{
public:
    PluginServerProxy();
    ~PluginServerProxy();

    void loadPlugin(const juce::File& pluginFile, double sampleRate, int blockSize,
                     const std::function<void(juce::String)>& onError);
    void closePlugin();

    bool isPluginLoaded() const { return loaded.load(); }
    juce::String getPluginName() const;

    void showEditorWindow();

    void sendNoteOn(int channel, int noteNumber, float velocity);
    void sendNoteOff(int channel, int noteNumber);

    // Audio-thread only. Adds (does not clear) into outputBuffer.
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples);

private:
    void handleMessage(const juce::MemoryBlock& message);

    std::unique_ptr<juce::ChildProcess> childProcess;
    std::unique_ptr<PluginServerConnection> connection;

    std::atomic<bool> loaded { false };
    juce::String pluginName;

    juce::WaitableEvent loadResultEvent;
    juce::String loadError;

    juce::CriticalSection midiLock;
    juce::MidiBuffer pendingMidi;

    juce::CriticalSection resultLock;
    juce::WaitableEvent audioResultEvent;
    juce::AudioBuffer<float> lastResult;
};
