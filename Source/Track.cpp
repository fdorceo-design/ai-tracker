#include "Track.h"

Track::Track(int idIn, juce::String nameIn) : id(idIn), name(std::move(nameIn))
{
}

void Track::loadPlugin(const juce::File& file, double sampleRate, int blockSize,
                        const std::function<void(juce::String)>& onError)
{
    proxy.loadPlugin(file, sampleRate, blockSize, onError);
    if (proxy.isPluginLoaded())
    {
        instrumentName = proxy.getPluginName();
        pluginPath = file.getFullPathName();
    }
}

juce::String Track::getPluginName() const
{
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
    proxy.renderNextBlock(outputBuffer, numSamples);
}

void Track::sendNoteOn(int channel, int noteNumber, float velocity)
{
    proxy.sendNoteOn(channel, noteNumber, velocity);
}

void Track::sendNoteOff(int channel, int noteNumber)
{
    proxy.sendNoteOff(channel, noteNumber);
}
