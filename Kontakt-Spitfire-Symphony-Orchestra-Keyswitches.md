# Spitfire Symphony Orchestra (Kontakt) - Keyswitch & MIDI Control Reference

AI Tracker / local-AI operation reference.

## Scope and confidence

- This file deliberately does **not** use UACC.
- Scope is limited to the **standard factory All Techniques patches** needed by AI Tracker.
- Excluded from this reference: **Individual/single-articulation patches, Performance patches, Core Techniques patches, Decorative Techniques patches, Legacy patches, Curated patches, Motif patches, reduced/lightweight variants, TM variants, and similar alternate patch families**.
- MIDI pitch is written as raw **MIDI Note No.**. In this project/Kontakt convention, MIDI Note No. 0 = C-2.
- Spitfire documents the normal factory keyswitch scheme as beginning at C-2 and ascending chromatically.
- The technique names and ordering below are taken from Spitfire's current SSO technique lists for the standard All Techniques patches.
- Therefore the Note No. tables below are a **factory-map derivation**: C-2 / Note 0 + official technique order.
- They are not claimed to be per-row empirical screenshots from the user's Kontakt instance.
- The current SSO manual independently demonstrates `Trumpet Solo - All techniques` using C-2 through D#-1; that agrees with the standard 16-technique trumpet table below.
- User-custom keyswitch relocation or older saved NKIs can change the actual layout.


## Playable pitch ranges

These are the **published sounding/playable ranges** from the SSO manual, using the
manual's convention `middle C = C4`.  Raw MIDI Note No. is converted to the AI Tracker
/Kontakt convention used in this project (`MIDI Note No. 0 = C-2`).

| Instrument | Lowest | MIDI Note No. | Highest | MIDI Note No. |
|---|---:|---:|---:|---:|
| Violins 1 | G3 | 67 | C#7 | 109 |
| Violins 2 | G3 | 67 | C#7 | 109 |
| Violas | C3 | 60 | F#6 | 102 |
| Celli | C2 | 48 | Bb5 | 94 |
| Basses | C1 | 36 | F#3 | 66 |
| Piccolo Flute | D5 | 86 | G#7 | 116 |
| Flute C (Solo / a2) | C4 | 72 | C7 | 108 |
| Alto Flute | G3 | 67 | G6 | 103 |
| Bass Flute | C3 | 60 | C6 | 96 |
| Oboe (Solo / a2) | Bb3 | 70 | G6 | 103 |
| Cor Anglais | E3 | 64 | Bb5 | 94 |
| Clarinet Bb (Solo / a2) | D3 | 62 | F6 | 101 |
| Bass Clarinet Bb | Bb1 | 46 | F5 | 89 |
| Contrabass Clarinet Bb | Bb0 | 34 | C4 | 72 |
| Bassoon (Solo / a2) | Bb1 | 46 | D5 | 86 |
| Contrabassoon | Bb0 | 34 | Bb3 | 70 |
| Trumpet Bb (Solo / a2 / a6) | Db3 | 61 | D6 | 98 |
| Horn F (Solo / a2 / a6) | D2 | 50 | F5 | 89 |
| Tenor Trombone (Solo / a2 / a6) | F1 | 41 | C5 | 84 |
| Bass Trombone | Eb1 | 39 | Ab4 | 80 |
| Contrabass Trombone | C1 | 36 | F#3 | 66 |
| Cimbasso | C1 | 36 | C4 | 72 |
| Tuba F | C1 | 36 | Eb4 | 75 |
| Contrabass Tuba | C1 | 36 | Ab3 | 68 |

Notes:

- Spitfire explicitly warns that brass ranges can differ somewhat between solo and
  section versions.  The table above records the family range published in the manual
  and should be treated as a safe reference, not a promise that every articulation has
  samples on every endpoint.
- Individual articulations can have narrower playable ranges than the instrument's
  overall range (harmonics and special FX are obvious examples).
- SSO Solo Strings added in 2025 are a separate SSO component.  The current manual
  lists their All Techniques content, but the source checked for this revision does not
  publish a separate sampled low/high range table for those solo patches.  Do not
  silently substitute the section ranges as if they were directly documented.
- Percussion note ranges are trigger maps rather than acoustic pitch ranges and are
  handled separately.

## MIDI controls used by AI Tracker

### Strings / Brass / Woodwinds

