# Ample Bass Acoustic (ABA) — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://www.amplesound.net/en/Main_Panel_Manual-ABA.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Pop | C0 | 12 | B0–F4 | Vel 1–125 Sustain; 126–127 Pop |
| NH | Natural Harmonic | C#0 | 13 | E1–G3 |  |
| PM | Palm Mute | D0 | 14 | E1–F4 | High velocity Palm Mute; low velocity Dead Note |
| SIO | Slide In & Slide Out | D#0 | 15 | F#1–F4 | KS before note = Slide In; KS while sounding = Slide Out; auto-return to Sustain |
| LS | Legato Slide (Poly Legato) | E0 | 16 | F1–F4 | Overlapping notes; destination velocity controls slide speed; high-velocity E0 allows position change |
| HP | Hammer-On & Pull-Off (Poly Legato) | F0 | 17 | E1–F4 | Overlapping notes; high-velocity F0 allows position change |
| Slap | Slap / Dead Slap / Pop / Dead Pop | G0 | 19 | E1–F4 | Strings 3–4: high=Slap low=Dead Slap; strings 1–2: high=Pop low=Dead Pop |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---|---|---|
| B6 | 95 | Position Mode | low velocity = Melody Mode; high velocity = Chord Mode |
| D6 | 86 | Buzzing | buzz all currently ringing notes |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| E6–G6 | — | String Assignment | 4th→1st string; high velocity affects fret position, low velocity does not |
| A#0 | 22 | Position Assignment trigger | then E1–A#2 selects position |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| G#4 | 68 | Hit Top (Mute) |
| A4 | 69 | Hit Top (Open) |
| A#4 | 70 | Hit Rim |
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
| D#5 | 75 | Note Repeater |

## 5. Operational notes

- C0 + D0 together: mixed Sustain/Palm Mute mode (high velocity Sustain, low velocity Palm Mute).
- Mod wheel controls vibrato; vibrato noise is triggered when the control value reaches approximately 0.75 or above.
- Manual section 'Hold Pedal Toggle' is listed, but this main-panel manual does not state a MIDI note number for it.

## 6. Verification status

- Pitch names/ranges/functions: transcribed from the official current Ample Sound Main Panel Manual.
- Raw MIDI Note No.: derived from the AI Tracker convention C4=60.
- Where the official manual does not provide a concrete MIDI note/value, no value has been invented.
