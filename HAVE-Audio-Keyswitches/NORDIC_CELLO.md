# NØRDIC CELLO — HAVE Audio AI Tracker Reference

Updated: 2026-09-08

- Instrument/type: **Cello**
- Source: **Official NØRDIC CELLO User Manual supplied by user + supplied NKI for supplemental articulation-name verification**
- Pitch convention: **C4 = MIDI 60**

## Main melodic modes

- Legato
- Harmony of the North
- Staccato
- Pizzicato
- Tremolo
- Sliding Fourths
- Sliding Down
- Mixed Articulations

## Performance controls relevant to AI Tracker

| Control | Function |
|---|---|
| CC1 / Mod Wheel | Dynamics. Long/sustained articulations crossfade dynamic layers. On short sounds such as Staccato/Pizzicato it controls volume instead. |
| Note Velocity | Short sounds respond to playing velocity for softer/louder dynamics. |
| Sustain Pedal (CC64) | Supported on Harmony of the North. |
| Legato Speed | Controls interval-transition responsiveness in the Legato mode. Exact MIDI CC is not factory-fixed in the manual. |

## Harmony of the North — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Sustain Vibrato |
| 2 | Bowed Harmonics |
| 3 | Still Water |
| 4 | Hendrix |
| 5 | Crescendo |

## Staccato — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Nordic Staccato |
| 2 | Harsh Staccato |
| 3 | Spiccato with Bow Tip |
| 4 | Alter Ego |
| 5 | Fast Crescendo |

## Pizzicato — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Long Pizzicato |
| 2 | Short Pizzicato |
| 3 | Pizzicato Harmonics |
| 4 | Vibrato Pizzicato |
| 5 | Slap |
| 6 | Fret Noises and FX |

## Sliding Down — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Descending 5ths |
| 2 | Descending minor 6ths |
| 3 | Descending major 6ths |
| 4 | Descending Major Chords |
| 5 | Descending Minor Chords |

## Mixed Articulations — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Legato |
| 2 | Staccato |
| 3 | Long Pizzicato |
| 4 | Short Pizzicato |
| 5 | Harmonics |
| 6 | Tremolo |

## Texture modes relevant to triggering

- Body Hits
- Bow Textures
- Low Engines
- Rises
- Bouncing

### Bow Textures — keyswitch slot order

| KS slot | Articulation |
|---:|---|
| 1 | Textures |
| 2 | Textural Loops |

## Time-based behavior

- TIME WARP affects Harmony of the North keyswitches 2–5, Sliding Fourths, Sliding Down, Bow Textures, Low Engines, Rises and Bouncing.
- Body Hits are percussive one-shots and are not affected by TIME WARP.

## Exact KS-note status

- The official manual identifies keyswitches by **slot number** and visually shows the yellow keys, but does not print their pitch names/MIDI note numbers.
- The supplied NKI confirms many articulation/group names but does not provide a sufficiently reliable factory note-number table for these KS slots.
- Therefore exact MIDI KS notes are intentionally left unset rather than inferred from the screenshot.

## NKI-confirmed supplemental group names

- Nordlegato / Nordlegato Open
- Pizz Harm / Pizz Long / Pizz Short / Pizz Slap / Pizz Vib
- Sliding4ths
- Spicc / Spicc Tip
- Stillwater Sustain
- Bodyhits / Bow Textures

## Verification

- Dynamics/velocity behavior: official manual page 8.
- Legato Speed: page 15.
- Harmony of the North: page 16.
- Staccato: page 18.
- Pizzicato: page 20.
- Sliding Down: page 25.
- Mixed Articulations: page 26.
- Bow Textures: page 29.
