# Kemençe of the Black Sea — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **Bowed strings**
- Status: **Commercial**
- Pitch convention: **C4 = MIDI 60**
- Source: **01-keyswitch(7).sfz + 02-natural…11-noises.sfz**

## Articulations

- Natural
- Staccato
- Pizzicato
- Col legno battuto
- Sul ponticello
- Artificial harmonics
- Tremolo fast
- Tremolo slow
- Col legno tratto
- Noises

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Dynamic |
| 21 | Vibrato Depth |
| 104 | Tails Only |
| 105 | Polyphonic Switch |
| 106 | Attack Time |
| 107 | Release Time |
| 108 | Tail Volume |
| 109 | Vibrato Speed |
| 110 | Vibrato Delay |
| 111 | Vibrato Fade |
| 112 | Humanize |

## Performance rules

- Exact KS note assignments are in `keyswitch_maps/keyswitch_map.sfz`, not supplied here.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