| Material | Primary dynamic control | Other control |
|---|---|---|
| Sustained / Long-family | CC1 Dynamics | CC11 Expression |
| Ordinary Short articulations | Note Velocity | CC11 Expression |
| Vibrato-capable sustained articulations | CC21 only when the loaded patch/articulation actually exposes and responds to vibrato | CC1 / CC11 as above |

Do **not** assume that every Long responds to CC21.  CC21 is articulation-dependent.

### Percussion (Kickstart)

SSO percussion uses a different model and must not inherit the strings/brass/woodwind control rules.

| Material | Primary dynamic control | Notes |
|---|---|---|
| Single hits / short hits | Note Velocity | Factory Kickstart patches are velocity-oriented for hit strength. |
| Rolls / sustained percussion | CC1 Dynamics where the technique provides recorded dynamic layers | Treat like sustained material, not like a single hit. |
| Expression | CC11 | Overall expressive trim. |
| Vibrato | **Not used** | Do not generate CC21 for normal percussion. |

The SSO manual also exposes `CC BASED VELOCITY`, which can deliberately make the
Dynamics slider/mod-wheel control note velocity.  AI Tracker should **not assume this
option is enabled**; the normal/default hit rule remains Velocity.

`ROLL ON HIGH VEL.` is another Kickstart option.  It can make a high-velocity hit
trigger a roll where available.  AI Tracker should not rely on this optional behaviour
unless the patch has explicitly been configured for it.

## Factory All-Techniques maps

Only the **standard/current factory All Techniques patches** are in scope here.
If a patch name or folder is marked `Individual`, `Performance`, `Core Techniques`,
`Decorative Techniques`, `Legacy`, `Curated`, `Motif`, `TM`, reduced/lightweight,
or another alternate variant, ignore it for this file.


## Strings

### Basses

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | FX |
| 1 | C#-2 | Long Flautando |
| 2 | D-2 | Long Harmonics |
| 3 | D#-2 | Long Sul Pont Distorted |
| 4 | E-2 | Long Sul Pont |
| 5 | F-2 | Long Super Sul Tasto |
| 6 | F#-2 | Long |
| 7 | G-2 | Marcato Attack |
| 8 | G#-2 | Short 0'5 |
| 9 | A-2 | Short 1'0 |
| 10 | A#-2 | Short Col Legno |
| 11 | B-2 | Harmonics |
| 12 | C-1 | Short Pizzicato Bartok |
| 13 | C#-1 | Short Pizzicato |
| 14 | D-1 | Short Spicc-Pizz |
| 15 | D#-1 | Short Spiccato |
| 16 | E-1 | Short Staccato Dig |
| 17 | F-1 | Tremolo Measured (150bpm) |
| 18 | F#-1 | Tremolo Measured (180bpm) |
| 19 | G-1 | Tremolo Sul Pont |
| 20 | G#-1 | Tremolo |
| 21 | A-1 | Trill (Major 2nd) |
| 22 | A#-1 | Trill (Minor 2nd) |

### Celli

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | FX |
| 1 | C#-2 | Legato Sul C |
| 2 | D-2 | Long CS Blend |
| 3 | D#-2 | Long CS Sul Pont |
| 4 | E-2 | Long CS |
| 5 | F-2 | Long Flautando |
| 6 | F#-2 | Long Harmonics |
| 7 | G-2 | Long Rachmaninoff Molto Vib |
| 8 | G#-2 | Long Sul C |
| 9 | A-2 | Long Sul Pont |
| 10 | A#-2 | Long Super Sul Tasto |
| 11 | B-2 | Long |
| 12 | C-1 | Marcato Attack |
| 13 | C#-1 | Short 0'5 |
| 14 | D-1 | Short 1'0 |
| 15 | D#-1 | Short Brushed CS |
| 16 | E-1 | Short Brushed |
| 17 | F-1 | Short Col Legno |
| 18 | F#-1 | Short CS |
| 19 | G-1 | Short Harmonics |
| 20 | G#-1 | Short Pizzicato Bartok |
| 21 | A-1 | Short Pizzicato |
| 22 | A#-1 | Short Spiccato |
| 23 | B-1 | Tremolo CS |
| 24 | C0 | Tremolo Measured (150bpm) |
| 25 | C#0 | Tremolo Measured (180bpm) |
| 26 | D0 | Tremolo Measured CS (150bpm) |
| 27 | D#0 | Tremolo Sul Pont |
| 28 | E0 | Tremolo |
| 29 | F0 | Trill (Major 2nd) |
| 30 | F#0 | Trill (Major 3rd) |
| 31 | G0 | Trill (Minor 2nd) |
| 32 | G#0 | Trill (Minor 3rd) |

