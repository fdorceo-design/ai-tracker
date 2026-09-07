#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <map>
#include "Sequencer.h"
#include "TrackerContentComponent.h"

// A narrow strip pinned to the left of the tracker viewport, showing the
// BPM/time-signature value active at each point down the timeline (i.e.
// wherever a tempo-events/timesig-events change lands), aligned vertically
// with the bar it applies to. It never scrolls horizontally with the track
// columns -- MainComponent keeps it outside the viewport and feeds it the
// viewport's vertical scroll offset each frame so its labels track the
// same bars the tracker grid shows.
class TempoMapColumn : public juce::Component
{
public:
    TempoMapColumn(Sequencer& sequencerIn, TrackerContentComponent& contentIn)
        : sequencer(sequencerIn), content(contentIn)
    {
    }

    void setScrollOffsetY(int y)
    {
        if (scrollOffsetY != y)
        {
            scrollOffsetY = y;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff306230));

        // Group by beat first so a tempo change and a time-signature change
        // landing on the same beat (e.g. the piece's initial values, both
        // nominally at beat 0) share one row instead of overlapping.
        std::map<double, juce::String> labelsByBeat;
        auto append = [&labelsByBeat](double beat, const juce::String& text)
        {
            auto& label = labelsByBeat[beat];
            label = label.isEmpty() ? text : label + "  " + text;
        };

        append(0.0, juce::String(sequencer.getBpm(), 0) + " bpm");
        append(0.0, juce::String(sequencer.getBeatsPerBar()) + "/4");

        for (const auto& ev : sequencer.getTempoEvents())
            append(ev.beat, juce::String(ev.bpm, 0) + " bpm");
        for (const auto& ev : sequencer.getTimeSigEvents())
            append(ev.beat, juce::String(ev.beatsPerBar) + "/4");

        g.setColour(juce::Colour(0xff9bbc0f));
        g.setFont(juce::Font(juce::FontOptions(13.0f)));

        for (const auto& [beat, label] : labelsByBeat)
        {
            const int y = content.getYForBeat(beat) - scrollOffsetY;
            if (y < -20 || y > getHeight() + 20)
                continue;
            g.drawText(label, 4, y, getWidth() - 8, 18, juce::Justification::centredLeft);
        }
    }

private:
    Sequencer& sequencer;
    TrackerContentComponent& content;
    int scrollOffsetY = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoMapColumn)
};
