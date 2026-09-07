#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"
#include "MidiSessionTests.h"

class AiTrackerApplication : public juce::JUCEApplication
{
public:
    AiTrackerApplication() = default;

    const juce::String getApplicationName() override { return "AI Tracker"; }
    const juce::String getApplicationVersion() override { return "0.0.1"; }

    void initialise(const juce::String& commandLine) override
    {
        const auto args = juce::StringArray::fromTokens(commandLine, true);
        if (args.size() == 2 && args[0] == "--midi-self-test")
        {
            setApplicationReturnValue(runMidiSessionTests(juce::File(args[1].unquoted())) ? 0 : 1);
            quit();
            return;
        }
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(const juce::String& name)
            : DocumentWindow(name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                  .findColour(juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

            // Default position: top-left of the screen rather than centred.
            // Height is 80% of the display (a 1080p-ish reference, scaled
            // via juce::Desktop so other resolutions still land sensibly).
            // Width fits exactly 4 track columns -- the grid starts with
            // only 2 tracks, so there's room to add 2 more via
            // "+ Add Track" before horizontal scrolling is needed.
            const auto displayArea = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
            constexpr int scrollbarAllowance = 20;
            const int width = juce::jmin(displayArea.getWidth(),
                                          2 * MainComponent::outerMargin + MainComponent::tempoColumnWidth
                                              + 4 * TrackerContentComponent::columnWidth + scrollbarAllowance);
            const int height = (int) (displayArea.getHeight() * 0.8);
            setBounds(displayArea.getX(), displayArea.getY(), width, height);

            setResizable(true, true);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(AiTrackerApplication)