### Ensembles

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long CS Blend |
| 1 | C#-2 | Long CS |
| 2 | D-2 | Long Flautando |
| 3 | D#-2 | Long Harmonics |
| 4 | E-2 | Long Sul Pont |
| 5 | F-2 | Long Sul String |
| 6 | F#-2 | Long Super Sul Tasto |
| 7 | G-2 | Long |
| 8 | G#-2 | Marcato Attack |
| 9 | A-2 | Short 0'5 |
| 10 | A#-2 | Short Brushed CS |
| 11 | B-2 | Short Brushed |
| 12 | C-1 | Short Col Legno |
| 13 | C#-1 | Short Harmonics |
| 14 | D-1 | Short Pizzicato Bartok |
| 15 | D#-1 | Short Pizzicato |
| 16 | E-1 | Short Spiccato CS |
| 17 | F-1 | Short Spiccato |
| 18 | F#-1 | Tremolo CS |
| 19 | G-1 | Tremolo SulPont |
| 20 | G#-1 | Tremolo |
| 21 | A-1 | Trill (Major 2nd) |
| 22 | A#-1 | Trill (Minor 2nd) |

### Violas

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | FX |
| 1 | C#-2 | Legato Sul C |
| 2 | D-2 | Long CS Blend |
| 3 | D#-2 | Long CS Sul Pont |
| 4 | E-2 | Long CS |
| 5 | F-2 | Long Flautando |
| 6 | F#-2 | Long Harmonics |
| 7 | G-2 | Long Rachmaninoff Molto Vib |
| 8 | G#-2 | Long Sul C |
| 9 | A-2 | Long Sul Pont |
| 10 | A#-2 | Long Super Sul Tasto |
| 11 | B-2 | Long |
| 12 | C-1 | Marcato Attack |
| 13 | C#-1 | Short 0'5 |
| 14 | D-1 | Short 1'0 |
| 15 | D#-1 | Short Brushed CS |
| 16 | E-1 | Short Brushed |
| 17 | F-1 | Short Col Legno |
| 18 | F#-1 | Short CS |
| 19 | G-1 | Short Harmonics |
| 20 | G#-1 | Short Pizzicato Bartok |
| 21 | A-1 | Short Pizzicato |
| 22 | A#-1 | Short Spiccato |
| 23 | B-1 | Tremolo CS |
| 24 | C0 | Tremolo Measured (150bpm) |
| 25 | C#0 | Tremolo Measured (180bpm) |
| 26 | D0 | Tremolo Sul Pont |
| 27 | D#0 | Tremolo |
| 28 | E0 | Trill (Major 2nd) |
| 29 | F0 | Trill (Minor 2nd) |

### Violins 1

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | FX |
| 1 | C#-2 | Legato Sul G |
| 2 | D-2 | Long CS Blend |
| 3 | D#-2 | Long CS Sul Pont |
| 4 | E-2 | Long CS |
| 5 | F-2 | Long Flautando |
| 6 | F#-2 | Long Harmonics |
| 7 | G-2 | Long Rachmaninoff Molto Vib |
| 8 | G#-2 | Long Sul G |
| 9 | A-2 | Long Sul Pont Distorted |
| 10 | A#-2 | Long Sul Pont |
| 11 | B-2 | Long Sul Tasto |
| 12 | C-1 | Long Super Sul Tasto |
| 13 | C#-1 | Long |
| 14 | D-1 | Marcato Attack |
| 15 | D#-1 | Short 0'5 |
| 16 | E-1 | Short 1'0 |
| 17 | F-1 | Short Brushed CS |
| 18 | F#-1 | Short Brushed |
| 19 | G-1 | Short Col Legno |
| 20 | G#-1 | Short CS |
| 21 | A-1 | Short Harmonics |
| 22 | A#-1 | Short Pizzicato Bartok |
| 23 | B-1 | Short Pizzicato |
| 24 | C0 | Short Spiccato |
| 25 | C#0 | Tremolo Measured (150bpm) |
| 26 | D0 | Tremolo Measured (180bpm) |
| 27 | D#0 | Tremolo Measured CS (150bpm) |
| 28 | E0 | Tremolo Sul Pont |
| 29 | F0 | Tremolo |
| 30 | F#0 | Trill (Major 2nd) |
| 31 | G0 | Trill (Major 3rd) |
| 32 | G#0 | Trill (Minor 2nd) |
| 33 | A0 | Trill (Minor 3rd) |

