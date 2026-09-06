#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

// Loads a single VST3 instrument, routes its audio to the default output
// device, and lets MIDI notes be injected programmatically (a stand-in for
// notes that will eventually come from the sequencer or the AI API).
class PluginHost
{
public:
    PluginHost();
    ~PluginHost();

    void loadPlugin(const juce::File& file, std::function<void(juce::String)> onError);
    void closePlugin();

    bool isPluginLoaded() const { return processor != nullptr; }
    juce::String getPluginName() const;

    void showEditorWindow();

    void sendNoteOn(int channel, int noteNumber, float velocity);
    void sendNoteOff(int channel, int noteNumber);

    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

private:
    class PluginWindow;

    void setProcessor(std::unique_ptr<juce::AudioPluginInstance> newProcessor);
    void hideEditorWindow();

    juce::AudioPluginFormatManager formatManager;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer player;
    std::unique_ptr<juce::AudioPluginInstance> processor;
    std::unique_ptr<PluginWindow> editorWindow;
};
