#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "PluginServerConnection.h"

#if JUCE_WINDOWS
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #include <windows.h>
#endif

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
    bool launchChildProcess(const juce::String& commandLine);
    void terminateChildProcessIfRunning();

    // Raw Win32 process creation instead of juce::ChildProcess: JUCE's
    // implementation always passes CREATE_NO_WINDOW plus
    // STARTF_USESTDHANDLES (even with no stream capture requested), which
    // Kontakt 8 reliably refused to instantiate under (confirmed via an
    // isolated test: the exact same code launched as a normal foreground
    // process succeeds every time; launched via juce::ChildProcess it fails
    // every time, regardless of every other variable tried). Presumed to be
    // copy-protection/anti-automation logic reacting to a windowless child.
#if JUCE_WINDOWS
    PROCESS_INFORMATION processInfo {};
    bool processStarted = false;
#endif
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
