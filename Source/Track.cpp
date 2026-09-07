#include "Track.h"

Track::Track(int idIn, juce::String nameIn) : id(idIn), name(std::move(nameIn))
{
}

void Track::loadPlugin(const juce::File& file, double sampleRate, int blockSize,
                        const std::function<void(juce::String)>& onError)
{
    externalMidiOutput.reset();
    externalMidiDeviceName.clear();

    proxy.loadPlugin(file, sampleRate, blockSize, onError);
    if (proxy.isPluginLoaded())
    {
        instrumentName = proxy.getPluginName();
        pluginPath = file.getFullPathName();
    }
}

bool Track::routeToExternalMidi(const juce::String& deviceName)
{
    proxy.closePlugin();
    externalMidiOutput.reset();
    externalMidiDeviceName.clear();

    for (const auto& device : juce::MidiOutput::getAvailableDevices())
    {
        if (device.name == deviceName)
        {
            externalMidiOutput = juce::MidiOutput::openDevice(device.identifier);
            break;
        }
    }

    if (externalMidiOutput == nullptr)
        return false;

    externalMidiDeviceName = deviceName;
    instrumentName = "External MIDI: " + deviceName;
    pluginPath.clear();
    return true;
}

juce::String Track::getPluginName() const
{
    if (externalMidiOutput != nullptr)
        return "External MIDI: " + externalMidiDeviceName;

    // Prefer the proxy's live name: once a plugin has loaded, it also
    // reflects a subsequent crash (appends "(crashed)"), which the
    // persisted instrumentName -- captured only at load time -- would
    // otherwise silently hide.
    const auto proxyName = proxy.getPluginName();
    return proxyName.isNotEmpty() ? proxyName : instrumentName;
}

void Track::showEditorWindow()
{
    proxy.showEditorWindow();
}

void Track::prepareToPlay(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
}

void Track::releaseResources()
{
}

void Track::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples)
{
    if (externalMidiOutput != nullptr)
        return;
    proxy.renderNextBlock(outputBuffer, numSamples);
}

void Track::sendNoteOn(int channel, int noteNumber, float velocity)
{
    if (externalMidiOutput != nullptr)
    {
        externalMidiOutput->sendMessageNow(juce::MidiMessage::noteOn(channel, noteNumber, velocity));
        return;
    }
    proxy.sendNoteOn(channel, noteNumber, velocity);
}

void Track::sendNoteOff(int channel, int noteNumber)
{
    if (externalMidiOutput != nullptr)
    {
        externalMidiOutput->sendMessageNow(juce::MidiMessage::noteOff(channel, noteNumber));
        return;
    }
    proxy.sendNoteOff(channel, noteNumber);
}

void Track::sendCC(int channel, int controllerNumber, int value)
{
    if (externalMidiOutput != nullptr)
    {
        externalMidiOutput->sendMessageNow(juce::MidiMessage::controllerEvent(channel, controllerNumber, value));
        return;
    }
    proxy.sendCC(channel, controllerNumber, value);
}
