#include "MainComponent.h"

namespace
{
    constexpr int apiPort = 8080;
}

MainComponent::MainComponent()
{
    titleLabel.setText("AI Tracker", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(20.0f));
    addAndMakeVisible(titleLabel);

    addTrackButton.onClick = [this] { engine.addTrack({}); };
    addAndMakeVisible(addTrackButton);

    playButton.onClick = [this] { sequencer.play(); };
    addAndMakeVisible(playButton);

    stopButton.onClick = [this] { sequencer.stop(); };
    addAndMakeVisible(stopButton);

    positionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(positionLabel);

    apiLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(apiLabel);

    trackerViewport.setViewedComponent(&trackerContent, false);
    addAndMakeVisible(trackerViewport);
    trackerContent.refreshTracks();

    if (apiServer.start(apiPort))
        apiLabel.setText("API: http://127.0.0.1:" + juce::String(apiPort), juce::dontSendNotification);
    else
        apiLabel.setText("API failed to start", juce::dontSendNotification);

    setSize(900, 640);
    startTimerHz(10);
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

    auto topRow = area.removeFromTop(30);
    addTrackButton.setBounds(topRow.removeFromLeft(120));
    topRow.removeFromLeft(10);
    playButton.setBounds(topRow.removeFromLeft((topRow.getWidth() - 10) / 2));
    topRow.removeFromLeft(10);
    stopButton.setBounds(topRow);
    area.removeFromTop(10);

    positionLabel.setBounds(area.removeFromTop(24));
    apiLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    trackerViewport.setBounds(area);
}

void MainComponent::timerCallback()
{
    trackerContent.refreshTracks();

    juce::String txt = sequencer.isPlaying() ? "Playing" : "Stopped";
    txt << "  beat " << juce::String(sequencer.getPositionBeats(), 2)
        << "  bpm " << juce::String(sequencer.getBpm(), 0);
    positionLabel.setText(txt, juce::dontSendNotification);
}