### Violins 2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | FX |
| 1 | C#-2 | Legato Sul G |
| 2 | D-2 | Long CS Blend |
| 3 | D#-2 | Long CS Sul Pont |
| 4 | E-2 | Long CS |
| 5 | F-2 | Long Flautando |
| 6 | F#-2 | Long Harmonics |
| 7 | G-2 | Long Rachmaninoff Molto Vib |
| 8 | G#-2 | Long Sul G |
| 9 | A-2 | Long Sul Pont |
| 10 | A#-2 | Long Super Sul Tasto |
| 11 | B-2 | Long |
| 12 | C-1 | Marcato Attack |
| 13 | C#-1 | Short 0'5 |
| 14 | D-1 | Short 1'0 |
| 15 | D#-1 | Short Brushed CS |
| 16 | E-1 | Short Brushed |
| 17 | F-1 | Short Col Legno |
| 18 | F#-1 | Short CS |
| 19 | G-1 | Short Harmonics |
| 20 | G#-1 | Short Pizzicato Bartok |
| 21 | A-1 | Short Pizzicato |
| 22 | A#-1 | Short Spiccato |
| 23 | B-1 | Tremolo CS |
| 24 | C0 | Tremolo Measured (150bpm) |
| 25 | C#0 | Tremolo Measured (180bpm) |
| 26 | D0 | Tremolo Sul Pont |
| 27 | D#0 | Tremolo |
| 28 | E0 | Trill (Major 2nd) |
| 29 | F0 | Trill (Minor 2nd) |

## Brass

### Bass Trombone Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Cuivre |
| 2 | D-2 | Long Flutter |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Bass Trombones a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | FX Gliss |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Cimbassi a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Alt |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Rip |
| 5 | F-2 | Short Marcato |
| 6 | F#-2 | Short Staccato |
| 7 | G-2 | Short Tenuto |

### Cimbasso Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Cuivre |
| 2 | D-2 | Long |
| 3 | D#-2 | Rip |
| 4 | E-2 | Short Marcato |
| 5 | F-2 | Short Staccato |
| 6 | F#-2 | Short Tenuto |

### Contrabass Trombone

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Cuivre |
| 2 | D-2 | Long Stopped |
| 3 | D#-2 | Long |
| 4 | E-2 | Rip |
| 5 | F-2 | Short Marcato |
| 6 | F#-2 | Short Staccato Muted |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Contrabass Tuba Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long |
| 2 | D-2 | Rip |
| 3 | D#-2 | Short Marcato |
| 4 | E-2 | Short Staccato |
| 5 | F-2 | Short Tenuto |

### Horn Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Legato |
| 2 | D-2 | Long Flutter |
| 3 | D#-2 | Long |
| 4 | E-2 | Rip |
| 5 | F-2 | Short Marcato |
| 6 | F#-2 | Short Staccatissimo |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |
| 9 | A-2 | Trill (Major 2nd) |
| 10 | A#-2 | Trill (Minor 2nd) |

### Horns a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Bells up Crotchet |
| 1 | C#-2 | Bells up Long |
| 2 | D-2 | Bells up Quaver |
| 3 | D#-2 | Bells up Staccatissimo |
| 4 | E-2 | Bells up Staccato |
| 5 | F-2 | Fall |
| 6 | F#-2 | Legato |
| 7 | G-2 | Long Cuivre |
| 8 | G#-2 | Long Flutter |
| 9 | A-2 | Long Stopped |
| 10 | A#-2 | Long |
| 11 | B-2 | Multitongue |
| 12 | C-1 | Rip |
| 13 | C#-1 | Short Marcato |
| 14 | D-1 | Short Staccatissimo |
| 15 | D#-1 | Short Staccato |
| 16 | E-1 | Short Tenuto |
| 17 | F-1 | Trill (Major 2nd) |
| 18 | F#-1 | Trill (Minor 2nd) |

