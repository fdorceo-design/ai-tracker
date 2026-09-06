#include "MainComponent.h"

namespace
{
    constexpr int midiChannel = 1;
    constexpr int testNoteNumber = 60; // C4
    constexpr int apiPort = 8080;
}

MainComponent::MainComponent()
{
    titleLabel.setText("AI Tracker", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(20.0f));
    addAndMakeVisible(titleLabel);

    statusLabel.setText("No plugin loaded", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    apiLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(apiLabel);

    loadButton.onClick = [this] { loadPluginClicked(); };
    addAndMakeVisible(loadButton);

    editorButton.onClick = [this] { pluginHost.showEditorWindow(); };
    editorButton.setEnabled(false);
    addAndMakeVisible(editorButton);

    testNoteButton.onClick = [this] { testNoteClicked(); };
    testNoteButton.setEnabled(false);
    addAndMakeVisible(testNoteButton);

    demoNotesButton.onClick = [this] { addDemoNotesClicked(); };
    addAndMakeVisible(demoNotesButton);

    playButton.onClick = [this] { sequencer.play(); };
    addAndMakeVisible(playButton);

    stopButton.onClick = [this] { sequencer.stop(); };
    addAndMakeVisible(stopButton);

    if (apiServer.start(apiPort))
        apiLabel.setText("API: http://127.0.0.1:" + juce::String(apiPort), juce::dontSendNotification);
    else
        apiLabel.setText("API failed to start", juce::dontSendNotification);

    setSize(500, 420);
}

MainComponent::~MainComponent()
{
    stopTimer();
    apiServer.stop();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    titleLabel.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);
    loadButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);
    editorButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);
    testNoteButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);
    demoNotesButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);

    auto transportRow = area.removeFromTop(30);
    playButton.setBounds(transportRow.removeFromLeft(transportRow.getWidth() / 2).reduced(4, 0));
    stopButton.setBounds(transportRow.reduced(4, 0));
    area.removeFromTop(10);

    statusLabel.setBounds(area.removeFromTop(24));
    apiLabel.setBounds(area.removeFromTop(24));
}

void MainComponent::loadPluginClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select a VST3 plugin",
        juce::File("C:\\Program Files\\Common Files\\VST3"),
        "*.vst3");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file == juce::File())
                return;

            statusLabel.setText("Loading " + file.getFileNameWithoutExtension() + "...",
                                 juce::dontSendNotification);

            pluginHost.loadPlugin(file, [this](juce::String error)
            {
                statusLabel.setText("Failed: " + error, juce::dontSendNotification);
                editorButton.setEnabled(false);
                testNoteButton.setEnabled(false);
            });

            if (pluginHost.isPluginLoaded())
            {
                statusLabel.setText("Loaded: " + pluginHost.getPluginName(), juce::dontSendNotification);
                editorButton.setEnabled(true);
                testNoteButton.setEnabled(true);
            }
        });
}

void MainComponent::testNoteClicked()
{
    if (!pluginHost.isPluginLoaded())
        return;

    pluginHost.sendNoteOn(midiChannel, testNoteNumber, 0.9f);
    noteIsOn = true;
    startTimer(800);
}

void MainComponent::addDemoNotesClicked()
{
    sequencer.clearNotes();
    // A simple one-bar C major arpeggio to prove the sequencer -> plugin path.
    const int pitches[] = { 60, 64, 67, 72 };
    for (int i = 0; i < 4; ++i)
        sequencer.addNote(pitches[i], 0.85f, (double) i, 0.9);
    sequencer.setLoop(true, 0.0, 4.0);
    statusLabel.setText("Demo notes added (4 beats, looping)", juce::dontSendNotification);
}

void MainComponent::timerCallback()
{
    stopTimer();
    if (noteIsOn)
    {
        pluginHost.sendNoteOff(midiChannel, testNoteNumber);
        noteIsOn = false;
    }
}
