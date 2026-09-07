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
rounded up to a four-beat boundary. The importer only reads the tempo at
time 0 — it does not parse later tempo-change or time-signature meta events
out of the file into scheduled events. The sequencer itself does support
mid-piece tempo and meter changes, just not by reading them from an imported
file automatically; see `POST /api/tempo-events` and `POST
/api/timesig-events` to add them explicitly after import.

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

## External MIDI routing (fallback for un-hostable plugins)

Some VST3 instruments (Kontakt 8, UJAM's BM-* line, and Synthesizer V were
confirmed) reliably fail to instantiate no matter how they're hosted --
in-process, out-of-process, with or without a device, synchronous or async.
Root cause undiagnosed after extensive investigation; only a live debugger
session would narrow it further. One unconfirmed guess: every plugin that
failed exposes multiple audio output busses (Kontakt's multi-out routing,
UJAM's stem outs, Synth V's separate voice busses) -- this host only ever
negotiates a single stereo output bus, so a plugin defaulting to or
requiring a multi-out configuration might be instantiating into a bus
layout this host never offers it. Not verified against a single-output
plugin known to fail, so treat it as a lead, not a diagnosis. For these,
route the track's notes to a real standalone build of the instrument
instead:

1. Install a virtual MIDI cable driver -- **loopMIDI**
   (https://www.tobias-erichsen.de/software/loopmidi.html) is free and known
   to work. Open it and create one port. A generic name like
   `AI Tracker to Standalone` is recommended over naming it after one
   specific plugin (e.g. "...to Kontakt"), since the same port works for
   any standalone app you route to.
2. In the standalone app (e.g. Kontakt 8 Standalone), set that virtual port
   as its MIDI input and your real interface as its audio output.
3. In AI Tracker, click a track's **toMIDI** button (or
   `POST /api/tracks/:id/midi-output {"device":"<port name>"}`) and pick the
   virtual port from the list (`GET /api/midi-outputs`).
4. Optionally launch the standalone app itself via
   `POST /api/launch-app {"path":"C:\\...\\Kontakt 8.exe"}`.

Audio from a track routed this way plays directly from the standalone app to
your real output device -- it is not mixed through AI Tracker, so it will
not appear in offline renders and has no level metering here. This trades
integration for reliability; auto-creating the virtual port programmatically
(via the teVirtualMIDI SDK loopMIDI itself is built on) was considered but
deferred -- it needs the SDK's exact C API, which requires downloading and
verifying Tobias Erichsen's SDK package rather than guessing signatures.

## Verification

Run `"AI Tracker.exe" --midi-self-test "C:\\absolute\\test-output"` to test
named MIDI export/import without loading third-party instruments. The process
exits with code 0 on success and writes `result.txt` plus MIDI test fixtures.
Tests cover Unicode names, repeated instruments, empty parts, timing,
velocity, keyswitches, initial tempo, invalid input, and legacy merge import.
