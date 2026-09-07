#include "TrackRowComponent.h"

TrackRowComponent::TrackRowComponent(AudioEngine& engineIn, int trackIdIn, std::function<void()> onRemoveIn)
    : engine(engineIn), trackId(trackIdIn), onRemove(std::move(onRemoveIn))
{
    nameLabel.setText(engine.getTrackName(trackId), juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
    nameLabel.setMinimumHorizontalScale(1.0f);
    nameLabel.setEditable(false, true, false);
    nameLabel.onTextChange = [this] { engine.setTrackName(trackId, nameLabel.getText()); };
    addAndMakeVisible(nameLabel);

    loadButton.onClick = [this] { loadClicked(); };
    addAndMakeVisible(loadButton);

    editorButton.onClick = [this] { engine.showEditorWindow(trackId); };
    editorButton.setEnabled(false);
    addAndMakeVisible(editorButton);

    removeButton.onClick = [this] { if (onRemove) onRemove(); };
    addAndMakeVisible(removeButton);

    statusLabel.setText("No plugin", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    statusLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(statusLabel);
}

void TrackRowComponent::resized()
{
    auto area = getLocalBounds().reduced(3);
    nameLabel.setBounds(area.removeFromTop(36));
    area.removeFromTop(2);
    statusLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(2);
    loadButton.setBounds(area.removeFromTop(22));
    area.removeFromTop(2);
    auto buttonRow = area.removeFromTop(22);
    editorButton.setBounds(buttonRow.removeFromLeft(buttonRow.getWidth() / 2).reduced(1, 0));
    removeButton.setBounds(buttonRow.reduced(1, 0));
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
    nameLabel.setText(engine.getTrackName(trackId), juce::dontSendNotification);
    if (engine.isPluginLoaded(trackId))
    {
        statusLabel.setText("Loaded: " + engine.getPluginName(trackId), juce::dontSendNotification);
        editorButton.setEnabled(true);
    }
    else
    {
        statusLabel.setText("Not loaded: " + engine.getPluginName(trackId), juce::dontSendNotification);
        editorButton.setEnabled(false);
    }
}
