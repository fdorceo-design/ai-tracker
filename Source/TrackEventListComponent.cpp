#include "TrackEventListComponent.h"
#include <algorithm>
#include <cmath>

TrackEventListComponent::TrackEventListComponent(Sequencer& sequencerIn, int trackIdIn)
    : sequencer(sequencerIn), trackId(trackIdIn)
{
    captionLabel.setText("bar.beat | len | pitch | vel", juce::dontSendNotification);
    captionLabel.setJustificationType(juce::Justification::centred);
    captionLabel.setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::bold)));
    addAndMakeVisible(captionLabel);

    addQuarterButton.onClick = [this] { addNoteClicked(1.0); };
    addAndMakeVisible(addQuarterButton);

    addEighthButton.onClick = [this] { addNoteClicked(0.5); };
    addAndMakeVisible(addEighthButton);

    addSixteenthButton.onClick = [this] { addNoteClicked(0.25); };
    addAndMakeVisible(addSixteenthButton);
}

void TrackEventListComponent::addNoteClicked(double lengthBeats)
{
    double nextBeat = 0.0;
    for (const auto& n : sequencer.getNotes())
        if (n.trackId == trackId)
            nextBeat = juce::jmax(nextBeat, n.startBeat + n.lengthBeats);
    sequencer.addNote(trackId, 60, 0.8f, nextBeat, lengthBeats);
}

void TrackEventListComponent::applyBarLayout(const std::vector<BarBand>& bands)
{
    std::vector<SequencerNote> mine;
    for (const auto& n : sequencer.getNotes())
        if (n.trackId == trackId)
            mine.push_back(n);
    std::sort(mine.begin(), mine.end(),
              [](const SequencerNote& a, const SequencerNote& b) { return a.startBeat < b.startBeat; });

    for (auto it = rows.begin(); it != rows.end();)
    {
        const bool stillExists = std::any_of(mine.begin(), mine.end(),
                                              [&](const SequencerNote& n) { return n.id == (*it)->getNoteId(); });
        if (!stillExists)
            it = rows.erase(it);
        else
            ++it;
    }

    std::vector<std::unique_ptr<NoteRowComponent>> reordered;
    reordered.reserve(mine.size());
    for (const auto& n : mine)
    {
        auto found = std::find_if(rows.begin(), rows.end(),
                                   [&](const std::unique_ptr<NoteRowComponent>& r) { return r->getNoteId() == n.id; });
        if (found != rows.end())
        {
            (*found)->refresh(n);
            reordered.push_back(std::move(*found));
            rows.erase(found);
        }
        else
        {
            auto row = std::make_unique<NoteRowComponent>(sequencer, n.id);
            row->refresh(n);
            addAndMakeVisible(*row);
            reordered.push_back(std::move(row));
        }
    }
    rows = std::move(reordered);

    const bool playing = sequencer.isPlaying();
    const double pos = sequencer.getPositionBeats();
    const int notesTop = captionHeight + rowHeight;

    bool haveCurrentBar = false;
    int currentBar = 0;
    int indexWithinBar = 0;

    for (size_t i = 0; i < rows.size(); ++i)
    {
        const auto& n = mine[i];
        const int bar = sequencer.getBarIndexForBeat(n.startBeat);

        if (!haveCurrentBar || bar != currentBar)
        {
            haveCurrentBar = true;
            currentBar = bar;
            indexWithinBar = 0;
        }
        else
        {
            ++indexWithinBar;
        }

        const auto bandIt = std::find_if(bands.begin(), bands.end(),
                                          [bar](const BarBand& b) { return b.barIndex == bar; });
        const int bandY = (bandIt != bands.end()) ? bandIt->yPixel : 0;

        rows[i]->setBounds(0, notesTop + bandY + indexWithinBar * rowHeight, getWidth(), rowHeight);
        rows[i]->setHighlighted(playing && pos >= n.startBeat && pos < n.startBeat + n.lengthBeats);
    }

    resized();
}

void TrackEventListComponent::resized()
{
    captionLabel.setBounds(0, 0, getWidth(), captionHeight);

    auto addRow = juce::Rectangle<int>(0, captionHeight, getWidth(), rowHeight);
    const int third = addRow.getWidth() / 3;
    addQuarterButton.setBounds(addRow.removeFromLeft(third).reduced(1, 0));
    addEighthButton.setBounds(addRow.removeFromLeft(third).reduced(1, 0));
    addSixteenthButton.setBounds(addRow.reduced(1, 0));

    for (auto& row : rows)
        row->setBounds(0, row->getY(), getWidth(), rowHeight);
}
