#include "TrackerContentComponent.h"
#include <algorithm>
#include <cmath>
#include <map>

TrackerContentComponent::TrackerContentComponent(AudioEngine& engineIn, Sequencer& sequencerIn)
    : engine(engineIn), sequencer(sequencerIn)
{
}

void TrackerContentComponent::refreshTracks()
{
    const auto ids = engine.getTrackIds();

    // Headers.
    for (auto it = headers.begin(); it != headers.end();)
    {
        if (std::find(ids.begin(), ids.end(), (*it)->getTrackId()) == ids.end())
            it = headers.erase(it);
        else
            ++it;
    }
    {
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
    }
    for (auto& h : headers)
        h->refreshStatus();

    // Event lists.
    for (auto it = eventLists.begin(); it != eventLists.end();)
    {
        if (std::find(ids.begin(), ids.end(), (*it)->getTrackId()) == ids.end())
            it = eventLists.erase(it);
        else
            ++it;
    }
    {
        std::vector<std::unique_ptr<TrackEventListComponent>> reordered;
        reordered.reserve(ids.size());
        for (auto id : ids)
        {
            auto found = std::find_if(eventLists.begin(), eventLists.end(),
                                       [id](const std::unique_ptr<TrackEventListComponent>& e) { return e->getTrackId() == id; });
            if (found != eventLists.end())
            {
                reordered.push_back(std::move(*found));
                eventLists.erase(found);
            }
            else
            {
                auto list = std::make_unique<TrackEventListComponent>(sequencer, id);
                addAndMakeVisible(*list);
                reordered.push_back(std::move(list));
            }
        }
        eventLists = std::move(reordered);
    }

    const bool tracksChanged = (ids != trackOrder);
    trackOrder = ids;

    // The rest of this function -- rebuilding the bar-band layout and every
    // track's note rows -- is O(total notes) and was previously done
    // unconditionally on every 10Hz timer tick. Skip it when nothing that
    // would change its output has happened since last time (still always
    // runs while playing, since the playhead/highlighted-row state needs
    // to keep advancing). This used to be cheap enough not to matter, but
    // competing with the flood of paint/resize messages during a live
    // window resize was enough to make the whole app appear to hang.
    const uint64_t currentRevision = sequencer.getRevision();
    if (!tracksChanged && currentRevision == lastSequencerRevision && !sequencer.isPlaying())
        return;
    lastSequencerRevision = currentRevision;

    // Shared bar layout: group every track's notes by bar, and size each
    // bar's band to the tallest track within it, so a track with fewer
    // notes in a given bar just leaves the rest of that band blank.
    // Bar indices account for any scheduled time-signature changes.
    std::map<int, std::map<int, int>> countPerBarPerTrack;
    for (const auto& n : sequencer.getNotes())
    {
        const int bar = sequencer.getBarIndexForBeat(n.startBeat);
        countPerBarPerTrack[bar][n.trackId] += 1;
    }

    barBands.clear();
    int y = 0;
    for (const auto& [barIndex, perTrack] : countPerBarPerTrack)
    {
        int maxCount = 1;
        for (const auto& [trackId, count] : perTrack)
            maxCount = juce::jmax(maxCount, count);

        BarBand band;
        band.barIndex = barIndex;
        band.yPixel = y;
        band.rowCount = maxCount;
        barBands.push_back(band);

        y += maxCount * TrackEventListComponent::rowHeight;
    }
    const int notesAreaHeight = y;

    for (auto& e : eventLists)
        e->applyBarLayout(barBands);

    const int width = juce::jmax(1, (int) trackOrder.size()) * columnWidth;
    const int fixedHeaderPart = TrackEventListComponent::captionHeight + TrackEventListComponent::rowHeight;
    setSize(width, headerHeight + fixedHeaderPart + juce::jmax(60, notesAreaHeight));
    resized();

    // The bar lines are decoration drawn by this component itself (not a
    // child), so they need an explicit repaint whenever bar membership
    // changes -- e.g. editing a note's beat can shift which bar it's in
    // without changing this component's overall size or any child's
    // bounds, which would otherwise leave stale lines on screen.
    repaint();
}

void TrackerContentComponent::resized()
{
    int x = 0;
    for (size_t i = 0; i < trackOrder.size(); ++i)
    {
        if (i < headers.size())
            headers[i]->setBounds(x, 0, columnWidth, headerHeight);
        if (i < eventLists.size())
            eventLists[i]->setBounds(x, headerHeight, columnWidth, getHeight() - headerHeight);
        x += columnWidth;
    }
}

void TrackerContentComponent::paintOverChildren(juce::Graphics& g)
{
    // Drawn once here (not per-column) so every column's bar line lands at
    // exactly the same pixel height.
    const int baseY = headerHeight + TrackEventListComponent::captionHeight + TrackEventListComponent::rowHeight;
    g.setColour(juce::Colour(0xff9bbc0f));
    for (size_t i = 1; i < barBands.size(); ++i)
        g.fillRect(0, baseY + barBands[i].yPixel - 1, getWidth(), 2);

    if (sequencer.isPlaying())
    {
        const int y = getYForBeat(sequencer.getPositionBeats());
        g.setColour(juce::Colour(0xffe0f8cf));
        g.fillRect(0, y - 1, getWidth(), 2);
    }
}

int TrackerContentComponent::getYForBeat(double beat) const
{
    const int baseY = headerHeight + TrackEventListComponent::captionHeight + TrackEventListComponent::rowHeight;
    if (barBands.empty())
        return baseY;

    const int bar = sequencer.getBarIndexForBeat(beat);

    for (const auto& band : barBands)
        if (band.barIndex == bar)
            return baseY + band.yPixel;

    if (bar < barBands.front().barIndex)
        return baseY;
    return baseY + barBands.back().yPixel;
}
