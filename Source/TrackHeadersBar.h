#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <algorithm>
#include "AudioEngine.h"
#include "TrackRowComponent.h"
#include "TrackerContentComponent.h"

// The per-track header row (name/channel/load/editor/remove), pulled out of
// TrackerContentComponent so it can live in its own horizontal-scroll-only
// juce::Viewport, staying visible at a fixed vertical position in the
// window while the note grid below it scrolls vertically -- MainComponent
// mirrors the note grid's horizontal scroll position onto this bar's
// viewport each tick so the two stay column-aligned.
class TrackHeadersBar : public juce::Component
{
public:
    static constexpr int headerHeight = 152;

    explicit TrackHeadersBar(AudioEngine& engineIn) : engine(engineIn) {}

    void refreshTracks()
    {
        const auto ids = engine.getTrackIds();

        for (auto it = headers.begin(); it != headers.end();)
        {
            if (std::find(ids.begin(), ids.end(), (*it)->getTrackId()) == ids.end())
                it = headers.erase(it);
            else
                ++it;
        }

        std::vector<std::unique_ptr<TrackRowComponent>> reordered;
        reordered.reserve(ids.size());
        for (auto id : ids)
        {
            auto found = std::find_if(headers.begin(), headers.end(),
                                       [id](const std::unique_ptr<TrackRowComponent>& h) { return h->getTrackId() == id; });
            if (found != headers.end())
            {
                reordered.push_back(std::move(*found));
                headers.erase(found);
            }
            else
            {
                auto header = std::make_unique<TrackRowComponent>(engine, id, [this, id] { engine.removeTrack(id); });
                addAndMakeVisible(*header);
                reordered.push_back(std::move(header));
            }
        }
        headers = std::move(reordered);
        for (auto& h : headers)
            h->refreshStatus();

        setSize(juce::jmax(1, (int) ids.size()) * TrackerContentComponent::columnWidth, headerHeight);
        resized();
    }

    void resized() override
    {
        int x = 0;
        for (auto& h : headers)
        {
            h->setBounds(x, 0, TrackerContentComponent::columnWidth, headerHeight);
            x += TrackerContentComponent::columnWidth;
        }
    }

private:
    AudioEngine& engine;
    std::vector<std::unique_ptr<TrackRowComponent>> headers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackHeadersBar)
};
