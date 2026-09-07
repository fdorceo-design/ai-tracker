#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"
#include "TrackRowComponent.h"
#include "TrackerGridComponent.h"

// Combines a row of per-track headers (name/load/editor/remove, reusing
// TrackRowComponent in its narrow layout) with the step grid below it.
// Sized to its content so a Viewport can scroll it in both directions.
class TrackerContentComponent : public juce::Component
{
public:
    TrackerContentComponent(AudioEngine& engineIn, Sequencer& sequencerIn);

    // Reconciles the header list against the engine's current tracks
    // (tracks can appear outside the UI, e.g. via the API or MIDI import),
    // then resizes this component to fit.
    void refreshTracks();

    void resized() override;

private:
    static constexpr int headerHeight = 118;

    AudioEngine& engine;
    Sequencer& sequencer;
    std::vector<int> trackOrder;
    std::vector<std::unique_ptr<TrackRowComponent>> headers;
    TrackerGridComponent grid;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackerContentComponent)
};
