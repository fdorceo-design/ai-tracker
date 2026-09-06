#include "MainComponent.h"

namespace
{
    constexpr int midiChannel = 1;
    constexpr int testNoteNumber = 60; // C4
}

MainComponent::MainComponent()
{
    titleLabel.setText("AI Tracker - VSTi host test", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(20.0f));
    addAndMakeVisible(titleLabel);

    statusLabel.setText("No plugin loaded", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    loadButton.onClick = [this] { loadPluginClicked(); };
    addAndMakeVisible(loadButton);

    editorButton.onClick = [this] { pluginHost.showEditorWindow(); };
    editorButton.setEnabled(false);
    addAndMakeVisible(editorButton);

    testNoteButton.onClick = [this] { testNoteClicked(); };
    testNoteButton.setEnabled(false);
    addAndMakeVisible(testNoteButton);

    setSize(500, 300);
}

MainComponent::~MainComponent()
{
    stopTimer();
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
    statusLabel.setBounds(area.removeFromTop(30));
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

void MainComponent::timerCallback()
{
    stopTimer();
    if (noteIsOn)
    {
        pluginHost.sendNoteOff(midiChannel, testNoteNumber);
        noteIsOn = false;
    }
}
