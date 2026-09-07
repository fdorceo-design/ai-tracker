#include "MainComponent.h"
#include <cmath>

namespace
{
    constexpr int apiPort = 8080;

    // Converts a plain decimal (quarter note = 1) into the smallest N/D
    // fraction with a power-of-two denominator that reproduces it -- e.g.
    // 1.5 -> 3/8, 1.0 -> 1/4, 4.0 -> 4/4. Purely a display convenience;
    // the app's own bar math only ever uses the quarter-note value itself.
    std::pair<int, int> quarterBeatsToFraction(double quarterBeats)
    {
        for (int denominator : { 1, 2, 4, 8, 16, 32, 64 })
        {
            const double numerator = quarterBeats * denominator / 4.0;
            const double rounded = std::round(numerator);
            if (std::abs(numerator - rounded) < 0.001 && rounded >= 1.0)
                return { (int) rounded, denominator };
        }
        return { juce::jmax(1, (int) std::round(quarterBeats * 64.0 / 4.0)), 64 };
    }
}

namespace
{
    // Classic Game Boy / LSDj 4-shade green palette.
    const juce::Colour gbDarkest  { 0xff0f380f };
    const juce::Colour gbDark     { 0xff306230 };
    const juce::Colour gbLight    { 0xff8bac0f };
    const juce::Colour gbLightest { 0xff9bbc0f };

    class LsdjLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
        {
            return juce::Font(juce::FontOptions(juce::jmin(22.0f, (float) buttonHeight * 0.7f)));
        }
    };

    void applyLsdjPalette()
    {
        static LsdjLookAndFeel lsdjLookAndFeel;
        juce::Desktop::getInstance().setDefaultLookAndFeel(&lsdjLookAndFeel);
        auto& laf = lsdjLookAndFeel;
        laf.setColour(juce::ResizableWindow::backgroundColourId, gbDarkest);
        laf.setColour(juce::Label::textColourId, gbLightest);
        laf.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        laf.setColour(juce::TextButton::buttonColourId, gbDark);
        laf.setColour(juce::TextButton::buttonOnColourId, gbLight);
        laf.setColour(juce::TextButton::textColourOffId, gbLightest);
        laf.setColour(juce::TextButton::textColourOnId, gbDarkest);
        laf.setColour(juce::TextEditor::backgroundColourId, gbDark);
        laf.setColour(juce::TextEditor::textColourId, gbLightest);
        laf.setColour(juce::TextEditor::outlineColourId, gbLight);
        laf.setColour(juce::TextEditor::focusedOutlineColourId, gbLightest);
        laf.setColour(juce::TextEditor::highlightColourId, gbLight);
        laf.setColour(juce::ScrollBar::thumbColourId, gbDark);
        laf.setColour(juce::ScrollBar::backgroundColourId, gbDarkest);
        laf.setColour(juce::DocumentWindow::backgroundColourId, gbDarkest);
        laf.setColour(juce::ComboBox::backgroundColourId, gbDark);
        laf.setColour(juce::ComboBox::textColourId, gbLightest);
        laf.setColour(juce::ComboBox::outlineColourId, gbLight);
        laf.setColour(juce::ComboBox::arrowColourId, gbLightest);
        laf.setColour(juce::ComboBox::buttonColourId, gbDark);
        laf.setColour(juce::PopupMenu::backgroundColourId, gbDarkest);
        laf.setColour(juce::PopupMenu::textColourId, gbLightest);
        laf.setColour(juce::PopupMenu::highlightedBackgroundColourId, gbDark);
        laf.setColour(juce::PopupMenu::highlightedTextColourId, gbLightest);
    }
}

