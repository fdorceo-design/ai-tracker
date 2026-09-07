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
    void refresh(const SequencerNote& note);
    void setHighlighted(bool shouldHighlight);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    Sequencer& sequencer;
    int noteId;
    juce::Label beatLabel, lengthLabel, velocityLabel;
    juce::ComboBox pitchBox;
    juce::TextButton deleteButton { "x" };
    bool highlighted = false;
};
