#include "PluginHost.h"

class PluginHost::PluginWindow : public juce::DocumentWindow
{
public:
    PluginWindow(juce::AudioProcessorEditor* editor, std::function<void()> onCloseIn)
        : DocumentWindow(editor->getName(),
                          juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour(juce::ResizableWindow::backgroundColourId),
                          DocumentWindow::closeButton)
        , onClose(std::move(onCloseIn))
    {
        setUsingNativeTitleBar(true);
        setContentOwned(editor, true);
        centreWithSize(getWidth(), getHeight());
        setResizable(false, false);
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        if (onClose)
            onClose();
    }

private:
    std::function<void()> onClose;
};

PluginHost::PluginHost()
{
    formatManager.addDefaultFormats();
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(&player);
}

PluginHost::~PluginHost()
{
    hideEditorWindow();
    deviceManager.removeAudioCallback(&player);
    player.setProcessor(nullptr);
}

void PluginHost::loadPlugin(const juce::File& file, std::function<void(juce::String)> onError)
{
    juce::OwnedArray<juce::PluginDescription> descriptions;
    for (auto* format : formatManager.getFormats())
        format->findAllTypesForFile(descriptions, file.getFullPathName());

    if (descriptions.isEmpty())
    {
        onError("No plugin found in " + file.getFullPathName());
        return;
    }

    const auto setup = deviceManager.getAudioDeviceSetup();
    juce::String errorMessage;
    auto instance = formatManager.createPluginInstance(*descriptions[0],
                                                         setup.sampleRate > 0 ? setup.sampleRate : 44100.0,
                                                         setup.bufferSize > 0 ? setup.bufferSize : 512,
                                                         errorMessage);
    if (instance == nullptr)
    {
        onError(errorMessage);
        return;
    }

    setProcessor(std::move(instance));
}

void PluginHost::closePlugin()
{
    setProcessor(nullptr);
}

void PluginHost::setProcessor(std::unique_ptr<juce::AudioPluginInstance> newProcessor)
{
    hideEditorWindow();
    player.setProcessor(nullptr);
    processor = std::move(newProcessor);
    player.setProcessor(processor.get());
}

void PluginHost::hideEditorWindow()
{
    editorWindow = nullptr;
}

juce::String PluginHost::getPluginName() const
{
    return processor != nullptr ? processor->getName() : juce::String();
}

void PluginHost::showEditorWindow()
{
    if (processor == nullptr)
        return;

    if (editorWindow != nullptr)
    {
        editorWindow->toFront(true);
        return;
    }

    if (auto* editor = processor->createEditorIfNeeded())
        editorWindow = std::make_unique<PluginWindow>(editor, [this] { hideEditorWindow(); });
}

void PluginHost::sendNoteOn(int channel, int noteNumber, float velocity)
{
    auto msg = juce::MidiMessage::noteOn(channel, noteNumber, velocity);
    msg.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    player.getMidiMessageCollector().addMessageToQueue(msg);
}

void PluginHost::sendNoteOff(int channel, int noteNumber)
{
    auto msg = juce::MidiMessage::noteOff(channel, noteNumber);
    msg.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    player.getMidiMessageCollector().addMessageToQueue(msg);
}
