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
    // A previous run that was force-killed (e.g. via Task Manager, or a
    // dev rebuild's taskkill) skips the note-offs a clean shutdown would
    // send, which can leave a note stuck sounding on whatever external
    // synth was listening. Blast panic across every MIDI output on the
    // system once, up front, before this session sends anything of its
    // own -- cheap insurance against inheriting someone else's stuck note.
    AudioEngine::panicAllMidiOutputDevices();

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

    // Manual escape hatch for stuck notes (e.g. left sounding by a
    // previous force-killed run, or anything else) -- All Notes Off / All
    // Sound Off on every channel of every current track's output.
    panicButton.onClick = [this] { sequencer.stop(); engine.panicAllTracks(); };
    addAndMakeVisible(panicButton);

    exportMidiButton.onClick = [this] { exportMidiClicked(); };
    addAndMakeVisible(exportMidiButton);

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

    // Start with 2 empty tracks already visible rather than an empty grid --
    // BPM 120 / 4/4 are already Sequencer's own defaults, nothing to set.
    // The default window width (see Main.cpp) fits 4 columns, so there's
    // room to add 2 more with "+ Add Track" before any horizontal scrolling
    // is needed.
    for (int i = 0; i < 2; ++i)
        engine.addTrack({});

    trackerViewport.setViewedComponent(&trackerContent, false);
    addAndMakeVisible(trackerViewport);
    trackerContent.refreshTracks();
    addAndMakeVisible(tempoMapColumn);

    scrollToTopButton.setButtonText(juce::CharPointer_UTF8("\xe2\x96\xb3")); // "△"
    scrollToTopButton.onClick = [this] { trackerViewport.setViewPosition(trackerViewport.getViewPositionX(), 0); };
    addAndMakeVisible(scrollToTopButton);
    scrollToTopButton.toFront(false);

    addAndMakeVisible(playbackMask);
    playbackMask.toFront(false);

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
    sequencer.stop();
    engine.panicAllTracks();
}

void MainComponent::exportMidiClicked()
{
    const auto defaultFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                                  .getChildFile("AI Tracker Session.mid");
    exportFileChooser = std::make_unique<juce::FileChooser>("Export MIDI", defaultFile, "*.mid");
    exportFileChooser->launchAsync(
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file == juce::File())
                return;

            const bool ok = sequencer.exportToMidiFile(file);
            juce::NativeMessageBox::showAsync(
                juce::MessageBoxOptions()
                    .withIconType(ok ? juce::MessageBoxIconType::InfoIcon : juce::MessageBoxIconType::WarningIcon)
                    .withTitle(ok ? "Exported" : "Export failed")
                    .withMessage(ok ? file.getFullPathName() : "Could not write to " + file.getFullPathName())
                    .withButton("OK"),
                nullptr);
        });
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    playbackMask.setBounds(getLocalBounds());

    auto area = getLocalBounds().reduced(outerMargin);
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
    panicButton.setBounds(topRow.removeFromLeft(70));
    topRow.removeFromLeft(10);
    bpmLabel.setBounds(topRow.removeFromLeft(90));
    topRow.removeFromLeft(10);
    timeSignatureLabel.setBounds(topRow.removeFromLeft(60));
    topRow.removeFromLeft(10);
    exportMidiButton.setBounds(topRow.removeFromLeft(120));
    area.removeFromTop(10);

    positionLabel.setBounds(area.removeFromTop(24));
    apiLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    tempoMapColumn.setBounds(area.removeFromLeft(tempoColumnWidth));
    trackerViewport.setBounds(area);

    constexpr int scrollTopButtonSize = 26;
    scrollToTopButton.setBounds(area.getRight() - scrollTopButtonSize - 4, area.getY() + 4,
                                 scrollTopButtonSize, scrollTopButtonSize);
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
        // Snaps rather than follows smoothly: only jumps when the playing
        // bar itself changes, straight to the position where that bar's
        // first row sits as the viewport's 2nd displayed row -- a
        // deliberate jolt at each bar change, not a continuous scroll.
        const int bar = sequencer.getBarIndexForBeat(sequencer.getPositionBeats());
        if (bar != lastScrolledBar)
        {
            lastScrolledBar = bar;
            const double barStartBeat = sequencer.getBarStartBeat(bar);
            const int bandY = trackerContent.getYForBeat(barStartBeat);
            auto viewPos = trackerViewport.getViewPosition();
            viewPos.y = juce::jmax(0, bandY - TrackEventListComponent::rowHeight);
            trackerViewport.setViewPosition(viewPos);
        }
    }
    else
    {
        lastScrolledBar = -1;
    }

    tempoMapColumn.setScrollOffsetY(trackerViewport.getViewPositionY());
    tempoMapColumn.repaint();
    playbackMask.repaint();
}
