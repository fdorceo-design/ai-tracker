#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioEngine.h"

class TrackRowComponent : public juce::Component
{
public:
    TrackRowComponent(AudioEngine& engineIn, int trackIdIn, std::function<void()> onRemoveIn);

    int getTrackId() const { return trackId; }

    void resized() override;
    void refreshStatus();

private:
    void loadClicked();
    void toMidiClicked();

    AudioEngine& engine;
    int trackId;
    std::function<void()> onRemove;

    juce::Label nameLabel;
    juce::TextButton loadButton{"Load VST3..."};
    juce::TextButton toMidiButton{"toMIDI"};
    juce::TextButton editorButton{"Editor"};
    juce::TextButton removeButton{"Remove"};
    juce::Label statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
};
