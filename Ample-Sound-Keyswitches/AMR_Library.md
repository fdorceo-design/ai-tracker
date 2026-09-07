# Ample Metal Ray5 — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://amplesound.net/en/Main_Panel_Manual-AMR.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Accent | C0 | 12 | 5-string bass range | velocity-dependent Sustain/Accent |
| NH | Natural Harmonic | C#0 | 13 | 5-string bass range |  |
| PM | Palm Mute | D0 | 14 | 5-string bass range | velocity controls mute/dead-note behavior |
| SIO | Slide In & Slide Out | D#0 | 15 | 5-string bass range |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | 5-string bass range |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | 5-string bass range |  |
| Slap | Slap / Dead Slap | G0 | 19 | 5-string bass range | velocity-dependent |
| Pop | Pop / Dead Pop | A0 | 21 | 5-string bass range | velocity-dependent |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---:|---|---|
| B6 | 95 | Position Mode | low velocity Melody; high velocity Chord |
| D#6 | 87 | Auto Legato Mode | high velocity Auto Slide; low velocity Auto HP |
| String Assignment | — | String-force keys | five-string mapping; see official AMR manual for exact force-note range |

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

- Metal Ray5 is a five-string bass; string-force mapping therefore differs from four-string ABP/ABY.

## 5. Verification status

- Core keyswitch names and standard v4 behavior are based on the official current Ample Sound manuals.
- Raw MIDI Note No. values are derived from C4=60.
- Product-specific values not explicitly verified are left generic rather than guessed.
