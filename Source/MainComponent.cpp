#include "MainComponent.h"
#include <algorithm>

namespace
{
    constexpr int apiPort = 8080;
    constexpr int trackRowHeight = 34;
}

MainComponent::MainComponent()
{
    titleLabel.setText("AI Tracker", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(20.0f));
    addAndMakeVisible(titleLabel);

    addTrackButton.onClick = [this] { addTrackClicked(); };
    addAndMakeVisible(addTrackButton);

    demoButton.onClick = [this] { addDemoTrackClicked(); };
    addAndMakeVisible(demoButton);

    playButton.onClick = [this] { sequencer.play(); };
    addAndMakeVisible(playButton);

    stopButton.onClick = [this] { sequencer.stop(); };
    addAndMakeVisible(stopButton);

    positionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(positionLabel);

    apiLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(apiLabel);

    tracksViewport.setViewedComponent(&tracksContainer, false);
    addAndMakeVisible(tracksViewport);

    if (apiServer.start(apiPort))
        apiLabel.setText("API: http://127.0.0.1:" + juce::String(apiPort), juce::dontSendNotification);
    else
        apiLabel.setText("API failed to start", juce::dontSendNotification);

    setSize(560, 480);
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
    addTrackButton.setBounds(topRow.removeFromLeft(topRow.getWidth() / 2).reduced(4, 0));
    demoButton.setBounds(topRow.reduced(4, 0));
    area.removeFromTop(10);

    auto transportRow = area.removeFromTop(30);
    playButton.setBounds(transportRow.removeFromLeft(transportRow.getWidth() / 2).reduced(4, 0));
    stopButton.setBounds(transportRow.reduced(4, 0));
    area.removeFromTop(10);

    positionLabel.setBounds(area.removeFromTop(24));
    apiLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    tracksViewport.setBounds(area);
    relayoutTracks();
}

void MainComponent::relayoutTracks()
{
    const int width = tracksViewport.getWidth() - tracksViewport.getScrollBarThickness();
    const int height = juce::jmax(trackRowHeight, (int) trackRows.size() * trackRowHeight);
    tracksContainer.setSize(width, height);

    int y = 0;
    for (auto& row : trackRows)
    {
        row->setBounds(0, y, width, trackRowHeight);
        y += trackRowHeight;
    }
}

void MainComponent::addTrackClicked()
{
    const int id = engine.addTrack({});
    auto row = std::make_unique<TrackRowComponent>(engine, id, [this, id] { removeTrack(id); });
    tracksContainer.addAndMakeVisible(*row);
    trackRows.push_back(std::move(row));
    relayoutTracks();
}

void MainComponent::addDemoTrackClicked()
{
    const int id = engine.addTrack("Demo");
    auto row = std::make_unique<TrackRowComponent>(engine, id, [this, id] { removeTrack(id); });
    tracksContainer.addAndMakeVisible(*row);
    trackRows.push_back(std::move(row));
    relayoutTracks();

    // A one-bar C major arpeggio to prove the sequencer -> track path once
    // a plugin is loaded into this track via its row's Load button.
    const int pitches[] = { 60, 64, 67, 72 };
    for (int i = 0; i < 4; ++i)
        sequencer.addNote(id, pitches[i], 0.85f, (double) i, 0.9);
    sequencer.setLoop(true, 0.0, 4.0);
}

void MainComponent::removeTrack(int trackId)
{
    engine.removeTrack(trackId);
    trackRows.erase(std::remove_if(trackRows.begin(), trackRows.end(),
                                    [trackId](const std::unique_ptr<TrackRowComponent>& r)
                                    { return r->getTrackId() == trackId; }),
                     trackRows.end());
    relayoutTracks();
}

void MainComponent::timerCallback()
{
    juce::String txt = sequencer.isPlaying() ? "Playing" : "Stopped";
    txt << "  beat " << juce::String(sequencer.getPositionBeats(), 2)
        << "  bpm " << juce::String(sequencer.getBpm(), 0);
    positionLabel.setText(txt, juce::dontSendNotification);
}
