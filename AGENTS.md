# AI Tracker — notes for AI agents

If you're operating this app (composing via the HTTP API, or troubleshooting
its behavior), read this file. It's not tool-specific — the conventions
below apply regardless of which AI/agent you are.

## MIDI note number / octave-name convention

`pitch` in the API is always the raw MIDI note number (0-127) — that part is
unambiguous. Converting a spoken/written note *name* (e.g. "C0", "F#-1") to a
MIDI number is where things go wrong: different tools (and different Kontakt
preference settings) disagree on which octave "C0" or "middle C" means, and
getting it wrong shifts everything by a full octave *with no error* — it
just silently plays/switches the wrong thing.

**For Kontakt keyswitches specifically, don't compute the MIDI number from a
stated octave-naming convention at all — verify it empirically.** Send a
candidate note and read the actual articulation name Kontakt displays (top
of the instrument panel). Three different octave-naming assumptions were
tried in this project for the same keyswitch bank and all three were wrong;
what actually worked was sending `MIDI 0` upward directly (the keyswitch
zone's own articulation list, 0-indexed from absolute MIDI 0), no name
conversion involved. See
[Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md) for the
verified table and how it was confirmed.

Kontakt UI note: the small red/blue range-indicator keyboard on an
instrument's header can visually reset/disappear without the underlying
articulation actually changing — it's not a reliable signal. Trust the
articulation name label instead.

**Launch order**: when routing tracks to an external standalone (see
[MIDI.md](MIDI.md)), AI Tracker must be launched *before* the standalone app,
not after — confirmed repeatedly by the user with Kontakt Standalone;
launching it first left the connection unreliable/silent even with the
virtual MIDI port, its enabled state, and the channel assignment all looking
correct. Treat this as a general rule for any standalone routed this way
(only actually confirmed with Kontakt so far, but the user's own read is
that it's likely universal, not Kontakt-specific). If asked to launch the
standalone app (`POST /api/launch-app`), it's already safe by construction
since AI Tracker exists first — but don't tell the user to start the
standalone app manually before AI Tracker is already running.

## Composition conventions

- Beats are 0-indexed. Bar lines are normally `floor(startBeat / beatsPerBar)`,
  but if any time-signature events are scheduled (see below), bar indices
  follow the meter map instead — always trust `getBarIndexForBeat` / the
  tracker UI's bar lines over a hand computation once a piece has a meter
  change. `startBeat: 0.0` sits in **bar index 0** — the band above the
  *first* bar line in the tracker UI. Never place notes at a negative
  `startBeat`: the sequencer only fires notes whose `startBeat` is `>=` the
  current playback position, and playback starts at position 0.0, so
  anything before that never triggers.
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
- **A mid-piece articulation change is different: don't put the keyswitch at
  the exact same `startBeat` as the note it's meant to affect**, even though
  the sequencer processes same-tick notes in vector/insertion order (so a
  keyswitch added *before* the colliding note in API-call order would, in
  principle, still fire first within that tick). It's fragile in practice —
  easy to get the insertion order backwards without noticing (e.g. building
  the keyswitch list in a separate pass *after* the melody notes, which then
  land later in the vector, meaning after the note on a tie). Give the
  keyswitch a `startBeat` a little *before* the note instead (e.g. 0.05-0.1
  beat earlier) so it's genuinely separated in time, not just in insertion
  order.
- Default: auto-generated pieces occupy bar 0 for setup only and start their
  actual music at bar 1. Don't add extra silent bars beyond that.

## Expression: CC, tempo, and time-signature events

- `POST /api/cc {trackId, controller, value, beat}` sends a one-shot MIDI CC
  (0-127 controller and value). It has no duration — the value holds until
  the next CC on that controller/track. Useful CCs on the Sacconi library:
  CC1 dynamics, CC11 expression/overall volume, CC21 vibrato intensity. A
  swell is just several CC events at increasing beats/values.
- `POST /api/tempo-events {beat, bpm}` schedules a tempo change (rubato,
  accelerando/ritardando as a step function — for a gradual change, place
  several events close together). Not per-track; affects the whole piece.
- `POST /api/timesig-events {beat, numerator, denominator}` schedules a
  meter change (denominator defaults to 4 if omitted; `beatsPerBar` also
  still works as an alias for `numerator`). A bar's length in quarter-note
  beats is `numerator * 4.0 / denominator`, so e.g. 3/8 is 1.5 beats/bar —
  fractional bar lengths are fine. **Must land exactly on a bar boundary of
  the previously-active meter** — the bar-index math assumes this and will
  misalign the tracker grid if not. `POST /api/transport/timesignature`
  (the non-scheduled, immediate version) takes the same fields.
- All three follow the same GET/DELETE/`.../clear` pattern as `/api/notes`.

## Reference docs

- [README.md](README.md) — what this project is, build/run instructions.
- [MIDI.md](MIDI.md) — MIDI import/export and external MIDI routing
  (loopMIDI fallback for VST3 plugins that crash when hosted, e.g. Kontakt).
- [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md) — keyswitch
  note numbers for the user's Spitfire Sacconi Quartet setup (empirically
  verified — see the file's own history note on why that mattered), so they
  don't need to be re-derived from the manual each session.
- [Kontakt-Spitfire-Solo-Strings-Keyswitches.md](Kontakt-Spitfire-Solo-Strings-Keyswitches.md) —
  keyswitches for the six Solo Strings startup patches (screenshot-verified).
- [Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md](Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md) —
  keyswitches for SSO's factory All Techniques patches (strings, brass,
  woodwinds, 2025 Solo Strings). **Factory-map derivation, not per-row
  empirically verified** — confirm against the actual running instance
  before relying on it, same caveat as the PP013 doc below.
- [Kontakt-BBCSO-Professional-Keyswitches.md](Kontakt-BBCSO-Professional-Keyswitches.md) —
  BBC Symphony Orchestra Professional, work in progress. **Uses a different
  octave-naming convention than every other doc here** (`C4 = MIDI 60`,
  confirmed empirically via Violins 1 and Horn — not the `C-2 = MIDI 0`
  convention the Sacconi/Solo Strings/SSO/PP013 docs use). Kontakt's octave
  display is a per-library/per-instance setting; don't assume one doc's
  convention carries over to another.
- [Kontakt-PP013-Electric-Cellist-Keyswitches.md](Kontakt-PP013-Electric-Cellist-Keyswitches.md) —
  keyswitches and playable ranges for Peter Gregson Electric Cellist PP013,
  checked against the user's running instance.
- [Ample-Sound-Keyswitches/](Ample-Sound-Keyswitches/README.md) — Ample
  Sound bass/guitar/ukulele/metal libraries (24 instruments). Sourced
  from the official Ample Sound manual per instrument, not empirically
  re-verified against a running instance. Uses `C4 = MIDI 60` (same
  convention as BBCSO, not the Sacconi/Solo Strings/SSO/PP013
  `C-2 = MIDI 0` convention).
- [MODWHEEL-Keyswitches/](MODWHEEL-Keyswitches/README.md) — MODWHEEL
  Kontakt libraries (22 products). Most of these are **patch-specific,
  not one universal keyswitch map** — don't assume a MIDI note number
  that isn't explicitly published in a given file; where the doc says a
  number isn't published, load the patch and check its GUI/manual rather
  than guessing.
