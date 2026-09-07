# Ample Ethno Ukulele (AEU) — AI Tracker Reference

Updated: 2026-09-08

Evidence priority: official Ample Sound current Main Panel Manual. Raw MIDI Note No. uses **C4 = 60**.

Official manual: https://www.amplesound.net/en/Main_Panel_Manual-AEU.pdf

## 1. Main articulations / keyswitches

| Abbr. | Articulation | Keyswitch | Raw MIDI Note No. | Playable range | Notes |
|---|---|---:|---:|---|---|
| Sus | Sustain & Pop | C0 | 12 | C3(G2)–D#5 | Velocity <127 Sustain; velocity 127 Pop |
| NH | Natural Harmonic | C#0 | 13 | G2–D#5 |  |
| PM | Palm Mute | D0 | 14 | C3(G2)–D#5 | Lower velocity = deeper mute |
| SIO | Slide In & Slide Out | D#0 | 15 | D3(A2)–D#5 |  |
| LS | Legato Slide (Poly Legato) | E0 | 16 | C#3(G#2)–D#5 |  |
| HP | Hammer-On & Pull-Off | F0 | 17 | C3(G2)–D#5 |  |
| Str | Strum | F#0 | 18 | C3(G2)–D#5 |  |

## 2. Control / mode keys

| Note / range | Raw MIDI Note No. | Function | Behavior |
|---|---|---|---|
| B6 | 95 | Position Mode | low velocity = Melody; high velocity = Chord |
| G#6 | 92 | Open String First | high velocity ON; low velocity OFF |
| D#6 | 87 | Auto Legato Mode | high velocity = Automatic Slide; low velocity = Automatic Hammer-On/Pull-Off |
| A0–C1 | — | String Assignment | 4th→1st string |
| C#1 | 25 | Position Assignment trigger | then G1–A2 selects position |

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
| E6 | 88 | Note Repeater |

## 5. Operational notes

- C0 + D0: mixed Sustain/Palm Mute. C0 + F#0: strings 3–4 Strum, strings 1–2 Sustain.
- Open String First affects C3, E3, G3, A3.
- Manual lists Hold Pedal Toggle but does not give a MIDI note in the main-panel text.

## 6. Verification status

- Pitch names/ranges/functions: transcribed from the official current Ample Sound Main Panel Manual.
- Raw MIDI Note No.: derived from the AI Tracker convention C4=60.
- Where the official manual does not provide a concrete MIDI note/value, no value has been invented.
