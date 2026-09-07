# Ample Bass Jaco Fretless (ABJF) — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://www.amplesound.net/en/Main_Panel_Manual-ABJF.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Accent | C0 | 12 | B0–E4 | Vel 1–125 Sustain; 126–127 Accentuation |
| NH | Natural Harmonic | C#0 | 13 | E1–D4 |  |
| PM | Palm Mute | D0 | 14 | E1–E4 | High velocity Palm Mute; low velocity Dead Note |
| SIO | Slide In & Slide Out | D#0 | 15 | F#1–F4 |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | F1–D#4 |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | F1–D#4 |  |
| Rep | Repeat Note | F#0 | 18 | E1–E4 |  |
| Slap | Slap / Slap Dead Note | G0 | 19 | E1–E4 | vel 64–127 Slap; 1–63 Dead Slap |
| AH | Artificial Harmonic | G#0 | 20 | E1–E4 |  |
| Pop | Pop / Pop Dead Note | A0 | 21 | E1–E4 | vel 64–127 Pop; 1–63 Dead Pop |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---|---|---|
| B6 | 95 | Position Mode | low velocity Melody; high velocity Chord |
| A4 | 69 | Buzzing | buzz ringing notes |
| D#6 | 87 | Auto Legato Mode | high velocity Auto Slide; low velocity Auto HP |
| E6–G6 | — | String Assignment | 4th→1st string |
| A#0 | 22 | Position Assignment trigger | then E1–G#2 selects position |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| F5 | 77 | Scratch 1 |
| F#5 | 78 | Scratch 2 |
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
| C#5 | 73 | Note Repeater |
| D#5 | 75 | Note Repeater |

## 5. Operational notes

- C0 + D0: velocity-dependent Sustain/Palm Mute. G0 + A0: strings 3–4 Slap, strings 1–2 Pop.
- Mod wheel controls vibrato; noise at value ≥ 0.75.
- Manual lists Hold Pedal Toggle but does not give a MIDI note in the main-panel text.

## 6. Verification status

- Pitch names/ranges/functions: transcribed from the official current Ample Sound Main Panel Manual.
- Raw MIDI Note No.: derived from the AI Tracker convention C4=60.
- Where the official manual does not provide a concrete MIDI note/value, no value has been invented.
