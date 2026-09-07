# Ample Bass Upright (ABU) — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://www.amplesound.net/en/Main_Panel_Manual-ABU.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Accent | C0 | 12 | B0–F4 | Vel 1–125 Sustain; 126–127 Accentuation |
| NH | Natural Harmonic | C#0 | 13 | E1–D4 |  |
| PM | Palm Mute | D0 | 14 | E1–F4 |  |
| SIO | Slide In & Slide Out | D#0 | 15 | F#1–F4 | KS before note = Slide In; KS while sounding = Slide Out |
| LS | Legato Slide (Poly Legato) | E0 | 16 | F1–F4 | Overlapping notes; velocity controls slide speed |
| HP | Hammer-On & Pull-Off (Poly Legato) | F0 | 17 | E1–F4 | Overlapping notes |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---|---|---|
| B6 | 95 | Position Mode | low velocity = Melody; high velocity = Chord |
| A4 | 69 | Buzzing | buzz all currently ringing notes |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| E6–G6 | — | String Assignment | 4th→1st string |
| A#0 | 22 | Position Assignment trigger | then E1–A#2 selects position |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| F#4 | 66 | Hit Rim (Mute) |
| G4 | 67 | Hit Top (Palm Mute) |
| G#4 | 68 | Hit Top (Finger Mute) |
| A4 | 69 | Hit Top (Open) |
| A#4 | 70 | Hit Rim (Open) |
| F5 | 77 | Scratch |
| F#5 | 78 | Breath |
| G5 | 79 | Single String Slap |
| G#5 | 80 | Left-Hand Slap Noise |
| A5 | 81 | Right-Hand Slap Noise |
| A#5 | 82 | FX Slide Turn 4 |
| B5 | 83 | FX Slide Turn 3 |
| C6 | 84 | FX Slide Down 4 |
| C#6 | 85 | FX Slide Down 3 |

## 4. Pattern / repeater notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| B4 | 71 | Octave Pattern: descending fourth |
| C5 | 72 | Octave Pattern: unison |
| D5 | 74 | Octave Pattern: ascending fifth |
| E5 | 76 | Octave Pattern: ascending octave |
| D#5 | 75 | Note Repeater |

## 5. Operational notes

- Mod wheel controls vibrato; vibrato noise at value ≥ 0.75.
- Manual lists Hold Pedal Toggle but does not give a MIDI note in the main-panel text.

## 6. Verification status

- Pitch names/ranges/functions: transcribed from the official current Ample Sound Main Panel Manual.
- Raw MIDI Note No.: derived from the AI Tracker convention C4=60.
- Where the official manual does not provide a concrete MIDI note/value, no value has been invented.
