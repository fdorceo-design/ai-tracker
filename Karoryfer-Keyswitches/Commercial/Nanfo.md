# Nanfo — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **West African multi-instrument**
- Status: **Commercial**
- Pitch convention: **C4 = MIDI 60**
- Source: **01-seperewa.sfz through 14-flute.sfz**

## Keyswitch / trigger maps

### Seperewa

| MIDI | Note | Function |
|---:|---:|---|
| 26 | D1 | Tremolo |
| 27 | D#1 | Short |
| 28 | E1 | Long |

### Kora

| MIDI | Note | Function |
|---:|---:|---|
| 26 | D1 | Octave Tremolo |
| 27 | D#1 | Tremolo |
| 28 | E1 | Octave Short |
| 29 | F1 | Octave Long |
| 30 | F#1 | Short |
| 31 | G1 | Long |

### Kamele Ngoni

| MIDI | Note | Function |
|---:|---:|---|
| 36 | C2 | Tremolo |
| 37 | C#2 | Short |
| 38 | D2 | Long |

### Kafonu

| MIDI | Note | Function |
|---:|---:|---|
| 27 | D#1 | Tremolo |
| 28 | E1 | Octave Short |
| 29 | F1 | Octave Long |
| 30 | F#1 | Short |
| 31 | G1 | Long |

### Balafon

| MIDI | Note | Function |
|---:|---:|---|
| 36 | C2 | Tremolo |
| 37 | C#2 | Octave |
| 38 | D2 | Normal Hit |

### Small Xylophone

| MIDI | Note | Function |
|---:|---:|---|
| 44 | G#2 | Tremolo |
| 45 | A2 | Hit |

### Large Xylophone

| MIDI | Note | Function |
|---:|---:|---|
| 40 | E2 | Octave |
| 41 | F2 | Hit |

### Seperewa 14

| MIDI | Note | Function |
|---:|---:|---|
| 31 | G1 | Shake |
| 32 | G#1 | No Shake / Steady |
| 33 | A1 | Staccato |

### Kora 14

| MIDI | Note | Function |
|---:|---:|---|
| 40 | E2 | Muted |
| 41 | F2 | Ordinary / Long |

### Kavivi

| MIDI | Note | Function |
|---:|---:|---|
| 28 | E1 | Low Octave |
| 29 | F1 | Low Ordinary |

### Kologo Chromatic

| MIDI | Note | Function |
|---:|---:|---|
| 36 | C2 | Both / Extended |
| 37 | C#2 | One |

### Flute

| MIDI | Note | Function |
|---:|---:|---|
| 52 | E3 | Short |
| 53 | F3 | Long |

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Dynamics / Tremolo Dynamics where used |
| 13 | Mode / Drone control depending on program |
| 14 | Release or Bass-layer control depending on program |
| 21 | Vibrato on wind programs |
| 32 | Transpose on Kologo |
| 100 | Unison |
| 101 | Unison Width |
| 105 | Mono Switch on Atenteben/Flute |
| 114 | Vibrato Speed |
| 115 | Vibrato Fade |
| 116 | Vibrato Delay |
| 117 | Vibrato Humanize |
| 118 | Vibrato Breathiness |

## Performance rules

- Atenteben: explicit playable range visible in supplied SFZ = MIDI 48–72 (C3–C5) for its defined key regions.
- Flute default KS = MIDI 53 (F3 Long); Short = MIDI 52 (E3).
- Balafon/Small Xylophone hit layers use velocity 1–42 / 43–85 / 86–127.
- Large Xylophone: CC14 > 0 adds octave-down bass layer.
- Kologo Oct has no top-level keyswitch block in the supplied file.
- Exact playable ranges for the other pitched instruments are in included map files not supplied here.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
