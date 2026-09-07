#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginServerProtocol.h"
#include "PluginServerConnection.h"

#if JUCE_WINDOWS
 #include <windows.h>
 #include <DbgHelp.h>
 #pragma comment(lib, "dbghelp.lib")
#endif

namespace
{
#if JUCE_WINDOWS
    // Process-wide (any thread, unlike __try/__except which is per-call-site
    // and per-thread) so it can catch Kontakt et al. crashing on a thread
    // they spawn internally during their own instantiation -- exactly the
    // case the SEH guard below cannot reach. Kept to raw Win32 calls (no
    // JUCE/STL allocation) since it runs in a crash context.
    LONG WINAPI writeMinidumpAndContinue(EXCEPTION_POINTERS* exceptionPointers)
    {
        const DWORD pid = GetCurrentProcessId();

        wchar_t modulePath[MAX_PATH]{};
        GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(modulePath, L'\\');
        if (lastSlash != nullptr)
            *lastSlash = 0;

        wchar_t dumpPath[MAX_PATH]{};
        wsprintfW(dumpPath, L"%s\\PluginServerCrash_%lu.dmp", modulePath, pid);

        HANDLE file = CreateFileW(dumpPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE)
        {
            MINIDUMP_EXCEPTION_INFORMATION mei;
            mei.ThreadId = GetCurrentThreadId();
            mei.ExceptionPointers = exceptionPointers;
            mei.ClientPointers = FALSE;

            MiniDumpWriteDump(GetCurrentProcess(), pid, file,
                               (MINIDUMP_TYPE) (MiniDumpWithDataSegs | MiniDumpWithThreadInfo),
                               exceptionPointers != nullptr ? &mei : nullptr, nullptr, nullptr);
            CloseHandle(file);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    int filterHardCrash(unsigned int code)
    {
        return (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_STACK_OVERFLOW)
                   ? EXCEPTION_EXECUTE_HANDLER
                   : EXCEPTION_CONTINUE_SEARCH;
    }

    // May throw/allocate freely -- this frame is only ever called *from*
    // createInstanceGuarded below, never contains a __try itself.
    juce::AudioPluginInstance* doCreatePluginInstance(juce::AudioPluginFormatManager* formatManager,
                                                       const juce::PluginDescription* description,
                                                       double sampleRate, int blockSize, juce::String* outError)
    {
        auto instance = formatManager->createPluginInstance(*description, sampleRate, blockSize, *outError);
        return instance.release();
    }

    // Catches a plugin crashing outright on THIS thread during instantiation
    // (some plugins spawn their own worker threads too, which this cannot
    // catch -- that's exactly why the plugin runs in its own process rather
    // than relying on this alone). MSVC forbids C++ objects with
    // destructors in a function containing __try, so this frame is kept to
    // raw pointers/PODs only.
    juce::AudioPluginInstance* createInstanceGuarded(juce::AudioPluginFormatManager* formatManager,
                                                      const juce::PluginDescription* description,
                                                      double sampleRate, int blockSize,
                                                      juce::String* outError, bool* crashed)
    {
        *crashed = false;
        juce::AudioPluginInstance* result = nullptr;

        __try
        {
            result = doCreatePluginInstance(formatManager, description, sampleRate, blockSize, outError);
        }
        __except (filterHardCrash(GetExceptionCode()))
        {
            result = nullptr;
            *crashed = true;
        }

        return result;
    }
#else
    juce::AudioPluginInstance* createInstanceGuarded(juce::AudioPluginFormatManager* formatManager,
                                                      const juce::PluginDescription* description,
                                                      double sampleRate, int blockSize,
                                                      juce::String* outError, bool* crashed)
    {
        *crashed = false;
        auto instance = formatManager->createPluginInstance(*description, sampleRate, blockSize, *outError);
        return instance.release();
    }
#endif
}

class PluginServerWindow : public juce::DocumentWindow
{
public:
    explicit PluginServerWindow(juce::AudioProcessorEditor* editor)
        : DocumentWindow(editor->getName(),
                          juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour(juce::ResizableWindow::backgroundColourId),
                          DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(editor, true);
        centreWithSize(getWidth(), getHeight());
        setResizable(false, false);
        setVisible(true);
    }

    void closeButtonPressed() override { setVisible(false); }
};

class PluginServerApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "AiTrackerPluginServer"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
#if JUCE_WINDOWS
        SetUnhandledExceptionFilter(writeMinidumpAndContinue);
#endif

        const auto args = juce::StringArray::fromTokens(commandLine, true);

        juce::String pipeName, pluginPath;
        double sampleRate = 44100.0;
        int blockSize = 512;

        for (int i = 0; i + 1 < args.size(); ++i)
        {
            if (args[i] == "--pipe") pipeName = args[i + 1].unquoted();
            else if (args[i] == "--plugin") pluginPath = args[i + 1].unquoted();
            else if (args[i] == "--samplerate") sampleRate = args[i + 1].getDoubleValue();
            else if (args[i] == "--blocksize") blockSize = args[i + 1].getIntValue();
        }

        // One log file per child process (pid-suffixed), flushed on every
        // call, so if this process dies mid-load the last line written is
        // the last thing it was doing -- the crash itself gives no other
        // diagnostic (no WER report has been produced for any of these).
        const auto pidForLog = (unsigned long) GetCurrentProcessId();
        const auto logFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                                  .getSiblingFile("PluginServerLog_" + juce::String(pidForLog) + ".txt");
        fileLogger.reset(new juce::FileLogger(logFile, "AiTrackerPluginServer log"));

        log("start: plugin=\"" + pluginPath + "\" pipe=\"" + pipeName + "\" sr=" + juce::String(sampleRate)
            + " bs=" + juce::String(blockSize));

        this->pipeName = pipeName;

        formatManager.addDefaultFormats();
        log("addDefaultFormats: done");

        // The pipe/PluginServerConnection is deliberately not created until
        // after the plugin has finished loading (see connectAndSend) --
        // one less thing running concurrently with plugin instantiation.
        // The actual root cause of Kontakt/BM-RICO/Synthesizer V failing to
        // load was traced to the *parent* process: PluginServerProxy was
        // launching this exe via juce::ChildProcess, which unconditionally
        // passes CREATE_NO_WINDOW: some copy-protected plugins refuse to
        // instantiate under a windowless/headless-looking process. Fixed on
        // the proxy side with a raw CreateProcess call instead.
        startLoadingPlugin(pluginPath, sampleRate, blockSize);
    }

