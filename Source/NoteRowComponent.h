#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Sequencer.h"

// One editable note row: beat position, length, pitch and velocity as plain
// numbers (AI-authored notes rarely land on a musician-friendly grid
// position, so numbers are the honest representation) plus a delete button.
class NoteRowComponent : public juce::Component
{
public:
    NoteRowComponent(Sequencer& sequencerIn, int noteIdIn);

    int getNoteId() const { return noteId; }
    // barIndex is precomputed by the caller (TrackEventListComponent),
    // which already needs it for row layout -- avoids each row re-deriving
    // it via another Sequencer::getBarIndexForBeat call/sort. showBarNumber
    // is false for every row after the first in a given bar, so the number
    // only appears once per bar instead of repeating on every row.
    void refresh(const SequencerNote& note, int barIndex, bool showBarNumber);
    void setHighlighted(bool shouldHighlight);
    // For a lightweight per-tick highlight-only pass during playback, so
    // that doesn't require re-walking/re-sorting every track's notes (see
    // TrackEventListComponent::updateHighlights).
    const SequencerNote& getCachedNote() const { return cachedNote; }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    Sequencer& sequencer;
    int noteId;
    // barLabel/beatLabel show and edit a bar-relative position (1-indexed
    // bar number, beat resets to 1 at each bar line) rather than the
    // underlying absolute/running startBeat -- easier to read, though the
    // stored data is still a single absolute beat.
    juce::Label barLabel, beatLabel, lengthLabel, velocityLabel;
    juce::ComboBox pitchBox;
    juce::TextButton deleteButton { "x" };
    bool highlighted = false;
    SequencerNote cachedNote;
};
