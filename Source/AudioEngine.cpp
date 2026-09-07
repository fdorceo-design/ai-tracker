#include "AudioEngine.h"
#include <algorithm>

AudioEngine::AudioEngine()
{
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(this);
}

AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback(this);
    const juce::ScopedLock lock(tracksLock);
    tracks.clear();
}

Track* AudioEngine::findTrack(int trackId)
{
    for (auto& t : tracks)
        if (t->getId() == trackId)
            return t.get();
    return nullptr;
}

int AudioEngine::addTrack(juce::String name)
{
    const juce::ScopedLock lock(tracksLock);
    const int id = nextTrackId++;
    auto track = std::make_unique<Track>(id, name.isNotEmpty() ? name : ("Track " + juce::String(id)));
    track->prepareToPlay(sampleRate, blockSize);
    tracks.push_back(std::move(track));
    return id;
}

bool AudioEngine::removeTrack(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto it = std::find_if(tracks.begin(), tracks.end(),
                            [trackId](const std::unique_ptr<Track>& t) { return t->getId() == trackId; });
    if (it == tracks.end())
        return false;
    tracks.erase(it);
    return true;
}

std::vector<int> AudioEngine::getTrackIds() const
{
    const juce::ScopedLock lock(tracksLock);
    std::vector<int> ids;
    ids.reserve(tracks.size());
    for (auto& t : tracks)
        ids.push_back(t->getId());
    return ids;
}

juce::String AudioEngine::getTrackName(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr ? track->getName() : juce::String();
}

void AudioEngine::loadPlugin(int trackId, const juce::File& file, const std::function<void(juce::String)>& onError)
{
    Track* track = nullptr;
    {
        const juce::ScopedLock lock(tracksLock);
        track = findTrack(trackId);
    }
    if (track == nullptr)
    {
        onError("Unknown track " + juce::String(trackId));
        return;
    }
    // Deliberately unlocked: loading blocks waiting on the child process
    // (can legitimately take seconds for a large sample library), and
    // holding tracksLock for that long would stall the real audio
    // callback's ability to iterate/mix every other track. Safe because
    // the pointer can't be invalidated concurrently -- track add/remove
    // only ever happens from this same (JUCE message) thread.
    track->loadPlugin(file, sampleRate, blockSize, onError);
}

bool AudioEngine::setTrackName(int trackId, juce::String name)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId)) { track->setName(std::move(name)); return true; }
    return false;
}

int AudioEngine::getMidiChannel(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr ? track->getMidiChannel() : 1;
}

bool AudioEngine::setMidiChannel(int trackId, int channel)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId)) { track->setMidiChannel(channel); return true; }
    return false;
}

juce::String AudioEngine::getPluginPath(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr ? track->getPluginPath() : juce::String();
}

void AudioEngine::setInstrumentIdentity(int trackId, juce::String name, juce::String path)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId)) track->setInstrumentIdentity(std::move(name), std::move(path));
}

bool AudioEngine::routeToExternalMidi(int trackId, const juce::String& deviceName)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr && track->routeToExternalMidi(deviceName);
}

bool AudioEngine::isExternalMidiRouted(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr && track->isExternalMidiRouted();
}

juce::StringArray AudioEngine::getAvailableMidiOutputDevices()
{
    juce::StringArray names;
    for (const auto& device : juce::MidiOutput::getAvailableDevices())
        names.add(device.name);
    return names;
}

bool AudioEngine::launchExternalApp(const juce::File& executable)
{
    return executable.existsAsFile() && executable.startAsProcess();
}

bool AudioEngine::isPluginLoaded(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr && track->isPluginLoaded();
}

juce::String AudioEngine::getPluginName(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    auto* track = findTrack(trackId);
    return track != nullptr ? track->getPluginName() : juce::String();
}

void AudioEngine::showEditorWindow(int trackId)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId))
        track->showEditorWindow();
}

void AudioEngine::sendNoteOn(int trackId, int channel, int noteNumber, float velocity)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId))
        track->sendNoteOn(channel, noteNumber, velocity);
}

void AudioEngine::sendNoteOff(int trackId, int channel, int noteNumber)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId))
        track->sendNoteOff(channel, noteNumber);
}

void AudioEngine::sendCC(int trackId, int channel, int controllerNumber, int value)
{
    const juce::ScopedLock lock(tracksLock);
    if (auto* track = findTrack(trackId))
        track->sendCC(channel, controllerNumber, value);
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* /*inputChannelData*/, int /*numInputChannels*/,
                                                    float* const* outputChannelData, int numOutputChannels,
                                                    int numSamples, const juce::AudioIODeviceCallbackContext&)
{
    juce::AudioBuffer<float> outputBuffer(outputChannelData, numOutputChannels, numSamples);
    outputBuffer.clear();

    const juce::ScopedLock lock(tracksLock);
    for (auto& track : tracks)
        track->renderNextBlock(outputBuffer, numSamples);
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    blockSize = device->getCurrentBufferSizeSamples();

    const juce::ScopedLock lock(tracksLock);
    for (auto& track : tracks)
        track->prepareToPlay(sampleRate, blockSize);
}

void AudioEngine::audioDeviceStopped()
{
    const juce::ScopedLock lock(tracksLock);
    for (auto& track : tracks)
        track->releaseResources();
}
