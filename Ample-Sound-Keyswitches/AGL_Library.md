# Ample Guitar L (AGL) — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://www.amplesound.net/en/Main_Panel_Manual-AGL.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Pop | C0 | 12 | E1–C5 | Velocity <127 Sustain; velocity 127 Pop |
| NH | Natural Harmonic | C#0 | 13 | E2–C5 |  |
| PM | Palm Mute | D0 | 14 | E1–C5 | Lower velocity = deeper mute |
| SIO | Slide In & Slide Out | D#0 | 15 | F#1–C5 |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | F1–C5 |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | E1–C5 |  |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---|---|---|
| B6 | 95 | Position Mode | low velocity = Melody; high velocity = Chord |
| G#6 | 92 | Open String First | high velocity ON; low velocity OFF |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| G0–C1 | — | String Assignment | 6th→1st string |
| C#1 | 25 | Position Assignment trigger | then E1–G#2 selects position |

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
| F6 | 89 | Hit Top (Open) |
| F#6 | 90 | Hit Top (Mute) |
| G6 | 91 | Hit Rim |

## 4. Pattern / repeater notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| D6 | 86 | Note Repeater |

## 5. Operational notes

- C0 + D0 together: mixed Sustain/Palm Mute mode.
- Open String First affects E1, A1, D2, G2, B2, E3.
- Default vibrato mode is Auto Vibrato; vibrato parameters are in Settings.
- Manual lists Hold Pedal Toggle but does not give a MIDI note in the main-panel text.

## 6. Verification status

- Pitch names/ranges/functions: transcribed from the official current Ample Sound Main Panel Manual.
- Raw MIDI Note No.: derived from the AI Tracker convention C4=60.
- Where the official manual does not provide a concrete MIDI note/value, no value has been invented.
