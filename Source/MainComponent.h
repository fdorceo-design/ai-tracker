#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"
#include "ApiServer.h"
#include "TrackerContentComponent.h"
#include "TempoMapColumn.h"

class MainComponent : public juce::Component,
                       private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Width of the left-edge BPM/time-signature strip, plus the outer
    // margin used around the whole layout -- exposed so Main.cpp can size
    // the default window width off "N tracks wide" without duplicating
    // these numbers.
    static constexpr int tempoColumnWidth = 56;
    static constexpr int outerMargin = 20;

private:
    void timerCallback() override;

    AudioEngine engine;
    Sequencer sequencer { engine };
    ApiServer apiServer { engine, sequencer };

    juce::Label titleLabel;
    juce::TextButton addTrackButton{"+ Add Track"};
    juce::TextButton playButton{"Play"};
    juce::TextButton pauseButton{"Pause"};
    juce::TextButton stopButton{"Stop"};
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;
    juce::Label positionLabel;
    juce::Label apiLabel;

    juce::Viewport trackerViewport;
    TrackerContentComponent trackerContent { engine, sequencer };
    TempoMapColumn tempoMapColumn { sequencer, trackerContent };
    // Floats over the top-right corner of the viewport (not part of its
    // scrolling content) to jump straight back to beat 0 vertically.
    juce::TextButton scrollToTopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