### Horns a6

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Fanfare |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Tenor Trombone Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Cuivre |
| 2 | D-2 | Long Flutter Muted |
| 3 | D#-2 | Long Flutter |
| 4 | E-2 | Long Muted |
| 5 | F-2 | Long |
| 6 | F#-2 | Multitongue |
| 7 | G-2 | Rip |
| 8 | G#-2 | Short Marcato Muted |
| 9 | A-2 | Short Marcato |
| 10 | A#-2 | Short Staccato Muted |
| 11 | B-2 | Short Staccato |
| 12 | C-1 | Short Tenuto Muted |
| 13 | C#-1 | Short Tenuto |

### Tenor Trombones a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | FX Gliss |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Trombones a6

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Fanfare |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Trumpet Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall Muted |
| 1 | C#-2 | FX Gliss |
| 2 | D-2 | Long Flutter |
| 3 | D#-2 | Long Mariachi |
| 4 | E-2 | Long Muted |
| 5 | F-2 | Long |
| 6 | F#-2 | Multitongue |
| 7 | G-2 | Rip Muted |
| 8 | G#-2 | Short Marcato Muted |
| 9 | A-2 | Short Marcato |
| 10 | A#-2 | Short Staccato Muted |
| 11 | B-2 | Short Staccato |
| 12 | C-1 | Short Tenuto Muted |
| 13 | C#-1 | Short Tenuto |
| 14 | D-1 | Trill (Major 2nd) |
| 15 | D#-1 | Trill (Minor 2nd) |

### Trumpets a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Fanfare |
| 2 | D-2 | Long Mariachi |
| 3 | D#-2 | Long Muted |
| 4 | E-2 | Long |
| 5 | F-2 | Multitongue |
| 6 | F#-2 | Rip |
| 7 | G-2 | Short Marcato |
| 8 | G#-2 | Short Staccato |
| 9 | A-2 | Short Tenuto |
| 10 | A#-2 | Trill (Major 2nd) |
| 11 | B-2 | Trill (Minor 2nd) |

### Trumpets a6

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Fanfare |
| 2 | D-2 | Long Cuivre |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Rip |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |

### Tuba Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Fall |
| 1 | C#-2 | Long Stopped |
| 2 | D-2 | Long |
| 3 | D#-2 | Rip |
| 4 | E-2 | Short Marcato |
| 5 | F-2 | Short Staccato |
| 6 | F#-2 | Short Stopped |
| 7 | G-2 | Short Tenuto |

## Woodwinds

### Alto Flute

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Harmonics |
| 2 | D-2 | Long Overblown |
| 3 | D#-2 | Long |
| 4 | E-2 | Short Marcato |
| 5 | F-2 | Short Overblown |
| 6 | F#-2 | Short Staccato |
| 7 | G-2 | Short Tenuto |
| 8 | G#-2 | Trill (Major 2nd) |
| 9 | A-2 | Trill (Minor 2nd) |

### Bass Clarinet

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Short Staccato |
| 2 | D-2 | Short Tenuto |
| 3 | D#-2 | Trill (Major 2nd) |
| 4 | E-2 | Trill (Minor 2nd) |

### Bass Flute

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Overblown |
| 2 | D-2 | Long |
| 3 | D#-2 | Short Marcato |
| 4 | E-2 | Short Staccato |
| 5 | F-2 | Short Tenuto |

### Bassoon Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Harmonics |
| 2 | D-2 | Long |
| 3 | D#-2 | Multitongue |
| 4 | E-2 | Short Marcato |
| 5 | F-2 | Short Staccato |
| 6 | F#-2 | Short Tenuto |
| 7 | G-2 | Trill (Major 2nd) |
| 8 | G#-2 | Trill (Minor 2nd) |

### Bassoons a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long |
| 2 | D-2 | Multitongue |
| 3 | D#-2 | Short Marcato |
| 4 | E-2 | Short Staccato |
| 5 | F-2 | Short Tenuto |
| 6 | F#-2 | Trill (Major 2nd) |
| 7 | G-2 | Trill (Minor 2nd) |

### Clarinet Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long |
| 2 | D-2 | Multitongue |
| 3 | D#-2 | Short Staccato |
| 4 | E-2 | Trill (Major 2nd) |
| 5 | F-2 | Trill (Minor 2nd) |

