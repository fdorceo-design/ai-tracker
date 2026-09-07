# AI Tracker — notes for Claude

## Composition conventions

- Beats are 0-indexed: `startBeat: 0.0` is beat 1 of **measure 1**. There is
  no "measure 0" — never place notes (or keyswitch-priming notes) at a
  negative `startBeat` to sneak them in "before" the piece starts; the
  sequencer only fires notes whose `startBeat` is `>= ` the current playback
  position, and playback starts at position 0.0, so anything before that
  never triggers.
- When a track needs an articulation keyswitch primed before its first
  melodic note (see [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md)),
  put the keyswitch at `startBeat: 0.0` with a very short `lengthBeats`
  (e.g. `0.02`), and start the actual melodic content a little after it
  (e.g. `startBeat: 0.05` onward) — still within measure 1, not before it.
- Default: auto-generated pieces begin at measure 1, beat 0.0. Don't add
  silent lead-in measures unless the user asks for one.

## Reference docs

- [MIDI.md](MIDI.md) — MIDI import/export and external MIDI routing
  (loopMIDI fallback for VST3 plugins that crash when hosted, e.g. Kontakt).
- [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md) — keyswitch
  note numbers for the user's Spitfire Sacconi Quartet setup, so they don't
  need to be re-derived from the manual each session.
