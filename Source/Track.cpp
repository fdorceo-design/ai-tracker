#include "Track.h"
#include <juce_gui_extra/juce_gui_extra.h>

class Track::PluginWindow : public juce::DocumentWindow
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

Track::Track(int idIn, juce::String nameIn) : id(idIn), name(std::move(nameIn))
{
}

Track::~Track()
{
    hideEditorWindow();
}

void Track::loadPlugin(juce::AudioPluginFormatManager& formatManager, const juce::File& file,
                        double sampleRate, int blockSize, const std::function<void(juce::String)>& onError)
{
    juce::OwnedArray<juce::PluginDescription> descriptions;
    for (auto* format : formatManager.getFormats())
        format->findAllTypesForFile(descriptions, file.getFullPathName());

    if (descriptions.isEmpty())
    {
        onError("No plugin found in " + file.getFullPathName());
        return;
    }

    juce::String errorMessage;
    auto instance = formatManager.createPluginInstance(*descriptions[0], sampleRate, blockSize, errorMessage);
    if (instance == nullptr)
    {
        onError(errorMessage);
        return;
    }

    hideEditorWindow();
    processor = std::move(instance);
    prepareToPlay(sampleRate, blockSize);
}

juce::String Track::getPluginName() const
{
    return processor != nullptr ? processor->getName() : juce::String();
}

void Track::showEditorWindow()
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

void Track::hideEditorWindow()
{
    editorWindow = nullptr;
}

void Track::prepareToPlay(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
    midiCollector.reset(sampleRate);

    if (processor != nullptr)
    {
        processor->setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor->prepareToPlay(sampleRate, blockSize);
    }
}

void Track::releaseResources()
{
    if (processor != nullptr)
        processor->releaseResources();
}

void Track::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int numSamples)
{
    if (processor == nullptr)
        return;

    const int numChannels = outputBuffer.getNumChannels();
    scratchBuffer.setSize(numChannels, numSamples, false, false, true);
    scratchBuffer.clear();

    juce::MidiBuffer midi;
    midiCollector.removeNextBlockOfMessages(midi, numSamples);

    processor->processBlock(scratchBuffer, midi);

    for (int ch = 0; ch < numChannels; ++ch)
        outputBuffer.addFrom(ch, 0, scratchBuffer, ch, 0, numSamples);
}

void Track::sendNoteOn(int channel, int noteNumber, float velocity)
{
    auto msg = juce::MidiMessage::noteOn(channel, noteNumber, velocity);
    msg.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(msg);
}

void Track::sendNoteOff(int channel, int noteNumber)
{
    auto msg = juce::MidiMessage::noteOff(channel, noteNumber);
    msg.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(msg);
}
