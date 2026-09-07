# Spitfire Sacconi String Quartet — Keyswitch Reference

Individual-patch keyswitch layout, confirmed directly by the user from their
Kontakt Standalone setup (Sacconi Quartet library, one instrument per MIDI
channel: Violin1=ch1, Violin2=ch2, Viola=ch3, Cello=ch4).

**MIDI note numbers use the C4=60 convention** (i.e. middle C = 60, so
keyswitch start C0 = MIDI note 12). This is the same numbering AI Tracker's
`POST /api/notes` `pitch` field uses, so these numbers can be sent as-is.

Send a Note On (velocity doesn't matter, use e.g. 100) on the keyswitch note
*before* the notes that should use that articulation — Kontakt keyswitches are
sticky (stay active until the next keyswitch is sent).

## All four instruments (same layout)

Violin 1, Violin 2, and Cello have all 14 rows; Viola has 12 (no trills).

| MIDI | Note | Articulation |
|---|---|---|
| 12 | C0  | Spiccato |
| 13 | C#0 | Staccato |
| 14 | D0  | Pizzicato |
| 15 | D#0 | Pizzicato Bartok |
| 16 | E0  | Col Legno |
| 17 | F0  | Short Harmonics |
| 18 | F#0 | **Long** |
| 19 | G0  | Marcato Attack |
| 20 | G#0 | Long Flautando |
| 21 | A0  | Long Harmonics |
| 22 | A#0 | Unmeasured Tremolo |
| 23 | B0  | Measured Tremolo (150 bpm) |
| 24 | C1  | Trill Major 2nd (Violin1/2, Cello only) |
| 25 | C#1 | Trill Minor 2nd (Violin1/2, Cello only) |

## Quick reference

| Articulation | MIDI note (all instruments) |
|---|---|
| Long (sustain) | 18 (F#0) |
| Staccato | 13 (C#0) |
| Spiccato | 12 (C0) |
| Pizzicato | 14 (D0) |

## Instrument ranges (from the Sacconi manual)

| Instrument | Range | Clef |
|---|---|---|
| Violin 1 | G3–C7 | Treble |
| Violin 2 | G3–C7 | Treble |
| Viola | C3–C6 | Alto/Treble |
| Cello | C2–A4 | Bass |

## Notes

- This layout is specific to this user's current Kontakt keyswitch bank
  position (adjustable via "Keyboard Shimmier" in Expert View) — if they move
  the keyswitch zone, this table needs updating.
- All four instruments use the identical keyswitch layout, unlike what was
  first assumed — no per-instrument offset needed.
- AI Tracker's HTTP API (`POST /api/notes`) only sequences plain notes; a
  keyswitch is just a very short/zero-length note at the keyswitch pitch on
  the same track/channel, placed slightly before the notes it should affect.
