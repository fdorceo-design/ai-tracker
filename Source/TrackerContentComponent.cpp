#include "TrackerContentComponent.h"
#include <algorithm>

TrackerContentComponent::TrackerContentComponent(AudioEngine& engineIn, Sequencer& sequencerIn)
    : engine(engineIn), sequencer(sequencerIn), grid(engineIn, sequencerIn)
{
    addAndMakeVisible(grid);
}

void TrackerContentComponent::refreshTracks()
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
            headers.erase(found); // drop the now-moved-from (null) slot so later
                                   // lookups in this loop never dereference it
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

    trackOrder = ids;
    grid.setTrackOrder(trackOrder);

    const auto gridSize = grid.getRequiredSize();
    setSize(gridSize.getWidth(), headerHeight + gridSize.getHeight());
    resized();
}

void TrackerContentComponent::resized()
{
    int x = TrackerGridComponent::gutterWidth;
    for (auto& h : headers)
    {
        h->setBounds(x, 0, TrackerGridComponent::columnWidth, headerHeight);
        x += TrackerGridComponent::columnWidth;
    }

    grid.setBounds(0, headerHeight, getWidth(), getHeight() - headerHeight);
}
