#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Sequencer.h"
#include "TrackerContentComponent.h"

// A full-window overlay that, while playing, dims everything except the
// currently-playing bar (across all track columns) and the transport's
// Pause/Stop buttons -- so the eye is drawn to what's audible right now
// and the controls needed to react to it stay reachable/visible. Purely
// visual: never intercepts mouse input, so it doesn't affect clicking
// anything underneath it (dimmed or not).
class PlaybackMaskOverlay : public juce::Component
{
public:
    PlaybackMaskOverlay(Sequencer& sequencerIn, juce::Viewport& viewportIn, TrackerContentComponent& contentIn,
                         juce::Component& pauseButtonIn, juce::Component& stopButtonIn)
        : sequencer(sequencerIn), viewport(viewportIn), content(contentIn),
          pauseButton(pauseButtonIn), stopButton(stopButtonIn)
    {
        setInterceptsMouseClicks(false, false);
    }

    void paint(juce::Graphics& g) override
    {
        // Stays up through a pause (playing==false but position holds where
        // it was left), not just while actually playing -- only clears once
        // truly stopped/reset (position back at 0).
        if (!sequencer.isPlaying() && sequencer.getPositionBeats() <= 0.0)
            return;

        auto barRect = content.getBandRectForBeat(sequencer.getPositionBeats());
        if (!barRect.isEmpty())
        {
            barRect = getLocalArea(&content, barRect);
            barRect = barRect.getIntersection(getLocalArea(&viewport, viewport.getLocalBounds()));
        }

        g.saveState();
        if (!barRect.isEmpty())
            g.excludeClipRegion(barRect);
        g.excludeClipRegion(pauseButton.getBounds());
        g.excludeClipRegion(stopButton.getBounds());

        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillRect(getLocalBounds());
        g.restoreState();
    }

private:
    Sequencer& sequencer;
    juce::Viewport& viewport;
    TrackerContentComponent& content;
    juce::Component& pauseButton;
    juce::Component& stopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackMaskOverlay)
};
