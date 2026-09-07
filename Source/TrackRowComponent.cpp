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

    toMidiButton.onClick = [this] { toMidiClicked(); };
    addAndMakeVisible(toMidiButton);

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

    channelLabel.setText("Ch " + juce::String(engine.getMidiChannel(trackId)), juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification::centred);
    channelLabel.setFont(juce::Font(juce::FontOptions(12.0f)));
    channelLabel.setEditable(false, true, false);
    channelLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xff306230).withAlpha(0.6f));
    channelLabel.onTextChange = [this]
    {
        const auto digits = channelLabel.getText().retainCharacters("0123456789");
        if (digits.isNotEmpty())
            engine.setMidiChannel(trackId, digits.getIntValue());
        channelLabel.setText("Ch " + juce::String(engine.getMidiChannel(trackId)), juce::dontSendNotification);
    };
    addAndMakeVisible(channelLabel);
}

void TrackRowComponent::resized()
{
    auto area = getLocalBounds().reduced(3);
    nameLabel.setBounds(area.removeFromTop(36));
    area.removeFromTop(2);
    channelLabel.setBounds(area.removeFromTop(16));
    area.removeFromTop(2);
    statusLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(2);
    auto loadRow = area.removeFromTop(22);
    loadButton.setBounds(loadRow.removeFromLeft(loadRow.getWidth() / 2).reduced(1, 0));
    toMidiButton.setBounds(loadRow.reduced(1, 0));
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

void TrackRowComponent::toMidiClicked()
{
    const auto devices = AudioEngine::getAvailableMidiOutputDevices();

    juce::PopupMenu menu;
    if (devices.isEmpty())
    {
        menu.addItem(1, "No MIDI output devices found", false);
    }
    else
    {
        for (int i = 0; i < devices.size(); ++i)
            menu.addItem(i + 1, devices[i]);
    }

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(toMidiButton),
        [this, devices](int result)
        {
            if (result <= 0 || result > devices.size())
                return;

            const auto device = devices[result - 1];
            if (!engine.routeToExternalMidi(trackId, device))
                statusLabel.setText("Failed: MIDI device not found", juce::dontSendNotification);

            refreshStatus();
        });
}

void TrackRowComponent::refreshStatus()
{
    if (nameLabel.getCurrentTextEditor() == nullptr)
        nameLabel.setText(engine.getTrackName(trackId), juce::dontSendNotification);
    if (channelLabel.getCurrentTextEditor() == nullptr)
        channelLabel.setText("Ch " + juce::String(engine.getMidiChannel(trackId)), juce::dontSendNotification);
    if (engine.isPluginLoaded(trackId))
    {
        statusLabel.setText("Loaded: " + engine.getPluginName(trackId), juce::dontSendNotification);
        editorButton.setEnabled(!engine.isExternalMidiRouted(trackId));
    }
    else
    {
        statusLabel.setText("Not loaded: " + engine.getPluginName(trackId), juce::dontSendNotification);
        editorButton.setEnabled(false);
    }
}
