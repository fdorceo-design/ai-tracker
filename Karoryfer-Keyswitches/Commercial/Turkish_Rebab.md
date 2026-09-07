# Turkish Rebab — AI Tracker Reference

Updated: 2026-09-08

- Manufacturer: **Karoryfer Samples**
- Category: **Bowed strings**
- Status: **Commercial**
- Pitch convention: **C4 = MIDI 60**
- Source: **01-keyswitch(6).sfz**

## MIDI controls used by AI Tracker

| CC | Function |
|---:|---|
| 1 | Dynamics |
| 21 | Vibrato Depth |
| 32 | Transpose |
| 105 | Mono Switch |
| 107 | Release |
| 112 | Vibrato Speed |
| 115 | Vibrato Delay |
| 116 | Vibrato Fade |
| 117 | Humanize |

## Performance rules

- Exact KS mapping is delegated to included `modules/keyswitch.sfz` / `modules/groups/all_keysw.sfz`.

## Verification

- Only performance information useful to MIDI generation is retained.
- Mixer/microphone controls, internal sample-zone details and other nonessential engine data are omitted.
- Values not supported by the supplied SFZ are not guessed.
