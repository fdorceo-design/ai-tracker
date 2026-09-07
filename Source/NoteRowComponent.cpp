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
    makeEditable(beatLabel);
    beatLabel.onTextChange = [this] { sequencer.setNoteBeat(noteId, parseDouble(beatLabel.getText(), 0.0)); };
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

void NoteRowComponent::refresh(const SequencerNote& note)
{
    if (beatLabel.getCurrentTextEditor() == nullptr)
        beatLabel.setText(juce::String(note.startBeat, 3), juce::dontSendNotification);
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
    deleteButton.setBounds(area.removeFromRight(20));
    beatLabel.setBounds(area.removeFromLeft(area.getWidth() / 4));
    lengthLabel.setBounds(area.removeFromLeft(area.getWidth() / 3));
    pitchBox.setBounds(area.removeFromLeft(area.getWidth() / 2));
    velocityLabel.setBounds(area);
}
