# Ample Bass P — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://amplesound.net/en/Main_Panel_Manual-ABP.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Accent | C0 | 12 | bass range | vel 1–125 Sustain; 126–127 Accentuation |
| NH | Natural Harmonic | C#0 | 13 | bass range |  |
| PM | Palm Mute | D0 | 14 | bass range | high velocity Palm Mute; low velocity Dead Note |
| SIO | Slide In & Slide Out | D#0 | 15 | bass range |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | bass range |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | bass range |  |
| Rep | Repeat Note | F#0 | 18 | bass range |  |
| Slap | Slap / Dead Slap | G0 | 19 | bass range | vel 64–127 Slap; 1–63 Dead Slap |
| Tap | Tap | G#0 | 20 | bass range |  |
| Pop | Pop / Dead Pop | A0 | 21 | bass range | vel 64–127 Pop; 1–63 Dead Pop |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---:|---|---|
| B6 | 95 | Position Mode | low velocity = Melody Mode; high velocity = Chord Mode |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| E6–G6 | — | String Assignment | 4th → 1st string |
| A#0 | 22 | Position Assignment trigger | follow with a position note |

## 3. FX / performance trigger notes

| Note | Raw MIDI Note No. | Function |
|---|---:|---|
| F5 | 77 | Scratch 1 |
| F#5 | 78 | Scratch 2 / Raking |
| G5 | 79 | Single String Slap |
| G#5 | 80 | Left-Hand Slap Noise |
| A5 | 81 | Right-Hand Slap Noise |
| A#5 | 82 | FX Slide Turn |
| B5 | 83 | FX Slide Turn |
| C6 | 84 | FX Slide Down / Turn |
| C#6 | 85 | FX Slide Down |

## 4. Operational notes

- C0 + D0 = Sustain/Palm-Mute mixed mode.
- G0 + A0 = Slap/Pop split by strings on applicable 4-string models.
- Mod wheel controls vibrato; current bass manuals document vibrato noise at approximately value ≥ 0.75.

## 5. Verification status

- Core keyswitch names and standard v4 behavior are based on the official current Ample Sound manuals.
- Raw MIDI Note No. values are derived from C4=60.
- Product-specific values not explicitly verified are left generic rather than guessed.
