#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginServerProtocol.h"
#include "PluginServerConnection.h"

#if JUCE_WINDOWS
 #include <windows.h>
#endif

namespace
{
#if JUCE_WINDOWS
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

        connection = std::make_unique<PluginServerConnection>();
        connection->onDisconnect = [this] { quit(); };
        connection->onMessage = [this](const juce::MemoryBlock& mb) { handleMessage(mb); };

        // See the matching comment in PluginServerProxy::loadPlugin: this
        // timeout applies to writes too (this side's audioResult sends),
        // so it must stay finite rather than infinite.
        if (pipeName.isEmpty() || !connection->connectToPipe(pipeName, 4000))
        {
            quit();
            return;
        }

        formatManager.addDefaultFormats();

        juce::OwnedArray<juce::PluginDescription> descriptions;
        for (auto* format : formatManager.getFormats())
            format->findAllTypesForFile(descriptions, pluginPath);

        juce::String error;
        juce::AudioPluginInstance* raw = nullptr;
        bool crashed = false;

        if (descriptions.isEmpty())
            error = "No plugin found in " + pluginPath;
        else
            raw = createInstanceGuarded(&formatManager, descriptions[0], sampleRate, blockSize, &error, &crashed);

        if (crashed)
            error = "Plugin crashed while loading";

        if (raw != nullptr)
        {
            processor.reset(raw);
            processor->setRateAndBufferSizeDetails(sampleRate, blockSize);
            processor->prepareToPlay(sampleRate, blockSize);
            connection->sendMessage(PluginServerProtocol::buildLoadResult(true, processor->getName(), {}));
        }
        else
        {
            connection->sendMessage(PluginServerProtocol::buildLoadResult(false, {}, error));
        }
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
    std::unique_ptr<PluginServerConnection> connection;
    juce::AudioPluginFormatManager formatManager;
    std::unique_ptr<juce::AudioPluginInstance> processor;
    std::unique_ptr<PluginServerWindow> editorWindow;
    juce::AudioBuffer<float> scratch;
};

START_JUCE_APPLICATION(PluginServerApp)
