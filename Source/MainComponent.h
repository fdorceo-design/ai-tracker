#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"
#include "ApiServer.h"
#include "TrackRowComponent.h"

class MainComponent : public juce::Component,
                       private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void addTrackClicked();
    void addDemoTrackClicked();
    void removeTrack(int trackId);
    void relayoutTracks();
    void timerCallback() override;

    AudioEngine engine;
    Sequencer sequencer { engine };
    ApiServer apiServer { engine, sequencer };

    juce::Label titleLabel;
    juce::TextButton addTrackButton{"+ Add Track"};
    juce::TextButton demoButton{"Add Demo Track"};
    juce::TextButton playButton{"Play"};
    juce::TextButton stopButton{"Stop"};
    juce::Label positionLabel;
    juce::Label apiLabel;

    juce::Viewport tracksViewport;
    juce::Component tracksContainer;
    std::vector<std::unique_ptr<TrackRowComponent>> trackRows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
