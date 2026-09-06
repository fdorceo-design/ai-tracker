# Named MIDI sessions

`POST /api/export/midi` accepts an absolute `path`, for example
`{"path":"C:\\Music\\Autumn Leaves.mid"}`. The file stem is the song title and
is written to the conductor track. Each engine track, including empty ones,
is exported separately with:

- Standard Track Name: `Part name | Instrument name`
- Standard Instrument Name
- A versioned `AITracker:` JSON text event containing the original part name,
  instrument name and VST3 path
- Notes, including articulation and FX keys, at 960 ticks per quarter

Existing files are replaced through a temporary file after successful writing.

Rename a part with `POST /api/tracks/:id/name`, body `{"name":"Lead Guitar"}`.
The instrument name is added automatically at export, so it should not be
manually appended to the part name.

## Restore separate parts

`POST /api/import/midi` with `{"path":"C:\\Music\\Autumn Leaves.mid"}` appends
one engine track per MIDI note track. Empty tracks carrying AI Tracker metadata
are also restored. Existing tracks are retained. Playback stops and resets;
the initial MIDI tempo is restored, and the loop covers the resulting notes,
rounded up to a four-beat boundary. Later tempo changes and meter maps are not
supported by this sequencer.

VST3 instruments are loaded on the message thread from their saved paths.
If a path is missing, the same filename in the standard Windows VST3 directory
is tried. Name-only files also recognize Ample Bass U, Guitar L, Bass J and
Bass A in that directory. Unknown or missing instruments leave the notes and
instrument identity intact and return a per-track warning. Only import MIDI
from sources whose instrument-loading metadata you trust. `"loadPlugins":false`
restores parts without instantiating plugins.

The response includes `title`, `bpm`, and `tracks` with new IDs, names,
instrument names, note counts, load status, and any warnings. API-created and
imported tracks appear in the main UI with usable Editor buttons once loaded.

Passing `trackId` explicitly retains the old behavior: all MIDI note tracks
are merged into that one existing track, without loading plugins or changing
tempo. Old unnamed exports can be separated, but their instrument identities
cannot be inferred automatically. MIDI channels within a single source track
are not split; notes play on channel 1.

MIDI does not save plugin presets, mixer settings, effects, or sample library
state. This feature restores instrument identity, not an exact audio session.

## Verification

Run `"AI Tracker.exe" --midi-self-test "C:\\absolute\\test-output"` to test
named MIDI export/import without loading third-party instruments. The process
exits with code 0 on success and writes `result.txt` plus MIDI test fixtures.
Tests cover Unicode names, repeated instruments, empty parts, timing,
velocity, keyswitches, initial tempo, invalid input, and legacy merge import.
