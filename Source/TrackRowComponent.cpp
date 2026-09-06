#include "TrackRowComponent.h"

TrackRowComponent::TrackRowComponent(AudioEngine& engineIn, int trackIdIn, std::function<void()> onRemoveIn)
    : engine(engineIn), trackId(trackIdIn), onRemove(std::move(onRemoveIn))
{
    nameLabel.setText(engine.getTrackName(trackId), juce::dontSendNotification);
    addAndMakeVisible(nameLabel);

    loadButton.onClick = [this] { loadClicked(); };
    addAndMakeVisible(loadButton);

    editorButton.onClick = [this] { engine.showEditorWindow(trackId); };
    editorButton.setEnabled(false);
    addAndMakeVisible(editorButton);

    removeButton.onClick = [this] { if (onRemove) onRemove(); };
    addAndMakeVisible(removeButton);

    statusLabel.setText("No plugin", juce::dontSendNotification);
    addAndMakeVisible(statusLabel);
}

void TrackRowComponent::resized()
{
    auto area = getLocalBounds().reduced(2);
    nameLabel.setBounds(area.removeFromLeft(90));
    loadButton.setBounds(area.removeFromLeft(100));
    editorButton.setBounds(area.removeFromLeft(70));
    removeButton.setBounds(area.removeFromLeft(70));
    statusLabel.setBounds(area);
}

void TrackRowComponent::loadClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select a VST3 plugin",
        juce::File("C:\\Program Files\\Common Files\\VST3"),
        "*.vst3");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file == juce::File())
                return;

            statusLabel.setText("Loading...", juce::dontSendNotification);

            engine.loadPlugin(trackId, file, [this](juce::String error)
            {
                statusLabel.setText("Failed: " + error, juce::dontSendNotification);
            });

            refreshStatus();
        });
}

void TrackRowComponent::refreshStatus()
{
    if (engine.isPluginLoaded(trackId))
    {
        statusLabel.setText("Loaded: " + engine.getPluginName(trackId), juce::dontSendNotification);
        editorButton.setEnabled(true);
    }
}
