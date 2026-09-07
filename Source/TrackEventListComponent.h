#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Sequencer.h"
#include "NoteRowComponent.h"
#include "BarBand.h"

// A single track's notes as a sparse list -- one row per note that actually
// exists, sorted by beat. Rows are positioned according to a shared
// bar-band layout (computed once by the parent from every track's notes),
// so bar boundaries land at the same pixel height in every track's column
// even though each column has a different number of notes.
class TrackEventListComponent : public juce::Component
{
public:
    static constexpr int rowHeight = 22;
    static constexpr int captionHeight = 28;

    TrackEventListComponent(Sequencer& sequencerIn, int trackIdIn);

    int getTrackId() const { return trackId; }

    // Reconciles rows against this track's current notes (matched by note
    // id, so an in-progress edit in an existing row survives) and places
    // each one at its bar's shared band offset.
    void applyBarLayout(const std::vector<BarBand>& bands);

    void resized() override;

private:
    void addNoteClicked(double lengthBeats);

    Sequencer& sequencer;
    int trackId;
    std::vector<std::unique_ptr<NoteRowComponent>> rows;
    juce::TextButton addQuarterButton { "+ 1/4" };
    juce::TextButton addEighthButton { "+ 1/8" };
    juce::TextButton addSixteenthButton { "+ 1/16" };
    juce::Label captionLabel;
};
