# Spitfire Sacconi String Quartet — Keyswitch & CC Reference

Individual-patch keyswitch layout, empirically verified against the user's
actual Kontakt Standalone setup (Sacconi Quartet library, one instrument per
MIDI channel: Violin1=ch1, Violin2=ch2, Viola=ch3, Cello=ch4).

**Confirmed by direct test: the keyswitch zone starts at absolute MIDI note 0**
(not 12, not 24 — two earlier name-convention-based guesses were both wrong).
`MIDI 6` was sent and Kontakt's articulation display showed "Long", which
matches position 6 (0-indexed) in the list below starting from MIDI 0.
Ignore note-name/octave-label conventions for this table; the MIDI numbers
here are verified ground truth.

Send a Note On (velocity doesn't matter, use e.g. 0.8) on the keyswitch note
*before* the notes that should use that articulation — Kontakt keyswitches are
sticky (stay active until the next keyswitch is sent). A short/zero-length
note is enough; note-off timing doesn't matter for triggering the switch.

**Give the keyswitch a `startBeat` genuinely earlier than the note it should
affect (e.g. 0.05-0.1 beat) — never the same `startBeat`.** See
[CLAUDE.md](CLAUDE.md)'s composition conventions for why: same-beat ordering
relies on insertion order into the notes list, which is easy to get backwards
by accident (e.g. generating keyswitches in a pass that runs after the
melody notes).

## All four instruments (same layout, verified for Violin 1)

Violin 1, Violin 2, and Cello have all 14 rows; Viola has 12 (no trills).

| MIDI | Articulation |
|---|---|
| 0  | Spiccato |
| 1  | Staccato |
| 2  | Pizzicato |
| 3  | Pizzicato Bartok |
| 4  | Col Legno |
| 5  | Short Harmonics |
| 6  | **Long** (verified) |
| 7  | Marcato Attack |
| 8  | Long Flautando |
| 9  | Long Harmonics |
| 10 | Unmeasured Tremolo |
| 11 | Measured Tremolo (150 bpm) |
| 12 | Trill Major 2nd (Violin1/2, Cello only) |
| 13 | Trill Minor 2nd (Violin1/2, Cello only) |

## Quick reference

| Articulation | MIDI note (all instruments) |
|---|---|
| Long (sustain) | 6 |
| Staccato | 1 |
| Spiccato | 0 |
| Pizzicato | 2 |

## Continuous controllers (expression/nuance)

Confirmed by the user; each instrument's CONTROLLERS panel in the GUI
(Dynamics/Vibrato/Release/Expression sliders) maps to these:

| CC | Name | Effect |
|---|---|---|
| 1  | Mod wheel | Dynamics (soft↔loud within the current articulation) |
| 11 | Expression | Overall volume (0-100%) |
| 21 | Vibrato | Vibrato intensity |

Send via `POST /api/cc {trackId, controller, value, beat}` (`value` is raw
MIDI 0-127). Unlike a keyswitch note, a CC value is *not* one-shot in effect
— it stays at that value until the next CC on the same controller/track, so
it can be used for slow swells (multiple CC events ramping value over
several beats) as well as a single static level per phrase.

## Articulation-specific performance controls

For AI generation, do not treat all articulations as if they respond to the
same MIDI performance data.

General Spitfire behavior used here:

- **Long/sustained articulations**: primary dynamics = **CC1**.
- **Short articulations**: primary dynamics = **note velocity**.
- **CC11 Expression**: overall level trim on all articulations.
- **CC21 Vibrato**: articulation-dependent. Use it only where the selected
  articulation actually exposes useful vibrato states; do not assume that the
  visible Vibrato controller means every articulation responds.

| MIDI | Articulation | Primary dynamics | CC21 Vibrato |
|---:|---|---|---|
| 0 | Spiccato | Velocity | No practical use expected |
| 1 | Staccato | Velocity | No practical use expected |
| 2 | Pizzicato | Velocity | No |
| 3 | Pizzicato Bartok | Velocity | No |
| 4 | Col Legno | Velocity | No practical use expected |
| 5 | Short Harmonics | Velocity | No practical use expected |
| 6 | Long | CC1 | **Yes — confirmed** |
| 7 | Marcato Attack | Velocity (attack) | No practical use expected |
| 8 | Long Flautando | CC1 | **No — confirmed** |
| 9 | Long Harmonics | CC1 | **No — confirmed** |
| 10 | Unmeasured Tremolo | CC1 | Do not assume; verify |
| 11 | Measured Tremolo (150 bpm) | CC1 | Do not assume; verify |
| 12 | Trill Major 2nd | CC1 | Do not assume; verify |
| 13 | Trill Minor 2nd | CC1 | Do not assume; verify |

### AI rule

- For `Long`, shape musical dynamics with **CC1**, then use **CC11** only for
  additional level shaping. CC21 can be used for vibrato.
- For `Spiccato`, `Staccato`, `Pizzicato`, Bartok pizzicato, Col Legno and
  other short attacks, shape intensity primarily with **velocity**, not CC1.
- `Long Flautando` and `Long Harmonics` were checked in the user's running
  Sacconi patch and **do not respond to CC21**.
- Tremolo and Trill articulations are treated as non-CC21 techniques for AI
  generation; do not add vibrato automation to them.

## Kontakt UI notes learned along the way

- The small red/blue keyboard on the instrument header (red = keyswitch zone,
  blue = playable range) is purely cosmetic and can appear to reset/disappear
  without the underlying articulation state actually changing — don't use it
  to judge whether a keyswitch worked. Trust the articulation name label
  instead (top-left of the instrument panel, e.g. "Long", "Spiccato").
- Each instrument's OPTIONS panel has a "Reset on transport" toggle (resets
  articulation when transport starts) — turned out to be unrelated to the
  disappearing zone-color display in this case, but worth knowing it exists.

## Instrument ranges (from the Sacconi manual — manual's own octave labels,
convention unconfirmed; treat as approximate, not used for keyswitch math)

| Instrument | Range | Clef |
|---|---|---|
| Violin 1 | G3–C7 | Treble |
| Violin 2 | G3–C7 | Treble |
| Viola | C3–C6 | Alto/Treble |
| Cello | C2–A4 | Bass |

## Notes

- This layout is specific to this user's current Kontakt keyswitch bank
  position (adjustable via "Keyboard Shimmier" in Expert View) — if they move
  the keyswitch zone, this table needs updating (re-verify empirically,
  don't recompute from a note-name convention).
- All four instruments use the identical keyswitch layout — no
  per-instrument offset needed.
- AI Tracker's HTTP API (`POST /api/notes`) only sequences plain notes; a
  keyswitch is just a short note at the keyswitch pitch on the same
  track/channel, placed before the notes it should affect (in this project,
  in the reserved setup bar — see [CLAUDE.md](CLAUDE.md)).
- **History**: two earlier attempts at this table computed values from a
  note-name/octave convention (C4=60 giving Long=18, then C3=60 giving
  Long=30) — both were wrong and silently failed (no error, articulation
  just never changed). The lesson: for a specific user's Kontakt rig,
  verify keyswitch numbers empirically (send a note, read the articulation
  label) rather than computing them from a stated octave-naming convention.
