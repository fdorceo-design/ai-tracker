# NORDISK KONTRABASS — HAVE Audio AI Tracker Reference

Updated: 2026-09-08

- Instrument/type: **Upright Bass / Double Bass**
- Source: **Supplied Nordisk Kontrabass.nki/.nkr/.nkc; articulation names extracted from the NKI script/resource data**
- Pitch convention: **C4 = MIDI 60**

## Melodic / playable articulation groups confirmed in NKI

- Whales Legato
- Glissando Whales
- Harmony of Whales
- Harmony of Whales 2
- Pizzicato
- Double Stops Pizzicato
- Thumb Choked Pizz
- Weird Pizzicato
- Harmonics
- Natural Harmonics
- Dirty Tremolo

## Texture / trigger groups confirmed in NKI

- Twisted Pizzicato Textures
- Wood Body Percussion
- Bow Textures
- Pizz Drone 1
- Pizz Drone 2
- Pizz Drone 3

## Additional internal performance groups visible in NKI

- Whale short/long glissando up/down groups
- Nordlegato sustain / interval / release / repeated-note groups
- Pizzicato glissando
- Pizzicato harmonics
- Harmonic long / short groups
- Spiccato group

## Performance controls relevant to AI Tracker

- **Legato Speed** is explicitly exposed in the NKI script/UI.
- The instrument uses separate melodic and texture selectors.
- Tempo/sync controls exist for texture/loop-oriented material, but their UI/mixer implementation is not needed as fixed MIDI-generation data unless an exact CC assignment is supplied.

## Keyswitch status

- The supplied NKI confirms the articulation names and internal groups.
- A reliable factory KS-note table is not directly recoverable from the supplied compiled NKI/resource files without guessing.
- Exact MIDI KS note numbers are therefore left unset.

## AI Tracker handling

- Treat `Wood Body Percussion` as percussive trigger material rather than a normal sustained pitched articulation.
- Treat `Pizz Drone 1/2/3` and texture groups as special trigger/texture modes.
- Treat `Whales Legato`, `Pizzicato`, `Harmonics`, `Natural Harmonics`, and `Dirty Tremolo` as the principal playable articulation families until exact factory KS notes are confirmed.