MainComponent::MainComponent()
{
    applyLsdjPalette();

    titleLabel.setText("AI Tracker", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(20.0f));
    addAndMakeVisible(titleLabel);

    addTrackButton.onClick = [this] { engine.addTrack({}); };
    addAndMakeVisible(addTrackButton);

    playButton.onClick = [this] { sequencer.play(); };
    addAndMakeVisible(playButton);

    pauseButton.onClick = [this] { sequencer.pause(); };
    addAndMakeVisible(pauseButton);

    stopButton.onClick = [this] { sequencer.stop(); };
    addAndMakeVisible(stopButton);

    bpmLabel.setJustificationType(juce::Justification::centred);
    bpmLabel.setEditable(false, true, false);
    bpmLabel.setColour(juce::Label::backgroundColourId, gbDark);
    bpmLabel.onTextChange = [this]
    {
        const auto value = bpmLabel.getText().retainCharacters("0123456789.").getDoubleValue();
        if (value > 0.0)
            sequencer.setBpm(value);
        bpmLabel.setText(juce::String(sequencer.getBpm(), 0), juce::dontSendNotification);
    };
    addAndMakeVisible(bpmLabel);

    timeSignatureLabel.setJustificationType(juce::Justification::centred);
    timeSignatureLabel.setEditable(false, true, false);
    timeSignatureLabel.setColour(juce::Label::backgroundColourId, gbDark);
    timeSignatureLabel.onTextChange = [this]
    {
        // Plain decimal input, quarter note = 1 (e.g. "1.5" for 3/8) --
        // not "N/D" notation. Musical N/D is derived from it only for
        // display; the value actually driving bar math is this number.
        const auto quarterBeats = timeSignatureLabel.getText().retainCharacters("0123456789.").getDoubleValue();
        if (quarterBeats > 0.0)
        {
            const auto [numerator, denominator] = quarterBeatsToFraction(quarterBeats);
            sequencer.setTimeSignature(numerator, denominator);
        }
        timeSignatureLabel.setText(juce::String(sequencer.getBeatsPerBar()) + "/" + juce::String(sequencer.getTimeSigDenominator()),
                                    juce::dontSendNotification);
    };
    addAndMakeVisible(timeSignatureLabel);

    bpmLabel.setText(juce::String(sequencer.getBpm(), 0), juce::dontSendNotification);
    timeSignatureLabel.setText(juce::String(sequencer.getBeatsPerBar()) + "/" + juce::String(sequencer.getTimeSigDenominator()),
                                juce::dontSendNotification);

    positionLabel.setJustificationType(juce::Justification::centred);
    positionLabel.setEditable(false, true, false);
    positionLabel.setColour(juce::Label::backgroundColourId, gbDark);
    positionLabel.onTextChange = [this]
    {
        const auto value = positionLabel.getText().retainCharacters("0123456789.").getDoubleValue();
        sequencer.setPositionBeats(juce::jmax(0.0, value));
    };
    addAndMakeVisible(positionLabel);

    apiLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(apiLabel);

    // Start with 3 empty tracks already visible rather than an empty grid --
    // BPM 120 / 4/4 are already Sequencer's own defaults, nothing to set.
    for (int i = 0; i < 3; ++i)
        engine.addTrack({});

    trackerViewport.setViewedComponent(&trackerContent, false);
    addAndMakeVisible(trackerViewport);
    trackerContent.refreshTracks();
    addAndMakeVisible(tempoMapColumn);

    if (apiServer.start(apiPort))
        apiLabel.setText("API: http://127.0.0.1:" + juce::String(apiPort), juce::dontSendNotification);
    else
        apiLabel.setText("API failed to start", juce::dontSendNotification);

    setSize(900, 640);
    startTimerHz(10);
}

MainComponent::~MainComponent()
{
    stopTimer();
    apiServer.stop();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    titleLabel.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);

    auto topRow = area.removeFromTop(30);
    addTrackButton.setBounds(topRow.removeFromLeft(120));
    topRow.removeFromLeft(10);
    playButton.setBounds(topRow.removeFromLeft(80));
    topRow.removeFromLeft(10);
    pauseButton.setBounds(topRow.removeFromLeft(80));
    topRow.removeFromLeft(10);
    stopButton.setBounds(topRow.removeFromLeft(80));
    topRow.removeFromLeft(10);
    bpmLabel.setBounds(topRow.removeFromLeft(90));
    topRow.removeFromLeft(10);
    timeSignatureLabel.setBounds(topRow.removeFromLeft(60));
    area.removeFromTop(10);

    positionLabel.setBounds(area.removeFromTop(24));
    apiLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    tempoMapColumn.setBounds(area.removeFromLeft(56));
    trackerViewport.setBounds(area);
}

void MainComponent::timerCallback()
{
    trackerContent.refreshTracks();

    // Avoid stomping the label while the user is actively editing it.
    if (bpmLabel.getCurrentTextEditor() == nullptr)
        bpmLabel.setText(juce::String(sequencer.getBpm(), 0), juce::dontSendNotification);
    if (timeSignatureLabel.getCurrentTextEditor() == nullptr)
        timeSignatureLabel.setText(juce::String(sequencer.getBeatsPerBar()) + "/" + juce::String(sequencer.getTimeSigDenominator()),
                                    juce::dontSendNotification);

    if (positionLabel.getCurrentTextEditor() == nullptr)
    {
        juce::String txt = sequencer.isPlaying() ? "Playing" : "Stopped";
        txt << "  beat " << juce::String(sequencer.getPositionBeats(), 2);
        positionLabel.setText(txt, juce::dontSendNotification);
    }

    if (sequencer.isPlaying())
    {
        const int y = trackerContent.getYForBeat(sequencer.getPositionBeats());
        auto viewPos = trackerViewport.getViewPosition();
        const int viewportHeight = trackerViewport.getHeight();
        const int margin = viewportHeight / 3;

        if (y < viewPos.y + margin)
            viewPos.y = juce::jmax(0, y - margin);
        else if (y > viewPos.y + viewportHeight - margin)
            viewPos.y = y - viewportHeight + margin;

        trackerViewport.setViewPosition(viewPos);
    }

    tempoMapColumn.setScrollOffsetY(trackerViewport.getViewPositionY());
    tempoMapColumn.repaint();
}
