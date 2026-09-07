# Spitfire Sacconi String Quartet — Keyswitch Reference

Individual-patch keyswitch layout, as confirmed directly by the user from their
Kontakt Standalone setup (Sacconi Quartet library, one instrument per MIDI
channel: Violin1=ch1, Violin2=ch2, Viola=ch3, Cello=ch4). Keyswitches start at
**C-2 = MIDI note 0** and are laid out chromatically ascending, one keyswitch
per semitone, in the order listed below.

Send a Note On (velocity doesn't matter, use e.g. 100) on the keyswitch note
*before* the notes that should use that articulation — Kontakt keyswitches are
sticky (stay active until the next keyswitch is sent).

## Violin 1 (15 articulations)

| MIDI | Note | Articulation |
|---|---|---|
| 0  | C-2  | Long Flautando |
| 1  | C#-2 | Long Harmonics |
| 2  | D-2  | **Long** |
| 3  | D#-2 | Marcato Attack |
| 4  | E-2  | Measured Tremolo (150 bpm) |
| 5  | F-2  | Playable |
| 6  | F#-2 | Short Bartok Pizz |
| 7  | G-2  | Short Col Legno |
| 8  | G#-2 | Short Harmonics |
| 9  | A-2  | Short Pizzicato |
| 10 | A#-2 | Short Spiccato |
| 11 | B-2  | **Short Staccato** |
| 12 | C-1  | Trill Major 2nd |
| 13 | C#-1 | Trill Minor 2nd |
| 14 | D-1  | Unmeasured Tremolo |

## Violin 2 / Viola / Cello (13–14 articulations)

Violin 2 and Cello have 14 entries (with both trills); Viola has 13 (no
trills, ends at Unmeasured Tremolo one step earlier).

| MIDI | Note | Articulation |
|---|---|---|
| 0  | C-2  | Long Flautando |
| 1  | C#-2 | Long Harmonics |
| 2  | D-2  | Long Measured Tremolo (150 bpm) |
| 3  | D#-2 | **Long** |
| 4  | E-2  | Marcato Attack |
| 5  | F-2  | Short Bartok Pizz |
| 6  | F#-2 | Short Col Legno |
| 7  | G-2  | Short Harmonics |
| 8  | G#-2 | Short Pizzicato |
| 9  | A-2  | Short Spiccato |
| 10 | A#-2 | **Short Staccato** |
| 11 | B-2  | Trill Major 2nd (Viola: Unmeasured Tremolo, no trills) |
| 12 | C-1  | Trill Minor 2nd (Violin2/Cello only) |
| 13 | C#-1 | Unmeasured Tremolo (Violin2/Cello only) |

## Quick reference: "Long" and "Short Staccato" keyswitch per instrument

| Instrument | Long | Short Staccato |
|---|---|---|
| Violin 1 | D-2 (2) | B-2 (11) |
| Violin 2 | D#-2 (3) | A#-2 (10) |
| Viola | D#-2 (3) | A#-2 (10) |
| Cello | D#-2 (3) | A#-2 (10) |

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
- UACC (CC#32) is a position-independent alternative documented in the Sacconi
  manual, but requires each instrument's articulation lock set to "Locked to
  UACC" mode first; not used here since keyswitch note numbers are now known.
- AI Tracker's HTTP API (`POST /api/notes`) only sequences plain notes; a
  keyswitch is just a very short/zero-length note at the keyswitch pitch on
  the same track/channel, placed slightly before the notes it should affect.
