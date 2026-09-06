#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"
#include "MidiSessionTests.h"

class AiTrackerApplication : public juce::JUCEApplication
{
public:
    AiTrackerApplication() = default;

    const juce::String getApplicationName() override { return "AI Tracker"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }

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
            centreWithSize(getWidth(), getHeight());
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
