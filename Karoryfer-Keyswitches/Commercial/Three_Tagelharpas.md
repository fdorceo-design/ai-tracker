# Three Tagelharpas — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **Bowed strings**
- Status: **Commercial**
- Pitch convention: **C4 = MIDI 60**
- Source: **Main programs plus supplied 0101–0702 articulation/texture SFZ files**

## Keyswitch / trigger maps

### Tagelharpa

| MIDI | Note | Function |
|---:|---:|---|
| 36 | C2 | Tremolo |
| 37 | C#2 | Short |
| 38 | D2 | Long |
| 39 | D#2 | Mid |
| 40 | E2 | Portamento Long |
| 41 | F2 | Portamento Mid |
| 42 | F#2 | Legato Long |
| 43 | G2 | Legato Mid |

### Stråkharpa

| MIDI | Note | Function |
|---:|---:|---|
| 29 | F1 | Tremolo |
| 30 | F#1 | Short |
| 31 | G1 | Long |
| 32 | G#1 | Mid |
| 33 | A1 | Portamento Long |
| 34 | A#1 | Portamento Mid |
| 35 | B1 | Legato Long |
| 36 | C2 | Legato Mid |

### Bass Tagelharpa

| MIDI | Note | Function |
|---:|---:|---|
| 29 | F1 | Tremolo |
| 30 | F#1 | Short |
| 31 | G1 | Long |
| 32 | G#1 | Mid |
| 33 | A1 | Portamento Long |
| 34 | A#1 | Portamento Mid |
| 35 | B1 | Legato Long |
| 36 | C2 | Legato Mid |

### Tagelharpa Texture

| MIDI | Note | Function |
|---:|---:|---|
| 36 | C2 | Wild B |
| 37 | C#2 | Wild A |
| 38 | D2 | Smooth B |
| 39 | D#2 | Smooth A |
| 40 | E2 | Wild |
| 41 | F2 | Smooth |
| 42 | F#2 | Layered |

### Stråkharpa Texture

| MIDI | Note | Function |
|---:|---:|---|
| 34 | A#1 | Burst |
| 35 | B1 | Smooth |
| 36 | C2 | Layered |

### Bass Tagelharpa Texture

| MIDI | Note | Function |
|---:|---:|---|
| 34 | A#1 | Wild |
| 35 | B1 | Smooth |
| 36 | C2 | Layered |

### All Textures

| MIDI | Note | Function |
|---:|---:|---|
| 35 | B1 | Evolving |
| 36 | C2 | Stacked |

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Dynamics |
| 21 | Vibrato Switch |
| 23 | Drone Preference |
| 32 | Transpose |
| 107 | Release |

## Performance rules

- Tagelharpa default = MIDI 43 (G2 Legato Mid).
- Stråkharpa/Bass Tagelharpa default = MIDI 36 (C2 Legato Mid).
- CC21 split: 64–127 vibrato; 0–63 non-vibrato.
- Supplied articulation files confirm long/mid layers can add a short layer at velocity 41–127.
- Exact playable ranges are in referenced map files not supplied here.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
