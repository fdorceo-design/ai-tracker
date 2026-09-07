cmake_minimum_required(VERSION 3.22)

project(AiTracker VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(httplib)

# --- Main app -----------------------------------------------------------
# Hosts no plugins itself: every Track talks to a child
# AiTrackerPluginServer process (see below) over a named pipe, so a
# crash-prone VST3 (Kontakt et al.) can only take down its own process.

juce_add_gui_app(AiTracker
    PRODUCT_NAME "AI Tracker"
    COMPANY_NAME "AiTracker"
)

target_sources(AiTracker PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/TrackRowComponent.cpp
    Source/NoteRowComponent.cpp
    Source/TrackEventListComponent.cpp
    Source/TrackerContentComponent.cpp
    Source/Track.cpp
    Source/PluginServerProxy.cpp
    Source/AudioEngine.cpp
    Source/Sequencer.cpp
    Source/ApiServer.cpp
)

target_compile_definitions(AiTracker PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    # JUCE 8 defaults to a Direct2D renderer on Windows, which has known
    # hang/freeze issues on some systems during live window resizing (the
    # whole desktop compositor can stall waiting on a frame). Force the
    # older GDI-based software renderer instead -- slower, but stable.
    JUCE_DIRECT2D=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AiTracker,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AiTracker,JUCE_VERSION>"
)

target_link_libraries(AiTracker PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    httplib::httplib
)

if (WIN32)
    target_link_libraries(AiTracker PRIVATE ws2_32)
endif()

# --- Plugin server (one child process per loaded VST3) -------------------

juce_add_gui_app(AiTrackerPluginServer
    PRODUCT_NAME "AiTrackerPluginServer"
    COMPANY_NAME "AiTracker"
)

target_sources(AiTrackerPluginServer PRIVATE
    Source/PluginServerMain.cpp
)

target_compile_definitions(AiTrackerPluginServer PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AiTrackerPluginServer,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AiTrackerPluginServer,JUCE_VERSION>"
    JUCE_PLUGINHOST_VST3=1
)

target_link_libraries(AiTrackerPluginServer PRIVATE
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if (WIN32)
    target_link_libraries(AiTrackerPluginServer PRIVATE dbghelp)
endif()

# The main app locates AiTrackerPluginServer.exe next to its own binary at
# runtime, so copy it into AiTracker's output directory after each build.
add_custom_command(TARGET AiTracker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:AiTrackerPluginServer>
        $<TARGET_FILE_DIR:AiTracker>
    DEPENDS AiTrackerPluginServer
)
