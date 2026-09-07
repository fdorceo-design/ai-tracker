#include "TrackerGridComponent.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int defaultPitch = 60; // C4
    constexpr float defaultVelocity = 0.8f;

    juce::String noteName(int pitch)
    {
        return juce::MidiMessage::getMidiNoteName(pitch, true, true, 4);
    }
}

TrackerGridComponent::TrackerGridComponent(AudioEngine& engineIn, Sequencer& sequencerIn)
    : engine(engineIn), sequencer(sequencerIn)
{
    setSize(getRequiredSize().getWidth(), getRequiredSize().getHeight());
    startTimerHz(15);
}

TrackerGridComponent::~TrackerGridComponent()
{
    stopTimer();
}

void TrackerGridComponent::setTrackOrder(std::vector<int> ids)
{
    trackOrder = std::move(ids);
    const auto size = getRequiredSize();
    setSize(size.getWidth(), size.getHeight());
    repaint();
}

juce::Rectangle<int> TrackerGridComponent::getRequiredSize() const
{
    const int totalSteps = totalBeats * stepsPerBeat;
    const int width = gutterWidth + juce::jmax<int>(1, (int) trackOrder.size()) * columnWidth;
    const int height = totalSteps * rowHeight;
    return { 0, 0, width, height };
}

int TrackerGridComponent::findNoteIdAt(int trackId, int step, int* outPitch, float* outVelocity) const
{
    for (const auto& n : sequencer.getNotes())
    {
        if (n.trackId != trackId)
            continue;

        const int noteStep = (int) std::round(n.startBeat * stepsPerBeat);
        if (noteStep == step)
        {
            if (outPitch != nullptr) *outPitch = n.pitch;
            if (outVelocity != nullptr) *outVelocity = n.velocity;
            return n.id;
        }
    }
    return -1;
}

TrackerGridComponent::CellHit TrackerGridComponent::hitTest(juce::Point<int> pos) const
{
    CellHit hit;
    if (pos.x < gutterWidth || pos.y < 0)
        return hit;

    const int trackIndex = (pos.x - gutterWidth) / columnWidth;
    const int step = pos.y / rowHeight;
    const int totalSteps = totalBeats * stepsPerBeat;

    if (trackIndex < 0 || trackIndex >= (int) trackOrder.size() || step < 0 || step >= totalSteps)
        return hit;

    hit.valid = true;
    hit.trackIndex = trackIndex;
    hit.step = step;
    return hit;
}

void TrackerGridComponent::paint(juce::Graphics& g)
{
    auto backgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.fillAll(backgroundColour);

    const int totalSteps = totalBeats * stepsPerBeat;
    const auto textColour = backgroundColour.contrasting(0.9f);
    const auto gridLineColour = backgroundColour.contrasting(0.15f);
    const auto beatLineColour = backgroundColour.contrasting(0.3f);
    const auto barLineColour = backgroundColour.contrasting(0.5f);
    const auto emptyTextColour = backgroundColour.contrasting(0.35f);

    // Playhead highlight, drawn first so grid lines/text sit on top of it.
    if (sequencer.isPlaying())
    {
        const int playStep = (int) std::floor(sequencer.getPositionBeats() * stepsPerBeat) % juce::jmax(1, totalSteps);
        g.setColour(backgroundColour.contrasting(0.12f));
        g.fillRect(0, playStep * rowHeight, getWidth(), rowHeight);
    }

    // Step gutter.
    g.setFont(juce::Font(juce::FontOptions((float) rowHeight * 0.6f)));
    for (int step = 0; step < totalSteps; ++step)
    {
        const int y = step * rowHeight;
        const bool isBarStart = (step % (stepsPerBeat * 4)) == 0;
        const bool isBeatStart = (step % stepsPerBeat) == 0;

        g.setColour(isBarStart ? barLineColour : (isBeatStart ? beatLineColour : gridLineColour));
        g.drawHorizontalLine(y, 0.0f, (float) getWidth());

        if (isBeatStart)
        {
            g.setColour(textColour);
            g.drawText(juce::String(step / stepsPerBeat), 2, y, gutterWidth - 4, rowHeight,
                       juce::Justification::centredLeft);
        }
    }

    // Track columns.
    g.setFont(juce::Font(juce::FontOptions((float) rowHeight * 0.65f)));
    for (int col = 0; col < (int) trackOrder.size(); ++col)
    {
        const int trackId = trackOrder[(size_t) col];
        const int x = gutterWidth + col * columnWidth;

        g.setColour(gridLineColour);
        g.drawVerticalLine(x, 0.0f, (float) getHeight());

        for (int step = 0; step < totalSteps; ++step)
        {
            const int y = step * rowHeight;
            int pitch = 0;
            float velocity = 0.0f;
            const bool hasNote = findNoteIdAt(trackId, step, &pitch, &velocity) >= 0;

            if (hasNote)
            {
                g.setColour(juce::Colours::orange);
                const auto text = noteName(pitch) + " " + juce::String((int) std::round(velocity * 127.0f)).paddedLeft('0', 3);
                g.drawText(text, x + 4, y, columnWidth - 8, rowHeight, juce::Justification::centredLeft);
            }
            else
            {
                g.setColour(emptyTextColour);
                g.drawText("--- ---", x + 4, y, columnWidth - 8, rowHeight, juce::Justification::centredLeft);
            }
        }
    }

    g.setColour(barLineColour);
    g.drawVerticalLine(gutterWidth + (int) trackOrder.size() * columnWidth - 1, 0.0f, (float) getHeight());
}

void TrackerGridComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto hit = hitTest(e.getPosition());
    if (!hit.valid)
        return;

    const int trackId = trackOrder[(size_t) hit.trackIndex];
    const int existing = findNoteIdAt(trackId, hit.step);

    if (existing >= 0)
        sequencer.removeNote(existing);
    else
        sequencer.addNote(trackId, defaultPitch, defaultVelocity,
                           hit.step / (double) stepsPerBeat, 0.9 / stepsPerBeat);

    repaint();
}

void TrackerGridComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    const auto hit = hitTest(e.getPosition());
    if (!hit.valid)
        return;

    const int trackId = trackOrder[(size_t) hit.trackIndex];
    int pitch = 0;
    const int noteId = findNoteIdAt(trackId, hit.step, &pitch);
    if (noteId < 0)
        return;

    const int delta = wheel.deltaY > 0.0f ? 1 : -1;
    sequencer.setNotePitch(noteId, pitch + delta);
    repaint();
}

void TrackerGridComponent::timerCallback()
{
    repaint();
}
