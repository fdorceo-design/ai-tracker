#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginHost.h"

class MainComponent : public juce::Component,
                       private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void loadPluginClicked();
    void testNoteClicked();
    void timerCallback() override;

    PluginHost pluginHost;

    juce::Label titleLabel;
    juce::TextButton loadButton{"Load VST3..."};
    juce::TextButton editorButton{"Open Editor"};
    juce::TextButton testNoteButton{"Test Note (C4)"};
    juce::Label statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool noteIsOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
