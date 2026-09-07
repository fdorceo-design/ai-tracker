#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"
#include "ApiServer.h"
#include "TrackerContentComponent.h"
#include "TrackHeadersBar.h"
#include "TempoMapColumn.h"
#include "PlaybackMaskOverlay.h"

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
    void exportMidiClicked();

    AudioEngine engine;
    Sequencer sequencer { engine };
    ApiServer apiServer { engine, sequencer };

    juce::Label titleLabel;
    juce::TextButton addTrackButton{"+ Add Track"};
    juce::TextButton playButton{"Play"};
    juce::TextButton pauseButton{"Pause"};
    juce::TextButton stopButton{"Stop"};
    juce::TextButton panicButton{"Panic"};
    juce::TextButton exportMidiButton{"Export MIDI..."};
    std::unique_ptr<juce::FileChooser> exportFileChooser;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;
    juce::Label positionLabel;
    juce::Label apiLabel;

    // Header row (name/channel/load/editor/remove) in its own horizontal-
    // scroll-only viewport, fixed at the top -- stays visible while the
    // note grid below scrolls vertically. Its horizontal scroll position is
    // mirrored from trackerViewport's every tick so the columns stay
    // aligned; it never scrolls on its own.
    juce::Viewport headerViewport;
    TrackHeadersBar trackHeadersBar { engine };

    juce::Viewport trackerViewport;
    TrackerContentComponent trackerContent { engine, sequencer };
    TempoMapColumn tempoMapColumn { sequencer, trackerContent };
    // Floats over the top-right corner of the viewport (not part of its
    // scrolling content) to jump straight back to beat 0 vertically.
    juce::TextButton scrollToTopButton;

    // Dims everything except the currently-playing bar and the transport's
    // Pause/Stop buttons while playing. Added last / kept in front so it
    // draws over everything else.
    PlaybackMaskOverlay playbackMask { sequencer, trackerViewport, trackerContent, pauseButton, stopButton };

    // The playing bar last snapped to -- so the auto-scroll below only
    // jumps when it actually changes (a deliberate snap, not a smooth
    // follow), and resets when playback stops so the next play starts
    // with a fresh snap instead of assuming the old scroll position.
    int lastScrolledBar = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
