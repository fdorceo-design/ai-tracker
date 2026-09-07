# War Tuba — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **Brass / Hybrid**
- Status: **Installed / supplied**
- Pitch convention: **C4 = MIDI 60**
- Source: **1-solo-legato.sfz through 7-tubaborg.sfz**

## Articulations

- Sustain
- Staccato
- Staccatissimo

## Keyswitch / trigger maps

### Solo/Duo/Trio core

| MIDI | Note | Function |
|---:|---:|---|
| 24 | C1 | Core articulation slot 1 |
| 25 | C#1 | Core articulation slot 2 / default |
| 26 | D1 | Core articulation slot 3 |

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Sustain Dynamics |
| 69 | Attack |
| 70 | Legato Attack on Legato patches |
| 71 | Release |
| 111 | Vibrato → Pitch |
| 112 | Vibrato Speed |
| 114 | Vibrato → EQ |
| 115 | Vibrato Delay |
| 116 | Vibrato Fade |
| 117 | Unsteadiness |

## Performance rules

- KS range = MIDI 24–26; default = MIDI 25.
- The supplied top-level files do not identify which KS note corresponds to Sustain/Staccato/Staccatissimo, so that mapping is not guessed.
- Duo/Trio additionally expose Detune and Width controls.
- Tubaborg is a separate synth/hybrid patch: CC121 Mono Switch, CC120 Portamento Time, pitch bend ±12 semitones.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
