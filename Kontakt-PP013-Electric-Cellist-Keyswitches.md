# Peter Gregson Electric Cellist PP013 — Keyswitch Reference

Patch: "Various articulations.nki"

As supplied by the user (not yet empirically verified against the actual
running instance the way [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md)
was — that one went through three wrong octave-convention guesses before
being confirmed by direct test). **Before relying on these for playback,
send a candidate note and read the articulation name Kontakt actually
displays**, per the guidance in [CLAUDE.md](CLAUDE.md).

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
| 13 | Tremolo | CC1 | Do not assume; verify |
| 14 | Long Harmonic | CC1 | **No — confirmed** |
| 15 | Long Col Legno | CC1 | **No — confirmed** |
| 16 | Long (Artisan) | Phrase-style material; CC21 not used |
| 17 | Spiccato | Velocity | No practical use expected |
| 18 | Pizzicato | Velocity | No |
| 19 | Pizzicato (Bell) | Velocity | No |
| 20 | Col Legno | Velocity | No practical use expected |
| 21 | FX - Slide Up | Recorded FX / velocity as applicable | No assumption |
| 22 | FX - Slide Down | Recorded FX / velocity as applicable | No assumption |
| 23 | Aleatoric - Detuned | Recorded FX / velocity as applicable | No assumption |
| 24 | Aleatoric - Finger | Recorded FX / velocity as applicable | No assumption |
| 25 | Aleatoric - Tree Fall | Recorded FX / velocity as applicable | No assumption |

For AI generation: never add CC21 merely because a technique is a long sample.
If a CC21 response has not been verified for that specific articulation, leave
CC21 alone rather than inventing vibrato automation.

### Confirmed CC21 results

- `Long` — CC21 works.
- `Long Harmonic` — CC21 does not work.
- `Long Col Legno` — CC21 does not work.
- `Long (Artisan)` — treated as phrase-style material rather than a normal
  sustained articulation; do not generate CC21 vibrato automation for it.
