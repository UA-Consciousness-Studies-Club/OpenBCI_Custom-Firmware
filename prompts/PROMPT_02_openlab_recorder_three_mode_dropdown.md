# PROMPT 02 — OpenLab Recorder: run any of the three impedance styles from a dropdown

**Run this in the `VSC_OpenBCI_Custom-Firmware` workspace** (it edits the separate
`VSC_OpenLab_Rcrder` repo — clone/checkout it alongside, or work against the local
`/mnt/c/VSC_OpenLab_Rcrder`). Read `docs/IMPEDANCE_HARDWARE_FIRMWARE_DISCUSSION.md` first.

## Goal
Let OpenLab Recorder run **any of the three impedance styles**, chosen from a **dropdown settings
menu**, and display the per-channel kΩ + green/yellow/red result.

## The three styles (already partly built — reuse, do not rewrite the math)
OpenLab Recorder already has `src/impedance_check.py` with the verified ohms formula and two
modes:
1. **Slow / sequential** — `measure_channel(...)` loop (default; stock firmware).
2. **Parallel / all-at-once** — `measure_all_at_once(...)` / `--all-at-once` (stock firmware).
3. **Fast-cycle (firmware)** — NEW: requires the custom firmware from PROMPT_01. Add a
   `measure_fast_cycle(...)` that issues the new firmware command, reads the single streamed
   window, and segments it per channel using the firmware's per-channel marker
   (see `docs/WIRE_FORMAT.md` produced by PROMPT_01).

## What to build
1. **A settings dropdown.** OpenLab Recorder is currently headless (`launch.py` + `src/` CLI).
   Add a minimal settings panel (Tkinter is in the standard library — no new heavy dependency;
   or match whatever UI toolkit `launch.py` already uses — inspect it first). The panel exposes:
   - **Impedance mode** dropdown: `Slow / sequential (clean, ~40 s)`, `All-at-once (fast, cross-talk)`,
     `Fast-cycle (firmware, clean + fast — requires custom firmware)`.
   - Board selector (cyton / daisy), serial port, settle/window seconds, channel subset.
   - A "Run impedance" button and a per-channel results table (kΩ + GREEN/YELLOW/RED), reusing
     `quality_band()`.
2. **Wire the dropdown to the three code paths** in `impedance_check.py`. Gate "Fast-cycle" so it
   detects (or is told) whether the connected board runs the custom firmware; if stock firmware,
   disable that option with a tooltip ("requires VSC_OpenBCI_Custom-Firmware").
3. **Keep the existing CLI** (`--all-at-once`, `--self-test`, etc.) working; the dropdown is an
   additional front end over the same functions, not a replacement.
4. **Do not block the live LSL stream / recording** — the impedance run should pause/own the board
   the same way the existing code does (the bridge already publishes an LSL outlet; the impedance
   sweep must not push the 31.5 Hz tone to it).

## Constraints
- OpenLab Recorder is **MIT** — keep new code MIT; do not vendor copyleft code.
- No new heavy dependency for the UI (prefer stdlib Tkinter; justify anything else).
- The `measure_fast_cycle` path must degrade gracefully (clear error) if the board lacks the
  custom firmware or the wire-format marker is absent.

## Validation
- `--self-test` style check for `measure_fast_cycle`'s window-segmentation logic against a
  synthetic multi-channel buffer with injected markers (no hardware).
- Manual: dropdown switches modes; results table populates; CLI unaffected.

## Deliverables
- Settings panel module + dropdown wired to the three modes.
- `measure_fast_cycle()` in `impedance_check.py` (consuming PROMPT_01's wire format).
- Updated `notes/IMPEDANCE_AND_LIVE_VIEW.md` documenting the three-mode selector.
