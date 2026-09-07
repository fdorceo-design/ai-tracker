# BBC Symphony Orchestra Professional — AI Tracker Reference (Work in Progress)

Updated: 2026-09-08

This is a working reference for operating **BBC Symphony Orchestra Professional** from AI Tracker.

Priority of evidence used in this document:

1. User's actual running BBCSO instance / screenshots
2. Official Steinberg BBCSO Pro Dorico playback template and included information PDF
3. Official Spitfire BBCSO documentation/manuals
4. Derived/inferred values only where explicitly marked

Raw MIDI Note No. is the machine-facing value used by AI Tracker.

---

## 1. MIDI octave / pitch-number convention — confirmed

**This library uses a different octave-naming convention than the other
Kontakt reference docs in this repo** (Sacconi/Solo Strings/SSO/PP013 all use
`MIDI Note No. 0 = C-2`; BBCSO uses the standard `C4 = 60` convention below).
Kontakt's octave display is a per-library/per-instance setting, not a global
constant — always check which convention a given doc's own confirmed data
implies rather than assuming it matches another library's.

The user's actual running BBCSO instance was checked against AI Tracker.

### Violins 1
- Lowest playable note shown/confirmed: **G3**
- AI Tracker raw MIDI Note No.: **55**

Therefore for this BBCSO work:

- C4 = MIDI 60
- G3 = MIDI 55
- E2 = MIDI 40

The earlier provisional conversion that placed G3 at MIDI 67 was wrong by one octave and must not be used.

### Horn verification
BBCSO Horn Extended patch:
- Lowest playable note: **E2**
- raw MIDI Note No.: **40**

This agrees with the same pitch-number convention.

---

## 2. General BBCSO performance-control rules

From the Steinberg BBCSO Pro Dorico template documentation:

- Dynamics are assigned per technique to either:
  - **CC1**, or
  - **Note Velocity**
- **CC11** is used as a secondary dynamics / expression controller.
- Individual instruments now have their own expression maps in the 2025 template revision.
- Unpitched percussion uses percussion maps, with an expression map also assigned for CC1 / CC11 dynamics control.
- Pitched percussion uses expression maps.
- Keyswitches/techniques can be enabled or disabled in Dorico expression maps.
- Technique-specific playback delay is present in the expression maps and may differ by instrument/technique.

AI Tracker rule:
- Do not assume every sustained technique uses identical controllers.
- Do not assume every short technique uses identical controllers without checking the expression-map data.
- Prefer the actual Dorico expression-map setting where available.

---

## 3. BBCSO Professional playable ranges — confirmed conversion basis

The following manual ranges use the confirmed BBCSO/AI Tracker pitch-number convention above.

| Instrument | Manual range | Raw MIDI Note No. |
|---|---:|---:|
| Violins 1 / Leader | G3–C#7 | 55–97 |
| Violins 2 / Leader | G3–C#7 | 55–97 |
| Violas / Leader | C3–F#6 | 48–90 |
| Celli / Leader | C2–Bb5 | 36–82 |
| Contrabasses / Leader | C1–F#3 | 24–54 |
| Piccolo | D5–C8 | 74–108 |
| Solo Flute / a3 | B3–C7 | 59–96 |
| Bass Flute | C2–C4 | 36–60 |
| Solo Oboe / a3 | B3–F6 | 59–89 |
| Cor Anglais | E3–A5 | 52–81 |
| Solo Clarinet / a3 | D3–E6 | 50–88 |
| Bass Clarinet | Bb1–E5 | 34–76 |
| Contrabass Clarinet | Bb0–C4 | 22–60 |
| Bassoon / a3 | Bb1–D5 | 34–74 |
| Contrabassoon | C1–Bb3 | 24–58 |
| Solo Horn / a4 | E2–F5 | 40–77 |
| Solo Trumpet / section | E3–C6 | 52–84 |
| Solo Tenor Trombone / a3 | G1–D5 | 31–74 |
| Bass Trombones a2 | E1–G4 | 28–67 |
| Contrabass Trombone | E1–G4 | 28–67 |
| Tuba | D1–E4 | 26–64 |
| Contrabass Tuba | D1–C4 | 26–60 |
| Cimbasso | E0–E3 | 16–52 |

Notes:
- Treat these as library/manual playable ranges, not necessarily the range of every individual articulation.
- The pitch-number conversion is now empirically confirmed by Violins 1 and Horn.

---

# 4. Timpani — empirical reference

Patch:
- **Percussion: Timpani**
- BBCSO player version shown: **v1.12.14**
- **TRANSPOSE = 0**

The plugin keyboard therefore reflects the untransposed playable region.

The user's actual Timpani produces normal pitched timpani notes over its mapped pitched range; raw MIDI note and sounding pitch are treated as the same pitch-number relationship used elsewhere in BBCSO.

