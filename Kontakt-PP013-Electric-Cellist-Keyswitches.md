# Peter Gregson Electric Cellist PP013 — Keyswitch Reference

Patch: "Various articulations.nki"

The keyswitch list and playable ranges in this reference have been checked against the user's actual running Kontakt `Various articulations.nki` instance. Raw MIDI Note No. remains the authoritative machine-facing value; do not derive keyswitches from octave-name conventions.

| MIDI | Articulation |
|---|---|
| 12 | Long |
| 13 | Tremolo |
| 14 | Long Harmonic |
| 15 | Long Col Legno |
| 16 | Long (Artisan) |
| 17 | Spiccato |
| 18 | Pizzicato |
| 19 | Pizzicato (Bell) |
| 20 | Col Legno |
| 21 | FX - Slide Up |
| 22 | FX - Slide Down |
| 23 | Aleatoric - Detuned |
| 24 | Aleatoric - Finger |
| 25 | Aleatoric - Tree Fall |

## Playable pitch ranges

The ranges below are taken directly from the user's unchanged `Various articulations.nki`
Kontakt display.  Blue keys are the playable range.  Kontakt note labels are retained,
and raw MIDI Note No. is given using this project's established convention
(`MIDI Note No. 0 = C-2`, therefore Kontakt C1 = MIDI 36).

For articulations not individually screenshotted, the user confirmed that their playable
range is the same as `Long`.

| MIDI KS | Articulation | Playable range | Raw MIDI Note No. |
|---:|---|---|---:|
| 12 | Long | C1-B5 | 36-95 |
| 13 | Tremolo | same as Long | 36-95 |
| 14 | Long Harmonic | C4-B7 | 72-119 |
| 15 | Long Col Legno | C1-B4 | 36-83 |
| 16 | Long (Artisan) | C1-B5 | 36-95 |
| 17 | Spiccato | same as Long | 36-95 |
| 18 | Pizzicato | same as Long | 36-95 |
| 19 | Pizzicato (Bell) | same as Long | 36-95 |
| 20 | Col Legno | same as Long | 36-95 |
| 21 | FX - Slide Up | C1-B5 | 36-95 |
| 22 | FX - Slide Down | C1-B5 | 36-95 |
| 23 | Aleatoric - Detuned | C2-B4 | 48-83 |
| 24 | Aleatoric - Finger | C2-B3 | 48-71 |
| 25 | Aleatoric - Tree Fall | C2-B3 | 48-71 |

These are **patch trigger/playable ranges**, not a generic acoustic-cello range.

## Performance controls for AI use

Spitfire's normal orchestral-controller convention is:

- **CC1 (Dynamics)** — use for sustained/long articulations. It crossfades sampled
  dynamic layers; note velocity is not the primary loudness control for these.
- **Velocity** — use for short articulations by default.
- **CC11 (Expression)** — overall sample-volume trim; useful on both long and
  short techniques, but it does not replace CC1/velocity for timbral dynamics.
- **CC21 (Vibrato)** — only has a musical effect when the currently selected
  articulation actually contains multiple vibrato states / a vibrato mapping.
  The presence of the Vibrato fader in the Kontakt UI does **not** prove that
  every articulation responds to it.

AI-safe control map for `Various articulations.nki`:

| MIDI | Articulation | Primary dynamics | CC21 Vibrato |
|---:|---|---|---|
| 12 | Long | CC1 | **Yes — confirmed** |
| 13 | Tremolo | CC1 | N/A — tremolo is the articulation |
| 14 | Long Harmonic | CC1 | **No — confirmed** |
| 15 | Long Col Legno | CC1 | **No — confirmed** |
| 16 | Long (Artisan) | Phrase-style material; CC21 not used |
| 17 | Spiccato | Velocity | No practical use expected |
| 18 | Pizzicato | Velocity | No |
| 19 | Pizzicato (Bell) | Velocity | No |
| 20 | Col Legno | Velocity | No practical use expected |
| 21 | FX - Slide Up | Recorded FX / velocity as applicable | N/A |
| 22 | FX - Slide Down | Recorded FX / velocity as applicable | N/A |
| 23 | Aleatoric - Detuned | Recorded FX / velocity as applicable | N/A |
| 24 | Aleatoric - Finger | Recorded FX / velocity as applicable | N/A |
| 25 | Aleatoric - Tree Fall | Recorded FX / velocity as applicable | N/A |

For AI generation: never add CC21 merely because a technique is a long sample.
If a CC21 response has not been verified for that specific articulation, leave
CC21 alone rather than inventing vibrato automation.

### Confirmed CC21 results

- `Long` — CC21 works.
- `Long Harmonic` — CC21 does not work.
- `Long Col Legno` — CC21 does not work.
- `Long (Artisan)` — treated as phrase-style material rather than a normal
  sustained articulation; do not generate CC21 vibrato automation for it.

## Round-robin reset controls

`Reset from` and `Reset on transport` in the Kontakt UI are **round-robin reset controls**. They are not articulation reset commands.
