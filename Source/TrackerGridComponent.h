#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"
#include "Sequencer.h"

// LSDj/tracker-style step grid: one column per track, one row per 16th
// note. Click an empty cell to drop a note (C4, default velocity/length),
// click a filled cell to remove it, scroll the wheel over a filled cell to
// nudge its pitch. Notes are stored as free-form beat positions in
// Sequencer; this view just quantizes them to steps for display/editing.
class TrackerGridComponent : public juce::Component,
                              private juce::Timer
{
public:
    static constexpr int columnWidth = 132;
    static constexpr int gutterWidth = 48;
    static constexpr int rowHeight = 18;
    static constexpr int stepsPerBeat = 4;
    static constexpr int totalBeats = 64;

    TrackerGridComponent(AudioEngine& engineIn, Sequencer& sequencerIn);
    ~TrackerGridComponent() override;

    void setTrackOrder(std::vector<int> ids);
    juce::Rectangle<int> getRequiredSize() const;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

private:
    void timerCallback() override;

    struct CellHit
    {
        bool valid = false;
        int trackIndex = 0;
        int step = 0;
    };
    CellHit hitTest(juce::Point<int> pos) const;
    int findNoteIdAt(int trackId, int step, int* outPitch = nullptr, float* outVelocity = nullptr) const;

    AudioEngine& engine;
    Sequencer& sequencer;
    std::vector<int> trackOrder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackerGridComponent)
};
