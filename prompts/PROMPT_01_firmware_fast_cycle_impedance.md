# PROMPT 01 — Rewrite Cyton firmware for a fast-cycle impedance check (keep parallel + slow)

**Run this in the `VSC_OpenBCI_Custom-Firmware` workspace.** Base source is cloned at
`firmware/OpenBCI_Cyton_Library/` (MIT, v3.1.5, PIC32MX250F128B). Branch from it; do not edit
the upstream clone in place — copy to `firmware/OpenBCI_Cyton_Library_custom/` (or a git branch)
and work there. Read `docs/IMPEDANCE_HARDWARE_FIRMWARE_DISCUSSION.md` first.

## Goal
Add a firmware-sequenced **fast-cycle** impedance mode and a batched **all-at-once** mode to the
Cyton firmware, while **keeping the existing slow/sequential `z CH P N Z` behavior untouched** as
the default. Reflashable on the existing Cyton (PIC32 via Arduino IDE + chipKIT-core).

## The three styles the firmware must support
1. **Slow / sequential (unchanged):** the existing `z CH P N Z` path. Do not regress it.
2. **All-at-once (batched):** enable lead-off on all (or a supplied set of) channels in one pass,
   no per-channel stream stop/restart, so the host can read a single simultaneous window.
3. **Fast-cycle (new, the point of this project):** firmware sequences channels P-input lead-off
   ON → dwell N ms → OFF → next channel, entirely on the PIC32, **without** stopping/restarting
   the stream per channel. Emits a small per-channel marker so the host can segment the window
   per channel. Clean single-channel return path (no cross-talk) at a fraction of the latency.

## Exact symbols to work from (verified in `OpenBCI_32bit_Library.cpp` / `.h`)
- `processIncomingLeadOffSettings(char)` — the `z` command parser (latch = `OPENBCI_CHANNEL_IMPEDANCE_LATCH` = `Z`). Add a new multi-char command (pick an unused leading char, e.g. a `Z`-prefixed variant or a new `processIncoming...` handler registered in the `MULTI_CHAR_CMD_*` switch around line 144-154) for the new modes; do not overload the existing `z` semantics.
- `leadOffSettings[OPENBCI_NUMBER_OF_CHANNELS_DAISY][OPENBCI_NUMBER_OF_LEAD_OFF_SETTINGS]` — per-channel P/N state.
- `leadOffSetForAllChannels(void)` and `leadOffConfigureSignalForAll(byte, byte)` — **already exist**; build the batched/all-at-once mode on these instead of writing new register code.
- `streamSafeLeadOffSetForChannel(channel, pchan, nchan)` — the slow path; note it stops/restarts the stream (the `wasStreaming` guard). The fast-cycle mode must write `LOFF_SENSP`/`LOFF_SENSN` (0x0F/0x10) **without** the stop/restart — do the register writes inline in the streaming loop between sample emits.
- `configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ)` — leave the global magnitude/frequency at 6 nA / 31.2 Hz (the silicon allows only one global `FLEAD_OFF` frequency — see the discussion doc; do not attempt per-channel frequencies).
- `LOFF_STATP` (0x12) / `LOFF_STATN` (0x13) — **optionally** surface these DC comparator status bytes in the output stream for an instant per-channel connected/not flag (a separate, cheap signal from the AC magnitude). Gate behind its own command so it doesn't change the default packet format.

## Requirements / constraints
- Default behavior and packet format unchanged unless a new mode is explicitly commanded.
- Fast-cycle dwell time and channel set must be host-configurable via the new command's arg bytes.
- Per-channel segmentation: emit a lightweight marker (e.g., a status-byte field or a known
  sentinel sample) at each channel transition so the host can attribute the window. Document the
  exact wire format in `docs/WIRE_FORMAT.md`.
- Respect the `numberOfIncomingSettingsProcessed*` multi-char parser conventions already in the file.
- Keep it buildable in the Arduino IDE with chipKIT-core for the PIC32MX250F128B; update
  `library.properties` version and `changelog.md`.
- Add an example sketch under `examples/` that exercises all three modes.

## Validation (no-hardware where possible)
- Compile for the PIC32 target (chipKIT-core) — confirm it builds.
- If a software simulation harness exists (`tests-ptw-assert/`), add assertions for the new
  command parser state machine (byte sequence → expected `leadOffSettings` state + mode flag).
- Document the expected host-visible behavior so PROMPT_02's host code can target it.

## Deliverables
- Custom firmware branch with the three modes + optional DC-status surfacing.
- `docs/WIRE_FORMAT.md` describing the new command(s) + per-channel marker.
- Updated `changelog.md` + version bump.
- An `examples/` sketch demonstrating slow / all-at-once / fast-cycle.

## Out of scope (documented limits, do not attempt)
- Per-channel lead-off frequencies (ADS1299 `FLEAD_OFF` is global — silicon limit).
- Removing shared-bias cross-talk (board analog topology — needs per-channel reference hardware).
