# Vengeful Bass — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **Bass**
- Status: **Commercial**
- Pitch convention: **C4 = MIDI 60**
- Source: **Vengeful Bass.bank.xml + 01-keyswitch.sfz**

## Keyswitches

| MIDI | Note | Articulation |
|---:|---:|---|
| 35 | B1 | Sustain |
| 34 | A#1 | Staccato |
| 33 | A1 | Sul pont sustain |
| 32 | G#1 | Sul pont staccato |
| 31 | G1 | Molto sul pont sustain |
| 30 | F#1 | Molto sul pont staccato |
| 29 | F1 | Sul tasto sustain |
| 28 | E1 | Sul tasto staccato |
| 27 | D#1 | Ragged tremolo |
| 26 | D1 | Wind noise |
| 25 | C#1 | Spiccato |
| 24 | C1 | Tip spiccato |
| 23 | B0 | Ghost spiccato |
| 22 | A#0 | Jete |
| 21 | A0 | Ghost jete |
| 20 | G#0 | Colle |
| 19 | G0 | Chop |
| 18 | F#0 | Ricochet chop |
| 17 | F0 | Diagonal bowing |
| 16 | E0 | Pizz ord |
| 15 | D#0 | Pizz ord ghost |
| 14 | D0 | Pizz fat |
| 13 | C#0 | Pizz fat ghost |
| 12 | C0 | Harmonics |
| 11 | B-1 | Unclean harmonics |
| 10 | A#-1 | Pizz harmonics |

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Dynamics |
| 21 | Vibrato Depth |
| 105 | Legato Switch |
| 106 | Attack Time |
| 107 | Release Time |
| 109 | Release Vol |
| 112 | Vibrato Speed |
| 115 | Vibrato Delay |
| 116 | Vibrato Fade |
| 117 | Humanize |

## Performance rules

- KS range: MIDI 10–35; default = 35 (B0 Sustain).
- CC105: 0–63 Poly; 64–127 Mono/Legato.
- Pitch bend in the main playable program: ±12 semitones; harmonic/pizz-harmonic regions can disable bend.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
