# AI Tracker — notes for Claude

## Composition conventions

- Beats are 0-indexed and bar lines are computed as
  `floor(startBeat / beatsPerBar)`, so `startBeat: 0.0` sits in **bar index 0**
  — the band above the *first* bar line in the tracker UI. Never place notes
  at a negative `startBeat`: the sequencer only fires notes whose `startBeat`
  is `>=` the current playback position, and playback starts at position 0.0,
  so anything before that never triggers.
- **Bar index 0 is reserved for preparatory/setup data only** — articulation
  keyswitches, program changes, anything that needs to land before the music
  starts — never actual musical content. The composed piece itself begins at
  **bar index 1** (`startBeat: beatsPerBar`, e.g. `4.0` in 4/4), i.e. right at
  the *first* bar line, matching a non-anacrusis piece's true beat 1 with no
  ambiguity. (A piece with a written pickup/anacrusis is the one exception —
  place its pickup notes at the end of bar 0, immediately before the
  bar-1 downbeat, not spread across bar 0.)
- When a track needs an articulation keyswitch (see
  [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md)), add it as
  a short note early in bar 0 (e.g. `startBeat: 0.0`, `lengthBeats: 1.0` is
  plenty of room) — it never needs to compete for timing with bar 1's first
  note since they're a full bar apart.
- Default: auto-generated pieces occupy bar 0 for setup only and start their
  actual music at bar 1. Don't add extra silent bars beyond that.

## Reference docs

- [MIDI.md](MIDI.md) — MIDI import/export and external MIDI routing
  (loopMIDI fallback for VST3 plugins that crash when hosted, e.g. Kontakt).
- [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md) — keyswitch
  note numbers for the user's Spitfire Sacconi Quartet setup, so they don't
  need to be re-derived from the manual each session.
