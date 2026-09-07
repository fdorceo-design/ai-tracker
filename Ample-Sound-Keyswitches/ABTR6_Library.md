# Ample Bass TR6 — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://amplesound.net/en/Main_Panel_Manual-ABTR6.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Arp | Arpeggio | A-1 | 9 | A0–G#4 | subtypes: Sus2 / 7 / M7 / m7 by played register |
| Mor | Mordent | A#-1 | 10 | B0–C5 | major-second mordent; temporary legato behavior |
| SF | Slide Turn | B-1 | 11 | C1–C#5 | major-second slide turn |
| Sus | Sustain & Accent | C0 | 12 | B0–D5 | vel 1–125 Sustain; 126–127 Accentuation |
| NH | Natural Harmonic | C#0 | 13 | B0–G4 |  |
| PM | Palm Mute | D0 | 14 | B0–D5 | high velocity Palm Mute; low velocity Dead Note |
| SIO | Slide In & Slide Out | D#0 | 15 | C#1–D5 |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | A#0–D5 |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | B0–D5 |  |
| Pop | Pop / Dead Pop | F#0 | 18 | B0–D5 | vel 64–127 Pop; 1–63 Dead Pop |
| Slap | Slap / Dead Slap | G0 | 19 | B0–D5 | vel 64–127 Slap; 1–63 Dead Slap |
| Tap | Tap | G#0 | 20 | B0–D5 |  |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---:|---|---|
| B6 | 95 | Position Mode | low velocity Melody; high velocity Chord |
| D#6 | 87 | Auto Legato Mode | high velocity Auto Slide; low velocity Auto HP |
| C-1–F-1 | — | String Assignment | 6th → 1st string |
| F#-1 | 6 | Position Assignment trigger | then B0–G2 selects position |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| F5 | 77 | Scratch |
| F#5 | 78 | Raking |
| G5 | 79 | Single String Slap |
| G#5 | 80 | Left-Hand Slap Noise |
| A5 | 81 | Right-Hand Slap Noise |
| A#5 | 82 | FX Slide Turn 6 |
| B5 | 83 | FX Slide Turn 2 |
| C6 | 84 | FX Slide Turn 4 |
| C#6 | 85 | FX Slide Down 6 |
| D6 | 86 | FX Slide Down 5 |
| D#6 | 87 | FX Slide Down 4 |

## 4. Operational notes

- F#0 + G0: strings 4–6 = Slap, strings 1–3 = Pop.
- D#5 repeats currently sounding notes; polyphonic notes are supported.
- Mod wheel controls vibrato; vibrato noise is documented at value ≥ 0.75.

## 5. Verification status

- Core keyswitch names and standard v4 behavior are based on the official current Ample Sound manuals.
- Raw MIDI Note No. values are derived from C4=60.
- Product-specific values not explicitly verified are left generic rather than guessed.
