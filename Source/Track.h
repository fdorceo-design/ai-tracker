#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>

// One instrument slot: owns a loaded VST3 instance and its own MIDI queue,
// and mixes its rendered audio into the engine's output buffer each block.
class Track
{
public:
    Track(int idIn, juce::String nameIn);
    ~Track();

    int getId() const { return id; }
    juce::String getName() const { return name; }
    void setName(juce::String value) { name = std::move(value); }
    juce::String getPluginPath() const { return pluginPath; }
    void setInstrumentIdentity(juce::String label, juce::String path)
    { instrumentName = std::move(label); pluginPath = std::move(path); }

    void loadPlugin(juce::AudioPluginFormatManager& formatManager, const juce::File& file,
                     double sampleRate, int blockSize, const std::function<void(juce::String)>& onError);
    bool isPluginLoaded() const { return processor != nullptr; }
    juce::String getPluginName() const;

    void showEditorWindow();

    void prepareToPlay(double sampleRate, int blockSize);
    void releaseResources();

    // Audio-thread only: pulls queued MIDI, runs the plugin, and adds its
    // output into `outputBuffer` (does not clear it first).
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples);

    void sendNoteOn(int channel, int noteNumber, float velocity);
    void sendNoteOff(int channel, int noteNumber);

private:
    class PluginWindow;

    void hideEditorWindow();

    int id;
    juce::String name;
    juce::String instrumentName, pluginPath;
    std::unique_ptr<juce::AudioPluginInstance> processor;
    juce::MidiMessageCollector midiCollector;
    juce::AudioBuffer<float> scratchBuffer;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    std::unique_ptr<PluginWindow> editorWindow;
};
