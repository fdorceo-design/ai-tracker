[![AI Tracker](ai-tracker-banner.png)](https://github.com/fdorceo-design/ai-tracker)

# AI Tracker

A Windows tracker-style sequencer and VSTi/Kontakt host, built so an AI can
operate it directly over a plain HTTP API while a human watches (and edits)
the same session through a Tracker-style GUI. JUCE/C++, CMake build.

- **For humans**: run the app, add tracks, load an instrument (VST3 or route
  to an external MIDI/standalone synth), and either play with the GUI or let
  an AI drive it through the API below.
- **For an AI operating this app**: everything you need is `POST`/`GET`
  JSON over `http://127.0.0.1:8080` once the app is running — no code
  changes required to compose or play. Read
  **[AGENTS.md](AGENTS.md)** first; it has the API conventions that matter
  (beat numbering, bar-0 setup convention, keyswitch timing) and links to
  the rest of the reference docs below. That's the right file regardless of
  which AI/agent you are.

![AI Tracker default state](ai-tracker-default-state.png)

## Quick start

Build (CMake + Visual Studio, JUCE fetched automatically):

```bash
cmake -S . -B build
cmake --build build --config Release --target AiTracker AiTrackerPluginServer
```

Run `build/AiTracker_artefacts/Release/AI Tracker.exe` — keep
`AiTrackerPluginServer.exe` in the same folder (it's copied there
automatically by the build). The app opens an HTTP API on port 8080 and a
GUI window at the same time; both operate the same live session.

Prebuilt binaries are also available as a zip in the repo — it only needs
`AI Tracker.exe` and `AiTrackerPluginServer.exe` together in the same
folder to run.

## License

[CC BY-NC 4.0](LICENSE) — copyright is retained; noncommercial use,
redistribution, and modification are permitted with attribution.

## Reference docs

- **[AGENTS.md](AGENTS.md)** — composition/API conventions for an AI
  operating this app (start here if you're an AI, not a human).
- **[MIDI.md](MIDI.md)** — MIDI import/export, and the external-MIDI-routing
  fallback for VST3 instruments that crash when hosted directly (e.g.
  Kontakt) — route to a standalone instance over a virtual MIDI cable
  instead.
- Per-instrument-library keyswitch/CC references (MIDI note numbers for
  articulation switching, which CC does what, playable ranges) — each one
  states its own octave-naming convention and verification status, since
  both vary per library/instance and get it wrong silently:
  - [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md)
  - [Kontakt-Spitfire-Solo-Strings-Keyswitches.md](Kontakt-Spitfire-Solo-Strings-Keyswitches.md)
  - [Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md](Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md)
  - [Kontakt-PP013-Electric-Cellist-Keyswitches.md](Kontakt-PP013-Electric-Cellist-Keyswitches.md)
  - [Kontakt-BBCSO-Professional-Keyswitches.md](Kontakt-BBCSO-Professional-Keyswitches.md)
    (work in progress; **uses a different octave convention than the others**
    — read its own convention section before using it)
