# Karoryfer Samples — AI Tracker Reference Dictionary

Updated: 2026-09-08

This revision is intentionally limited to information AI Tracker needs to generate MIDI correctly:

- playable range when directly known from supplied SFZ
- keyswitch → articulation
- percussion / non-pitched note → sound or technique
- CC / Mod Wheel / switch values needed for performance
- velocity-dependent articulation behavior when relevant
- Mono / Poly / Legato / Pitch Bend behavior when relevant

Not included merely because it exists in the sample engine: microphone mixing, RR counts, internal sample filenames/regions, GUI/mixer data.

## Sections

- [Commercial/](Commercial/README.md) — paid Karoryfer products.
- [Free/](Free/README.md) — free Karoryfer products.
- [Other_or_Legacy/](Other_or_Legacy/README.md) — legacy/other products
  not fitting the Commercial/Free split.

## SFZ-verified products in this revision

- Kemençe of the Black Sea
- Nanfo
- The Hat With The Phat
- Three Tagelharpas
- Turkish Rebab
- Vengeful Bass
- Vengeful Cello
- Vengeful Viola
- Vengeful Violin
- War Tuba

## Pitch convention

- C4 = MIDI 60

## Missing ranges

A playable range is only stated when it can be established directly from the supplied SFZ. If the top-level program delegates key zones to missing `#include` mapping files, the range is left unset rather than inferred.
