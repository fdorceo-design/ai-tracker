#include "NoteRowComponent.h"
#include <cmath>

namespace
{
    double parseDouble(const juce::String& s, double fallback)
    {
        const auto cleaned = s.retainCharacters("0123456789.-");
        return cleaned.isEmpty() ? fallback : cleaned.getDoubleValue();
    }

    int parseInt(const juce::String& s, int fallback)
    {
        const auto cleaned = s.retainCharacters("0123456789-");
        return cleaned.isEmpty() ? fallback : cleaned.getIntValue();
    }

    void makeEditable(juce::Label& label)
    {
        label.setEditable(false, true, false);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::backgroundColourId, juce::Colour(0xff306230).withAlpha(0.6f));
        label.setFont(juce::Font(juce::FontOptions(12.0f)));
    }
}

NoteRowComponent::NoteRowComponent(Sequencer& sequencerIn, int noteIdIn)
    : sequencer(sequencerIn), noteId(noteIdIn)
{
    makeEditable(barLabel);
    barLabel.onTextChange = [this]
    {
        const int newBarIndex = juce::jmax(0, parseInt(barLabel.getText(), 1) - 1);
        const int oldBarIndex = sequencer.getBarIndexForBeat(cachedNote.startBeat);
        const double intraBarOffset = cachedNote.startBeat - sequencer.getBarStartBeat(oldBarIndex);
        sequencer.setNoteBeat(noteId, sequencer.getBarStartBeat(newBarIndex) + intraBarOffset);
    };
    addAndMakeVisible(barLabel);

    makeEditable(beatLabel);
    beatLabel.onTextChange = [this]
    {
        const double relBeat = juce::jmax(0.0, parseDouble(beatLabel.getText(), 1.0) - 1.0);
        const int barIndex = sequencer.getBarIndexForBeat(cachedNote.startBeat);
        sequencer.setNoteBeat(noteId, sequencer.getBarStartBeat(barIndex) + relBeat);
    };
    addAndMakeVisible(beatLabel);

    makeEditable(lengthLabel);
    lengthLabel.onTextChange = [this]
    { sequencer.setNoteLength(noteId, juce::jmax(0.001, parseDouble(lengthLabel.getText(), 1.0))); };
    addAndMakeVisible(lengthLabel);

    for (int p = 0; p <= 127; ++p)
        pitchBox.addItem(juce::String(p) + " " + juce::MidiMessage::getMidiNoteName(p, true, true, 4), p + 1);
    pitchBox.onChange = [this]
    {
        const int selectedId = pitchBox.getSelectedId();
        if (selectedId > 0)
            sequencer.setNotePitch(noteId, selectedId - 1);
    };
    addAndMakeVisible(pitchBox);

    makeEditable(velocityLabel);
    velocityLabel.onTextChange = [this]
    {
        const int v = juce::jlimit(0, 127, parseInt(velocityLabel.getText(), 100));
        sequencer.setNoteVelocity(noteId, (float) v / 127.0f);
    };
    addAndMakeVisible(velocityLabel);

    deleteButton.onClick = [this] { sequencer.removeNote(noteId); };
    addAndMakeVisible(deleteButton);
}

void NoteRowComponent::refresh(const SequencerNote& note, int barIndex, bool showBarNumber)
{
    cachedNote = note;
    const double relBeat = note.startBeat - sequencer.getBarStartBeat(barIndex) + 1.0;

    if (barLabel.getCurrentTextEditor() == nullptr)
        barLabel.setText(showBarNumber ? juce::String(barIndex + 1) : juce::String(), juce::dontSendNotification);
    if (beatLabel.getCurrentTextEditor() == nullptr)
        beatLabel.setText(juce::String(relBeat, 3), juce::dontSendNotification);
    if (lengthLabel.getCurrentTextEditor() == nullptr)
        lengthLabel.setText(juce::String(note.lengthBeats, 3), juce::dontSendNotification);
    if (!pitchBox.isPopupActive())
        pitchBox.setSelectedId(note.pitch + 1, juce::dontSendNotification);
    if (velocityLabel.getCurrentTextEditor() == nullptr)
        velocityLabel.setText(juce::String((int) std::round(note.velocity * 127.0f)), juce::dontSendNotification);
}

void NoteRowComponent::setHighlighted(bool shouldHighlight)
{
    if (highlighted != shouldHighlight)
    {
        highlighted = shouldHighlight;
        repaint();
    }
}

void NoteRowComponent::paint(juce::Graphics& g)
{
    if (highlighted)
    {
        g.setColour(juce::Colour(0xff9bbc0f).withAlpha(0.4f));
        g.fillAll();
    }
}

void NoteRowComponent::resized()
{
    auto area = getLocalBounds().reduced(1);
    deleteButton.setBounds(area.removeFromRight(18));
    barLabel.setBounds(area.removeFromLeft(20));
    beatLabel.setBounds(area.removeFromLeft(40));
    lengthLabel.setBounds(area.removeFromLeft(34));
    velocityLabel.setBounds(area.removeFromRight(30));
    pitchBox.setBounds(area); // gets whatever's left -- its text is the longest
}
