cmake_minimum_required(VERSION 3.22)

project(AiTracker VERSION 0.1.0)

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
    Source/AudioEngine.cpp
    Source/Sequencer.cpp
    Source/ApiServer.cpp
)

target_compile_definitions(AiTracker PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AiTracker,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AiTracker,JUCE_VERSION>"
    JUCE_PLUGINHOST_VST3=1
)

target_link_libraries(AiTracker PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
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
