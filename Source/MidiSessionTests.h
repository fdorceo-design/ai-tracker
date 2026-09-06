#pragma once
#include "Sequencer.h"

// Run explicitly with --midi-self-test <output-directory>; no instruments are loaded.
inline bool runMidiSessionTests(const juce::File& directory)
{
    directory.createDirectory();
    juce::StringArray failures;
    auto check = [&](bool ok, const juce::String& label) { if (!ok) failures.add(label); };
    AudioEngine engine;
    Sequencer seq(engine);
    const auto bass = engine.addTrack(juce::String::fromUTF8("\xe3\x83\x99\xe3\x83\xbc\xe3\x82\xb9"));
    const auto guitar = engine.addTrack("Comping");
    const auto lead = engine.addTrack("Lead | alternate");
    const auto empty = engine.addTrack("Empty");
    engine.setInstrumentIdentity(bass, "Ample Bass U", "C:\\Missing\\ABU.vst3");
    engine.setInstrumentIdentity(guitar, "Ample Guitar L", "C:\\Missing\\AGL.vst3");
    engine.setInstrumentIdentity(lead, "Ample Guitar L", "C:\\Missing\\AGL.vst3");
    seq.setBpm(93);
    seq.addNote(bass, 24, 0.7f, 0, 0.025);
    seq.addNote(bass, 55, 0.8f, 0.06, 0.9);
    seq.addNote(guitar, 63, 0.6f, 2.0 / 3.0, 0.22);
    seq.addNote(lead, 72, 0.75f, 3, 1.25);
    const auto file = directory.getChildFile("Roundtrip.mid");
    check(seq.exportToMidiFile(file), "export");
    check(seq.exportToMidiFile(file), "atomic overwrite");
    juce::MidiFile midi;
    auto stream = file.createInputStream();
    check(stream != nullptr && midi.readFrom(*stream), "read standard MIDI");
    check(midi.getNumTracks() == 5, "tempo plus four tracks including empty");
    stream.reset();
    const auto imported = seq.importMidiSession(file, false);
    check((bool) imported.getProperty("ok", false), "session import");
    check(engine.getTrackIds().size() == 8, "separate parts appended");
    check(seq.getNotes().size() == 8, "notes and keyswitches preserved");
    check(engine.getTrackName(5) == engine.getTrackName(bass), "Unicode part name");
    check(engine.getTrackName(7) == engine.getTrackName(lead), "delimiter in part name");
    check(engine.getPluginName(6) == "Ample Guitar L" && engine.getPluginName(7) == "Ample Guitar L", "same instrument in separate parts");
    check(engine.getTrackName(8) == engine.getTrackName(empty), "empty named track");
    check(engine.getPluginPath(5) == "C:\\Missing\\ABU.vst3", "instrument path metadata");
    check(std::abs(seq.getBpm() - 93) < 0.001, "tempo roundtrip");
    const auto notes = seq.getNotes();
    for (size_t i = 0; i < 4; ++i)
    {
        check(notes[i].pitch == notes[i + 4].pitch, "pitch roundtrip");
        check(std::abs(notes[i].startBeat - notes[i + 4].startBeat) <= 1.0 / 960, "timing roundtrip");
        check(std::abs(notes[i].lengthBeats - notes[i + 4].lengthBeats) <= 2.0 / 960, "duration roundtrip");
        check(std::abs(notes[i].velocity - notes[i + 4].velocity) <= 1.0 / 127, "velocity roundtrip");
    }
    const auto invalid = directory.getChildFile("invalid.mid");
    invalid.replaceWithText("invalid");
    check(!(bool) seq.importMidiSession(invalid, false).getProperty("ok", true), "reject invalid input");
    check(seq.getNotes().size() == 8 && engine.getTrackIds().size() == 8, "invalid import leaves session intact");
    check(seq.importFromMidiFile(file, guitar), "legacy explicit-track import");
    check(seq.getNotes().size() == 12, "legacy merges notes");
    directory.getChildFile("result.txt").replaceWithText(failures.isEmpty() ? "PASS\n" : failures.joinIntoString("\n"));
    return failures.isEmpty();
}
