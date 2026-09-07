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

    // Numeric-only, split into two narrow sub-columns (BPM | time-sig
    // numerator) side by side, to keep the whole strip compact.
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff306230));

        const int half = getWidth() / 2;
        g.setColour(juce::Colour(0xff9bbc0f).withAlpha(0.3f));
        g.drawVerticalLine(half, 0.0f, (float) getHeight());

        std::map<double, juce::String> bpmByBeat;
        bpmByBeat[0.0] = juce::String(sequencer.getBpm(), 0);
        for (const auto& ev : sequencer.getTempoEvents())
            bpmByBeat[ev.beat] = juce::String(ev.bpm, 0);

        std::map<double, juce::String> meterByBeat;
        meterByBeat[0.0] = juce::String(sequencer.getBeatsPerBar());
        for (const auto& ev : sequencer.getTimeSigEvents())
            meterByBeat[ev.beat] = juce::String(ev.beatsPerBar);

        g.setColour(juce::Colour(0xff9bbc0f));
        g.setFont(juce::Font(juce::FontOptions(12.0f)));

        for (const auto& [beat, label] : bpmByBeat)
        {
            const int y = content.getYForBeat(beat) - scrollOffsetY;
            if (y < -20 || y > getHeight() + 20)
                continue;
            g.drawText(label, 2, y, half - 4, 18, juce::Justification::centred);
        }
        for (const auto& [beat, label] : meterByBeat)
        {
            const int y = content.getYForBeat(beat) - scrollOffsetY;
            if (y < -20 || y > getHeight() + 20)
                continue;
            g.drawText(label, half + 2, y, getWidth() - half - 4, 18, juce::Justification::centred);
        }
    }

private:
    Sequencer& sequencer;
    TrackerContentComponent& content;
    int scrollOffsetY = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoMapColumn)
};