    void log(const juce::String& msg)
    {
        if (fileLogger != nullptr)
            fileLogger->logMessage("[thread=" + juce::String((juce::pointer_sized_int) GetCurrentThreadId())
                                    + "] " + msg);
    }

    void startLoadingPlugin(const juce::String& pluginPath, double sampleRate, int blockSize)
    {
        juce::OwnedArray<juce::PluginDescription> descriptions;
        log("findAllTypesForFile: before");
        for (auto* format : formatManager.getFormats())
            format->findAllTypesForFile(descriptions, pluginPath);
        log("findAllTypesForFile: after, found " + juce::String(descriptions.size()));

        if (descriptions.isEmpty())
        {
            log("no PluginDescription found, aborting");
            connectAndSend(PluginServerProtocol::buildLoadResult(false, {}, "No plugin found in " + pluginPath));
            return;
        }

        juce::String error;
        bool crashed = false;
        log("createPluginInstance (guarded): before");
        std::unique_ptr<juce::AudioPluginInstance> instance(
            createInstanceGuarded(&formatManager, descriptions[0], sampleRate, blockSize, &error, &crashed));
        log(juce::String("createPluginInstance (guarded): after, instance=")
            + (instance != nullptr ? "non-null" : "null") + " crashed=" + (crashed ? "true" : "false") + " error=\""
            + error + "\"");

        if (crashed)
            error = "Plugin crashed while loading";

        finishLoadingPlugin(std::move(instance), error, sampleRate, blockSize);
    }

    void finishLoadingPlugin(std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error,
                              double sampleRate, int blockSize)
    {
        if (instance == nullptr)
        {
            connectAndSend(PluginServerProtocol::buildLoadResult(false, {}, error));
            return;
        }

        processor = std::move(instance);
        log("setRateAndBufferSizeDetails: before");
        processor->setRateAndBufferSizeDetails(sampleRate, blockSize);
        log("setRateAndBufferSizeDetails: after");
        log("prepareToPlay: before");
        processor->prepareToPlay(sampleRate, blockSize);
        log("prepareToPlay: after");
        connectAndSend(PluginServerProtocol::buildLoadResult(true, processor->getName(), {}));
    }

    // Only now (load already finished, success or failure) does a
    // PluginServerConnection -- and its background ConnectionThread -- get
    // created at all.
    void connectAndSend(const juce::MemoryBlock& resultMessage)
    {
        connection = std::make_unique<PluginServerConnection>();
        connection->onDisconnect = [this] { log("connection lost -> quit()"); quit(); };
        connection->onMessage = [this](const juce::MemoryBlock& mb) { handleMessage(mb); };

        log("connectToPipe: before");
        if (pipeName.isEmpty() || !connection->connectToPipe(pipeName, 4000))
        {
            log("connectToPipe: failed, quitting");
            quit();
            return;
        }
        log("connectToPipe: connected");

        log("sendMessage(loadResult): before");
        connection->sendMessage(resultMessage);
        log("sendMessage(loadResult): after");
    }

    void shutdown() override
    {
        editorWindow = nullptr;
        if (processor != nullptr)
            processor->releaseResources();
        processor.reset();
        connection.reset();
    }

    void handleMessage(const juce::MemoryBlock& mb)
    {
        using namespace PluginServerProtocol;

        switch (readType(mb))
        {
            case MessageType::processBlock:
            {
                int numSamples = 0;
                juce::MidiBuffer midi;
                parseProcessBlock(mb, numSamples, midi);

                scratch.setSize(2, numSamples, false, false, true);
                scratch.clear();

                if (processor != nullptr)
                    processor->processBlock(scratch, midi);

                if (connection != nullptr)
                    connection->sendMessage(buildAudioResult(scratch));
                break;
            }

            case MessageType::showEditor:
            {
                if (processor == nullptr)
                    break;

                if (editorWindow != nullptr)
                    editorWindow->toFront(true);
                else if (auto* editor = processor->createEditorIfNeeded())
                    editorWindow = std::make_unique<PluginServerWindow>(editor);
                break;
            }

            case MessageType::shutdown:
                quit();
                break;

            default:
                break;
        }
    }

private:
    std::unique_ptr<juce::FileLogger> fileLogger;
    juce::String pipeName;
    std::unique_ptr<PluginServerConnection> connection;
    juce::AudioPluginFormatManager formatManager;
    std::unique_ptr<juce::AudioPluginInstance> processor;
    std::unique_ptr<PluginServerWindow> editorWindow;
    juce::AudioBuffer<float> scratch;
};

START_JUCE_APPLICATION(PluginServerApp)