### Clarinets a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Multitongue |
| 2 | D-2 | Short Marcato |
| 3 | D#-2 | Short Staccato |
| 4 | E-2 | Short Tenuto |
| 5 | F-2 | Trill (Major 2nd) |
| 6 | F#-2 | Trill (Minor 2nd) |

### Contrabass Clarinet

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Short Marcato |
| 2 | D-2 | Short Staccato |
| 3 | D#-2 | Short Tenuto |

### Contrabassoon

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Short Marcato |
| 2 | D-2 | Short Staccato |
| 3 | D#-2 | Short Tenuto |

### Cor Anglais

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Short Marcato |
| 2 | D-2 | Short Staccato |
| 3 | D#-2 | Short Tenuto |

### Flute Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Harmonics |
| 2 | D-2 | Long Hollow |
| 3 | D#-2 | Long |
| 4 | E-2 | Multitongue |
| 5 | F-2 | Short Marcato Sfz |
| 6 | F#-2 | Short Marcato |
| 7 | G-2 | Short Staccato |
| 8 | G#-2 | Short Tenuto |
| 9 | A-2 | Trill (Major 2nd) |
| 10 | A#-2 | Trill (Minor 2nd) |

### Flutes a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Harmonics |
| 2 | D-2 | Long Hollow |
| 3 | D#-2 | Long Overblown |
| 4 | E-2 | Long |
| 5 | F-2 | Multitongue |
| 6 | F#-2 | Short Marcato Sfz |
| 7 | G-2 | Short Marcato |
| 8 | G#-2 | Short Overblown |
| 9 | A-2 | Short Staccato |
| 10 | A#-2 | Short Tenuto |
| 11 | B-2 | Trill (Major 2nd) |
| 12 | C-1 | Trill (Minor 2nd) |

### Oboe Solo

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long |
| 2 | D-2 | Multitongue |
| 3 | D#-2 | Short Marcato |
| 4 | E-2 | Short Staccato |
| 5 | F-2 | Short Tenuto |
| 6 | F#-2 | Trill (Major 2nd) |
| 7 | G-2 | Trill (Minor 2nd) |

### Oboes a2

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long |
| 2 | D-2 | Short Marcato |
| 3 | D#-2 | Short Staccato |
| 4 | E-2 | Trill (Major 2nd) |
| 5 | F-2 | Trill (Minor 2nd) |

### Piccolo Flute

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long Flutter |
| 1 | C#-2 | Long Harmonics |
| 2 | D-2 | Long |
| 3 | D#-2 | Multitongue |
| 4 | E-2 | Short Marcato |
| 5 | F-2 | Short Staccato |
| 6 | F#-2 | Short Tenuto |
| 7 | G-2 | Trill (Major 2nd) |
| 8 | G#-2 | Trill (Minor 2nd) |

## SSO Solo Strings

These are the **2025 Solo Strings added inside Spitfire Symphony Orchestra**.
They are a different sample/library component from the separate product
`Spitfire Solo Strings`, so they are documented independently here.

Only the standard **All Techniques** patches are included. `Core Techniques`,
`Decorative Techniques`, and `Performance` patches are excluded by project scope.

Current SSO documentation and the 2025 SSO UI show the standard All Techniques
palette beginning at C-2 and ordered as follows:

### Solo Violin - All Techniques

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Spiccato |
| 2 | D-2 | Staccato |
| 3 | D#-2 | Pizzicato |
| 4 | E-2 | Short Harmonics |
| 5 | F-2 | Long Harmonics |

### Solo Violin 2 - All Techniques

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Spiccato |
| 2 | D-2 | Staccato |
| 3 | D#-2 | Pizzicato |
| 4 | E-2 | Short Harmonics |
| 5 | F-2 | Long Harmonics |

### Solo Viola - All Techniques

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Spiccato |
| 2 | D-2 | Staccato |
| 3 | D#-2 | Pizzicato |
| 4 | E-2 | Short Harmonics |
| 5 | F-2 | Long Harmonics |

### Solo Cello - All Techniques

| MIDI Note No. | Kontakt note | Technique |
|---:|:---:|---|
| 0 | C-2 | Long |
| 1 | C#-2 | Spiccato |
| 2 | D-2 | Staccato |
| 3 | D#-2 | Pizzicato |
| 4 | E-2 | Short Harmonics |
| 5 | F-2 | Long Harmonics |

### SSO Solo Strings control notes

