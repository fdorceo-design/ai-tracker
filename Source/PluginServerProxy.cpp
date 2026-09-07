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

    juce::String quoteIfNeeded(const juce::String& arg)
    {
        if (!arg.containsAnyOf(" \t\""))
            return arg;
        return "\"" + arg.replace("\"", "\\\"") + "\"";
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

bool PluginServerProxy::launchChildProcess(const juce::String& commandLine)
{
#if JUCE_WINDOWS
    STARTUPINFOW startupInfo {};
    startupInfo.cb = sizeof(startupInfo);

    // Raw CreateProcess (bInheritHandles=FALSE, no CREATE_NO_WINDOW) instead
    // of juce::ChildProcess -- see the comment on processInfo in the header
    // for why: JUCE's implementation unconditionally passes
    // CREATE_NO_WINDOW + STARTF_USESTDHANDLES, which Kontakt 8 refuses to
    // instantiate under (isolated and confirmed: identical code launched as
    // a normal process succeeds every time; via juce::ChildProcess it fails
    // every time).
    // EXPERIMENT: also try to break away from any Job Object this process
    // belongs to (e.g. a dev-tool sandbox), in case that -- not the window
    // station -- is what Kontakt's init reacts to. If the job doesn't
    // permit breakaway, CreateProcess just fails with this flag and we
    // retry without it.
    bool ok = CreateProcessW(nullptr, const_cast<LPWSTR>(commandLine.toWideCharPointer()),
                              nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT | CREATE_BREAKAWAY_FROM_JOB,
                              nullptr, nullptr, &startupInfo, &processInfo)
         != FALSE;
    if (!ok)
    {
        ok = CreateProcessW(nullptr, const_cast<LPWSTR>(commandLine.toWideCharPointer()),
                             nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT,
                             nullptr, nullptr, &startupInfo, &processInfo)
             != FALSE;
    }
    if (ok)
    {
        processStarted = true;
        CloseHandle(processInfo.hThread);
        processInfo.hThread = nullptr;
    }
    return ok;
#else
    juce::ignoreUnused(commandLine);
    return false;
#endif
}

void PluginServerProxy::terminateChildProcessIfRunning()
{
#if JUCE_WINDOWS
    if (!processStarted)
        return;

    if (WaitForSingleObject(processInfo.hProcess, 2000) == WAIT_TIMEOUT)
        TerminateProcess(processInfo.hProcess, 1);

    CloseHandle(processInfo.hProcess);
    processInfo = {};
    processStarted = false;
#endif
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
    // NOTE: this timeout applies to pipe *writes* too (JUCE shares one
    // value for both), and sendMessage() below is called from the real
    // audio thread every block. An infinite value here can hang the audio
    // thread forever if the child dies mid-write, which is worse than the
    // false-disconnect risk it was meant to fix -- renderNextBlock already
    // runs continuously once a device is open, so idle-looking pipes
    // should be rare in practice. Kept generous (well above one audio
    // block) rather than infinite.
    if (!connection->createPipe(pipeName, 4000))
    {
        onError("Failed to create IPC pipe for plugin server");
        return;
    }

    juce::String commandLine;
    commandLine << quoteIfNeeded(serverExe.getFullPathName())
                << " --pipe " << pipeName
                << " --plugin " << quoteIfNeeded(pluginFile.getFullPathName())
                << " --samplerate " << juce::String(sampleRate, 0)
                << " --blocksize " << juce::String(blockSize);

    if (!launchChildProcess(commandLine))
    {
        onError("Failed to launch AiTrackerPluginServer.exe");
        connection->disconnect();
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

    terminateChildProcessIfRunning();

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
