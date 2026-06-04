# VSC_OpenBCI_Custom-Firmware

> ⚠️ **Status: UNTESTED ON HARDWARE.** The custom firmware has been validated at the
> source/logic level (host-side parser/sequencer tests pass; the stock paths are verified
> unaffected) but has **not yet been compiled and flashed to a physical Cyton (PIC32MX250F128B)**.
> Treat it as not-yet-operational. This notice will be updated once on-device operationality is
> confirmed.

Custom OpenBCI Cyton firmware + host integration for **faster electrode impedance checks**, plus a host-side selector that lets OpenLab Recorder run any of three impedance-measurement styles.

Created 2026-06-02. The custom firmware is **built** and lives in
`firmware/OpenBCI_Cyton_Library_custom/` (v3.3.0-vsc) — an additive fork of the OpenBCI Cyton
Library v3.1.5. It is validated at the source/logic level (host-side tests pass; stock paths
verified unaffected) but **not yet compiled or flashed to hardware** (see status above). The
host-side OpenLab Recorder integration is **specified as a prompt** (`prompts/PROMPT_02_*`) and
is not yet implemented.

## What's here

| Path | Contents |
|---|---|
| `docs/IMPEDANCE_HARDWARE_FIRMWARE_DISCUSSION.md` | The full hardware/firmware/software analysis we worked out — the three-layer answer (silicon vs firmware vs host), the OpenBCI_GUI ohms formula, the three measurement styles, and the exact firmware symbols involved (verified against the cloned source) |
| `prompts/PROMPT_01_firmware_fast_cycle_impedance.md` | Ready-to-execute spec: rewrite the Cyton firmware to add a firmware-sequenced **fast-cycle** impedance mode + a batched **all-at-once** mode, keeping the existing **slow/sequential** path. Points at the real functions to change |
| `prompts/PROMPT_02_openlab_recorder_three_mode_dropdown.md` | Ready-to-execute spec: make OpenLab Recorder run any of the three styles, chosen from a dropdown settings menu |
| `firmware/OpenBCI_Cyton_Library_custom/` | The custom firmware (v3.3.0-vsc) — additive fork of OpenBCI Cyton Library v3.1.5 (PIC32MX250F128B). Host logic tests under `tests-ptw-assert/` |
| `docs/WIRE_FORMAT.md` | Host-side wire contract for the three RAW_AUX side channels (impedance `0x0FC5`, telemetry `0x0FC6`, event `0x0FC7` markers) |
| `docs/CUSTOMIZATION_AND_REGRESSION_AUDIT.md` | Inventory of all additions + regression audit proving the stock `z` path and default streaming are unaffected |

## The three impedance styles (defined once, used everywhere)

1. **Slow / sequential** — one channel at a time, ~31.25 Hz lead-off, clean single bias-return path. The OpenBCI-GUI-faithful ground truth (~40 s for 16 channels). Works on **stock firmware**.
2. **Parallel / all-at-once** — drive every channel's lead-off together, read one window, compute all in parallel (~2.5 s). Carries shared-bias cross-talk bias. Works on **stock firmware** (host-side; already implemented in OpenLab Recorder `impedance_check.py --all-at-once` and MentalMouseLab `impedance_sweep(parallel=True)`).
3. **Fast-cycle (firmware-accelerated sequential)** — the firmware sequences channels on the PIC32 without the per-channel stream stop/restart the stock firmware does, giving sequential-quality (clean return path) at far lower latency. **Requires this custom firmware.**

Slow and Parallel are the clean-vs-fast endpoints; Fast-cycle is the new middle that keeps the clean return path but removes the latency.

## Sources & references

The implementation was derived from primary sources — not from any forum. The changes were
worked out by reading the official OpenBCI source and the chip datasheet directly:

- **OpenBCI Cyton Library source** (MIT, v3.1.5) — <https://github.com/OpenBCI/OpenBCI_Cyton_Library> — the base firmware; its command dispatch, lead-off functions, and 33-byte packet format are what these changes extend.
- **Texas Instruments ADS1299 datasheet (SBAS499C)** — <https://www.ti.com/lit/gpn/ads1299> — the `SDATAC`/`WREG`/`RDATAC` command semantics behind the stream-preserving register writes, the 6 nA / 31.25 Hz lead-off drive, and the single global `FLEAD_OFF` frequency limit.
- **OpenBCI_GUI source** (MIT) — <https://github.com/OpenBCI/OpenBCI_GUI> — the canonical impedance ohms formula and the `z`-command sequence.
- **OpenBCI documentation — Cyton SDK** — <https://docs.openbci.com/Cyton/CytonSDK/> — the `z`/`x` serial command grammar and the 1–16 channel ASCII codes.

The novel parts (the firmware-sequenced fast-cycle impedance sweeper, the `n` extended-command family, and
the `0x0FC5`/`0x0FC6`/`0x0FC7` aux marker frames) were newly designed by combining these sources;
they are not copied from any existing implementation.

## Acknowledgments

This project exists only because of **OpenBCI** and the **OpenBCI community**. The entire
firmware foundation — the Cyton library, the ADS1299 acquisition layer, the command protocol,
and the years of open hardware and open-source work behind them — is theirs. Our absolute
thanks go to OpenBCI and every community contributor who built and shared it.

This repository is an **additive derivative** of the official OpenBCI Cyton firmware
(<https://github.com/OpenBCI/OpenBCI_Cyton_Library>, MIT License, Copyright (c) OpenBCI). The
original firmware is vendored here with its MIT license retained in full; our changes are
additive and the stock behavior is preserved. All original copyright and attribution belong
to OpenBCI.

This work was made possible by **QIM Group volunteers who donated their time** to help manifest the code's evolution.

Its creation was catalyzed by the **Consciousness Studies Club** and its brain-computer-interface
lab, whose research environment and instrument needs sparked this work.

## Disclaimer

This is research firmware, provided **"as is" without warranty of any kind** (see
[`LICENSE`](LICENSE)), and — per the status notice at the top — it is **untested on
hardware**. It is **not a medical device** and is not intended for clinical,
diagnostic, or any safety-critical use.

Flashing custom firmware to an OpenBCI Cyton (PIC32MX250F128B) is done **entirely at
your own risk**, including the risk of rendering the board inoperable. Responsibility
for any use rests solely with the user. Anyone who builds, flashes, copies, modifies,
or redistributes this firmware is solely responsible for:

- the safety of their own hardware, electrical setup, and electrode application;
- obtaining any required ethics / institutional review board approval and informed
  consent before recording from human participants;
- compliance with all applicable laws, regulations, and data-protection rules in
  their jurisdiction.

To the maximum extent permitted by law, the authors and contributors, the
**Consciousness Studies Club** and its brain-computer-interface lab, and the
**QIM Group** accept no liability for any loss, harm, damage, or hardware failure
arising from use of this firmware. Using it constitutes acceptance of these terms.

## License

Additive contributions in this repository are released under the MIT License
(Copyright (c) 2026 QIM Group and contributors) — see [`LICENSE`](LICENSE). The vendored
OpenBCI firmware remains under its original OpenBCI MIT license. Reproduced third-party
copyright and permission notices are in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