- `Long` and `Long Harmonics`: sustained material; use CC1 for dynamics and CC11 for expression.
- `Spiccato`, `Staccato`, `Pizzicato`, `Short Harmonics`: short material; use note Velocity for the primary dynamic/accent response.
- The current SSO Solo Strings UI exposes a Vibrato control, but CC21 response should still be treated as articulation-specific rather than assumed for every technique.

## Not mapped here yet

### Percussion / Kickstart note maps

Percussion ensemble patches require a **note-map reference**, even though they do not
need a conventional orchestral articulation-keyswitch table.

The SSO manual explicitly distinguishes two keyboard regions in Kickstart:

- **Red keys** = keyswitch range for selecting articulations/techniques.
- **Coloured keys** = playable range of the selected articulation/technique.
  Each mapped technique has its own colour-coded key or range.

For a patch such as `Ensembles - Contemporary`, the MIDI Note No. is therefore not a
musical pitch in the orchestral sense.  It is a **trigger address** for an instrument /
hit / technique.  A useful AI Tracker map must preserve this information, for example:

```text
MIDI Note No. / range -> instrument -> hit or technique
```

The supplied Kontakt screenshot shows the factory ensemble containing:

- Claves
- Guiro
- Ratchet
- Trash Metal
- Sleighbells
- Cowbells
- Shakers
- Tambourines

and shows their playable trigger regions as separate colour-coded keyboard blocks.

**Important:** the manual says Kickstart ships with pre-mapped Kontakt patches, but
also allows every hit/technique to be remapped to any desired MIDI note.  Therefore
exact percussion Note No. / ranges should be recorded from the factory patch (or an
unchanged factory instance) rather than inferred from an instrument's acoustic pitch.

For AI Tracker, these note maps are valuable because without them the AI can know
that a patch contains (for example) Claves or Guiro but still not know **which MIDI
Note No. actually triggers it**.

The present revision records the rule and patch contents but does **not invent exact
range endpoints** from the screenshot.  Exact factory Note No. ranges should be added
from direct Kontakt inspection / factory-map extraction.

Harp and piano are likewise not forced into the ordinary orchestral keyswitch table;
document them separately when their actual MIDI operation is needed.

## Source cross-check notes

- **SSO User Manual, Symphonic Percussion / Kickstart (pp. 45-51):**
  - Kickstart patches are pre-mapped but fully remappable.
  - The Kontakt keyboard's red range is the keyswitch range.
  - Coloured keys are the playable range of the selected technique, with each
    mapped articulation/technique colour-coded.
  - `CC BASED VELOCITY` optionally maps Dynamics/mod-wheel to note velocity.
  - `ROLL ON HIGH VEL.` optionally triggers rolls at high velocity where available.
  - Variations can have their own keyswitches (default variation KS start C-2).
- **Current SSO product/manual data:** instrument inventory and ordered orchestral
  technique lists used for the Strings / Brass / Woodwinds factory-map derivation.
- **SSO 2025 Solo Strings:** current SSO documentation identifies Solo Violin,
  Solo Violin 2, Solo Viola and Solo Cello All Techniques patches. A current 2025 SSO
  interface image shows the six-button order `Long`, `Spiccato`, `Staccato`,
  `Pizzicato`, `Short Harmonics`, `Long Harmonics`, with `KEYSWITCHES C-2`.
- **Project policy:** UACC is intentionally not used.

## AI Tracker rule of use

1. Send the keyswitch **before** the musical note; do not place both at the same startBeat and rely on insertion order.
2. Use raw MIDI Note No. from this table, not octave-name arithmetic.
3. For sustained musical shaping, prefer CC1; use CC11 as expression trim.
4. Do not generate CC21 merely because a technique is a Long. Use it only after the relevant SSO patch/articulation is known to respond.
5. If Kontakt's visible factory palette disagrees with this derived table, Kontakt wins; record the empirical mapping and update this file.

## Revision status

- Standard factory **All Techniques** scope only.
- UACC intentionally excluded by project decision.
- Individual/single-articulation, Performance, Core Techniques, Decorative Techniques, Legacy, Curated, Motif, reduced/lightweight, TM and similar alternate patches intentionally excluded.
- Strings / Brass / Woodwinds standard All-Techniques factory-map derivation included.
- SSO Solo Strings standard All Techniques patches included as a separate SSO section.
- Percussion handled only where a note-map is operationally useful to AI Tracker.
