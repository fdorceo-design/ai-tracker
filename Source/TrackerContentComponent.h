#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"
#include "TrackRowComponent.h"
#include "TrackEventListComponent.h"
#include "BarBand.h"

// Combines per-track headers (name/load/editor/remove) with a sparse,
// variable-length event list per track below them -- rows exist only where
// notes exist (Music-kun/Recomposer style). A shared bar-band layout is
// computed once from every track's notes so bar boundaries line up at the
// same pixel height across every column; the boundary lines themselves are
// drawn here (once, spanning the full width) rather than per-column.
class TrackerContentComponent : public juce::Component
{
public:
    TrackerContentComponent(AudioEngine& engineIn, Sequencer& sequencerIn);

    // Reconciles headers and event lists against the engine's current
    // tracks (tracks can appear outside the UI, e.g. via the API or MIDI
    // import), recomputes the shared bar layout, then resizes to fit.
    void refreshTracks();

    void resized() override;
    void paintOverChildren(juce::Graphics& g) override;

    // Absolute Y pixel (in this component's coordinate space) of the bar
    // containing the given beat, for scrolling the playhead into view.
    // Approximate: resolves to the bar's shared band, not the exact row,
    // since rows are a sparse per-track layout rather than a fixed timeline.
    int getYForBeat(double beat) const;

private:
    static constexpr int headerHeight = 152;
    static constexpr int columnWidth = 280;

    AudioEngine& engine;
    Sequencer& sequencer;
    std::vector<int> trackOrder;
    std::vector<std::unique_ptr<TrackRowComponent>> headers;
    std::vector<std::unique_ptr<TrackEventListComponent>> eventLists;
    std::vector<BarBand> barBands;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackerContentComponent)
};
