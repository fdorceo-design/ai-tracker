# Ample Guitar RB — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://amplesound.net/en/Main_Panel_Manual-AGRB.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| PH | Pinch Harmonic | B-1 | 11 | E1–upper range |  |
| Sus | Sustain & Pop | C0 | 12 | E1–upper range | velocity <127 Sustain; 127 = Pop/PH according to product/settings |
| NH | Natural Harmonic | C#0 | 13 | instrument range |  |
| PM | Palm Mute | D0 | 14 | instrument range | lower velocity gives stronger/deeper mute on applicable models |
| SIO | Slide In & Slide Out | D#0 | 15 | instrument range | KS before note = Slide In; while sounding = Slide Out |
| LS | Legato Slide (Poly Legato) | E0 | 16 | instrument range | overlapping notes; destination velocity controls slide speed |
| HP | Hammer-On & Pull-Off (Poly Legato) | F0 | 17 | instrument range | overlapping notes |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---:|---|---|
| B6 | 95 | Position Mode | low velocity = Melody Mode; high velocity = Chord Mode |
| G#6 | 92 | Open String First | high velocity = ON; low velocity = OFF |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| G0–C1 | — | String Assignment | 6th → 1st string; high velocity may affect fret position |
| C#1 | 25 | Position Assignment trigger | follow with a position note; exact selectable upper bound varies by instrument |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| F5 | 77 | Scratch |
| F#5 | 78 | Slap |
| G5 | 79 | Muting |
| G#5 | 80 | Strum Mute |
| A5 | 81 | Downstroke Noise 1 |
| A#5 | 82 | Upstroke Noise 1 |
| B5 | 83 | Downstroke Noise 2 |
| C6 | 84 | Upstroke Noise 2 |
| F6 | 89 | Hit Top / body hit (product dependent) |
| F#6 | 90 | Muted body hit / body FX (product dependent) |
| G6 | 91 | Rim/body FX (product dependent) |

## 4. Operational notes

- C0 + D0 together enables the Sustain/Palm-Mute mixed articulation where supported.
- Default vibrato behavior and depth are controlled in Settings; manuals document D#6 for Auto Legato and G#6 for Open String First.
- Product-specific upper playable ranges and some body-FX labels differ between models; where not explicitly verified here they are left generic rather than invented.

## 5. Verification status

- Core keyswitch names and standard v4 behavior are based on the official current Ample Sound manuals.
- Raw MIDI Note No. values are derived from C4=60.
- Product-specific values not explicitly verified are left generic rather than guessed.
