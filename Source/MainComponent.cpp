#include "MainComponent.h"

MainComponent::MainComponent()
{
    titleLabel.setText("AI Tracker - scaffold OK", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(24.0f));
    addAndMakeVisible(titleLabel);

    setSize(800, 600);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    titleLabel.setBounds(getLocalBounds());
}
