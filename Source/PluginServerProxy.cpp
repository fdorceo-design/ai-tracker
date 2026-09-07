#include "PluginServerProxy.h"
#include "PluginServerProtocol.h"

namespace
{
    juce::File findPluginServerExe()
    {
        const auto exeDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
        return exeDir.getChildFile("AiTrackerPluginServer.exe");
    }

    juce::String makePipeName()
    {
        return "AiTrackerPlugin_" + juce::Uuid().toString();
    }
}

PluginServerProxy::PluginServerProxy()
{
    connection = std::make_unique<PluginServerConnection>(false);
    connection->onMessage = [this](const juce::MemoryBlock& mb) { handleMessage(mb); };
    connection->onDisconnect = [this]
    {
        // The child died (crash, or was killed) or the pipe otherwise
        // dropped -- reflect that instead of leaving stale "loaded" state
        // that renderNextBlock/showEditorWindow would silently no-op
        // against forever.
        loaded = false;
        pluginName = pluginName.isNotEmpty() ? pluginName + " (crashed)" : "(crashed)";
        audioResultEvent.signal();
        loadResultEvent.signal();
    };
}

PluginServerProxy::~PluginServerProxy()
{
    closePlugin();
}

void PluginServerProxy::handleMessage(const juce::MemoryBlock& mb)
{
    using namespace PluginServerProtocol;
    switch (readType(mb))
    {
        case MessageType::loadResult:
        {
            bool ok = false;
            juce::String name, error;
            parseLoadResult(mb, ok, name, error);
            loaded = ok;
            pluginName = name;
            loadError = error;
            loadResultEvent.signal();
            break;
        }
        case MessageType::audioResult:
        {
            const juce::ScopedLock lock(resultLock);
            parseAudioResult(mb, lastResult);
            audioResultEvent.signal();
            break;
        }
        default:
            break;
    }
}

void PluginServerProxy::loadPlugin(const juce::File& pluginFile, double sampleRate, int blockSize,
                                    const std::function<void(juce::String)>& onError)
{
    closePlugin();

    const auto serverExe = findPluginServerExe();
    if (!serverExe.existsAsFile())
    {
        onError("AiTrackerPluginServer.exe not found next to the main app");
        return;
    }

    const auto pipeName = makePipeName();
    if (!connection->createPipe(pipeName, 30000))
    {
        onError("Failed to create IPC pipe for plugin server");
        return;
    }

    juce::StringArray args;
    args.add(serverExe.getFullPathName());
    args.add("--pipe");
    args.add(pipeName);
    args.add("--plugin");
    args.add(pluginFile.getFullPathName());
    args.add("--samplerate");
    args.add(juce::String(sampleRate, 0));
    args.add("--blocksize");
    args.add(juce::String(blockSize));

    childProcess = std::make_unique<juce::ChildProcess>();
    if (!childProcess->start(args))
    {
        onError("Failed to launch AiTrackerPluginServer.exe");
        connection->disconnect();
        childProcess.reset();
        return;
    }

    loadResultEvent.reset();
    // Heavy sample libraries can take a while to instantiate.
    if (!loadResultEvent.wait(30000))
    {
        onError("Plugin server did not respond (timed out loading)");
        closePlugin();
        return;
    }

    if (!loaded.load())
    {
        onError(loadError.isNotEmpty() ? loadError : "Plugin failed to load");
        closePlugin();
    }
}

void PluginServerProxy::closePlugin()
{
    if (connection != nullptr && connection->isConnected())
        connection->sendMessage(PluginServerProtocol::buildSimple(PluginServerProtocol::MessageType::shutdown));

    if (connection != nullptr)
        connection->disconnect();

    if (childProcess != nullptr)
    {
        childProcess->waitForProcessToFinish(2000);
        if (childProcess->isRunning())
            childProcess->kill();
        childProcess.reset();
    }

    loaded = false;
    pluginName.clear();
}

juce::String PluginServerProxy::getPluginName() const
{
    return pluginName;
}

void PluginServerProxy::showEditorWindow()
{
    if (connection != nullptr && connection->isConnected())
        connection->sendMessage(PluginServerProtocol::buildSimple(PluginServerProtocol::MessageType::showEditor));
}

void PluginServerProxy::sendNoteOn(int channel, int noteNumber, float velocity)
{
    const juce::ScopedLock lock(midiLock);
    pendingMidi.addEvent(juce::MidiMessage::noteOn(channel, noteNumber, velocity), 0);
}

void PluginServerProxy::sendNoteOff(int channel, int noteNumber)
{
    const juce::ScopedLock lock(midiLock);
    pendingMidi.addEvent(juce::MidiMessage::noteOff(channel, noteNumber), 0);
}

void PluginServerProxy::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples)
{
    if (!loaded.load() || connection == nullptr || !connection->isConnected())
        return;

    juce::MidiBuffer midiForThisBlock;
    {
        const juce::ScopedLock lock(midiLock);
        midiForThisBlock.swapWith(pendingMidi);
    }

    audioResultEvent.reset();
    connection->sendMessage(PluginServerProtocol::buildProcessBlock(numSamples, midiForThisBlock));

    // A short timeout keeps a hung/dead child from freezing audio forever;
    // on timeout this block is just silent.
    if (audioResultEvent.wait(50))
    {
        const juce::ScopedLock lock(resultLock);
        const int channels = juce::jmin(outputBuffer.getNumChannels(), lastResult.getNumChannels());
        const int samples = juce::jmin(numSamples, lastResult.getNumSamples());
        for (int ch = 0; ch < channels; ++ch)
            outputBuffer.addFrom(ch, 0, lastResult, ch, 0, samples);
    }
}