## Timpani articulation keyswitches — empirically confirmed

The following were read directly from the user's running BBCSO instance.

| BBCSO keyswitch label | Technique |
|---|---|
| C-1 | Hits |
| C#-1 | Rolls |
| D-1 | Hits Soft |
| D#-1 | Rolls Soft |
| E-1 | Hits Hotrods |
| F-1 | Long Rolls Hotrods |
| F#-1 | Hits Damped |
| G-1 | Hits Super Damped |

Important:
- These are the BBCSO plugin's displayed **keyswitch note labels**.
- Raw MIDI Note No. for these keyswitches still needs to be taken from the expression-map data or empirically mapped before being considered machine-authoritative.
- Do not derive raw MIDI keyswitch numbers from the displayed octave name alone.

## Timpani technique-family classification

### Roll-family
- Rolls
- Rolls Soft
- Long Rolls Hotrods

### Hit-family
- Hits
- Hits Soft
- Hits Hotrods
- Hits Damped
- Hits Super Damped
- Additional second-page techniques reported by the user are also hit-family.

Likely control model:
- Roll-family -> CC1
- Hit-family -> Velocity

Status:
- This classification is musically/structurally consistent.
- Final AI rule should use the Dorico expression-map assignments where available rather than relying on naming alone.

## Timpani playable pitch range — empirically confirmed from the running plugin display

The user's actual BBCSO Timpani instance was shown with:

- `TRANSPOSE = 0`
- the playable region visibly indicated on the plugin keyboard
- the same raw-MIDI-to-pitch convention already confirmed by Violins 1 and Horn

The displayed playable region is therefore treated as authoritative for AI Tracker.

Playable range:

- **C3–D5**
- **raw MIDI Note No. 48–74**

The user additionally confirmed that these are normal pitched timpani notes: raw MIDI Note No. corresponds to sounding pitch in the same way as the other pitched BBCSO instruments.

This item is **confirmed** and must not be returned to the unresolved list merely because the official PDF does not print a low/high Timpani range table.

---

# 5. Dorico template source status

Files obtained:

- `BBCSO Pro Information.pdf`
- `BBCSO Pro.dorico_pt`
- `BBCSO Pro with Piano.dorico_pt`

The included information PDF states that:
- applying the playback template automatically loads patches and expression/percussion maps,
- tuned percussion uses expression maps,
- unpitched percussion uses percussion maps,
- dynamics are technique-dependent and use CC1 or note velocity with CC11 as secondary dynamics.

The current playback template revision gives individual expression maps to individual instruments so that correct technique-specific delays can be assigned.

The `.dorico_pt` playback-template package is the preferred source for extracting:
- technique names,
- keyswitch assignments,
- CC1 vs Velocity behavior,
- CC11 behavior,
- technique delay values,
- percussion maps.

---

# 6. Work still to do

## Can be extracted from supplied template data
- Full instrument-by-instrument expression-map list
- Raw keyswitch values where encoded
- CC1 vs Velocity per technique
- CC11 settings
- Technique-specific delays
- Pitched-percussion expression maps
- Unpitched-percussion MIDI trigger/percussion maps

## Needs empirical confirmation only if not present in source material
- Any pitched-percussion playable range other than the already-confirmed Timpani range when it is not published / encoded
- Any technique whose controller behavior is ambiguous
- Any keyswitch whose raw MIDI Note No. cannot be reliably recovered from the template
- Any plugin display/mapping that disagrees with the template

## Do not ask the user to re-confirm
- BBCSO pitch-number octave convention
- Violins 1 minimum G3 = raw MIDI 55
- Horn Extended minimum E2 = raw MIDI 40
- Timpani playable range C3–D5 = raw MIDI 48–74
- Timpani first-page keyswitch labels listed above
- Timpani hit-family vs roll-family distinction

---

# 6A. Correction / state-preservation note

During the first MD pass, the Timpani playable range was accidentally downgraded from confirmed to unresolved. That was incorrect.

For this working reference, once a fact has been empirically confirmed in the user's running BBCSO instance, later document revisions must preserve that status unless new contradictory evidence appears.

In particular:
- Timpani playable range C3–D5 / raw MIDI 48–74 is confirmed.
- It must not be reclassified as approximate, inferred, or pending confirmation.

---

# 7. Project rule for future checking

Workflow:

1. Search manual / official distributed material first.
2. Extract all machine-usable data available there.
3. Mark each datum as:
   - confirmed from source,
   - empirically confirmed,
   - inferred,
   - unresolved.
4. Only ask the user about genuinely unresolved information that matters to AI Tracker.
5. Do not ask the user to verify information that can already be obtained from the supplied official material.
6. When the user's actual BBCSO instance conflicts with a template/manual, the running instance wins and the discrepancy should be documented.
