---
title: Custom impedance firmware — wire format (impedance + extended commands)
date: 2026-06-03
firmware: OpenBCI_Cyton_Library_custom v3.3.0-vsc (fork of OpenBCI v3.1.5, PIC32MX250F128B)
applies_to: VSC_OpenBCI_Custom-Firmware/firmware/OpenBCI_Cyton_Library_custom
consumed_by: PROMPT_02 (OpenLab Recorder); MentalMouseLab (profiles / telemetry / event codes)
---

# Custom impedance firmware — wire format

This is the exact host-visible contract added by the custom firmware. The stock OpenBCI
serial protocol is otherwise unchanged; everything here is **additive**. The single-channel
`z CH P N Z` command and the default 33-byte packet format are **untouched**.

Reference for the underlying hardware/firmware/host split:
`docs/IMPEDANCE_HARDWARE_FIRMWARE_DISCUSSION.md`.

## 1. The three impedance styles and their commands

| Style | Command | Stock? | Return-path | Latency (16 ch) |
|---|---|---|---|---|
| Slow / sequential | `z CH P N Z` (per channel) | yes | clean (one channel) | ~40 s |
| All-at-once / parallel | `g <mask> Z` | host-side, stock-compatible | shared-bias cross-talk | ~2.5 s |
| Fast-cycle | `k <mask> <dwell> Z` | **requires this firmware** | clean (one channel) | ~ (16 × dwell) samples |

All new command characters — `g`, `k`, `l`, `m` — are unused by stock firmware **and** by
the Cyton SD-card char handler, so they never collide.

### 1.1 `g` — all-at-once batched lead-off (stock-compatible)

```
g  H3 H2 H1 H0  Z
```
- `H3 H2 H1 H0` = 4 ASCII-hex chars (`0`–`9`, `A`–`F`, case-insensitive) = a 16-bit channel
  mask, most-significant nibble first. Bit *i* (i = 0..15) drives **channel i+1**'s P input.
- Drives the P-input lead-off on every masked channel in a single `SDATAC`/`RDATAC` pass
  (no per-channel stream stop/restart). N inputs are left off, matching `z CH 1 0 Z`.
- Global drive is forced to the standard **6 nA / 31.2 Hz** (`LOFF_MAG_6NA` / `LOFF_FREQ_31p2HZ`).
- Examples: `gFFFFZ` = all 16 channels. `g00FFZ` = channels 1–8. `g0001Z` = channel 1 only.
- After issuing this, the host reads **one** simultaneous window and computes every channel
  with the standard ohms formula. (Same numbers the host-side `--all-at-once` path produces.)

### 1.2 `k` — fast-cycle (firmware-sequenced, the point of this firmware)

```
k  H3 H2 H1 H0  D1 D0  Z
```
- `H3..H0` = 16-bit channel mask (as above).
- `D1 D0` = 2 ASCII-hex chars = dwell **in samples per channel** (1–255; `0x00` falls back to
  the default 32). At 250 SPS, `0x20` (32) ≈ 128 ms per channel.
- Arms the on-chip sequencer. While the board is **streaming** (`b`), the firmware drives one
  masked channel's P-input lead-off at a time — ON → dwell `D` samples → OFF → next masked
  channel — entirely on the PIC32, **without** the per-channel stream stop/restart the stock
  `z` path pays. Clean single-channel return path, low latency.
- Order-independent with the stream start: you may send `k…Z` then `b`, or `b` then `k…Z`.
- The sweep ends automatically after the last masked channel; send `l` to stop early.
- Examples: `kFFFF20Z` = sweep all 16 channels, 32 samples each. `k00FF10Z` = sweep ch 1–8,
  16 samples each.

### 1.3 `l` — stop a fast-cycle impedance sweep (single char)

```
l
```
Immediately clears all lead-off, restores the pre-sweep packet type, lowers the gate. Safe
to send even if no sweep is running.

### 1.4 `m` — DC lead-off status surfacing (optional)

```
m  (1|0)  Z
```
- `m1Z` turns ON, `m0Z` turns OFF (default OFF).
- When ON, each fast-cycle sample's flags (see §2) include the ADS1299 `LOFF_STATP` /
  `LOFF_STATN` per-channel comparator bit for the **active** channel — an instant
  connected/not signal, separate from the AC magnitude. Costs one extra register read per
  sample only while a sweep is running.

## 2. Fast-cycle per-channel marker (how the host segments the window)

During a fast-cycle impedance sweep, every streamed sample is emitted as a **`RAW_AUX`** packet
(stop byte `0xC0 | PACKET_TYPE_RAW_AUX`). The standard 33-byte Cyton packet layout is
preserved; the **6 AUX bytes** carry three **big-endian `int16`** values:

| AUX short | Bytes | Meaning |
|---|---|---|
| `auxData[0]` | 0–1 | **marker magic = `0x0FC5`** — presence means "this packet is fast-cycle-tagged" |
| `auxData[1]` | 2–3 | **active channel**, 1-based (1–16) |
| `auxData[2]` | 4–5 | **flags** (bitfield, below) |

Packet byte layout (unchanged stock framing):
```
[0]    0xA0                      start byte
[1]    sampleCounter             0..255 rolling
[2..25] 24 bytes channel data    (3 bytes/ch, big-endian signed, as stock)
[26..31] 6 AUX bytes             = auxData[0..2] big-endian int16  <-- marker rides here
[32]   0xC0 | PACKET_TYPE_RAW_AUX stop byte
```

### Flags (`auxData[2]`)

| Bit (mask) | Name | Meaning |
|---|---|---|
| `0x0001` | `SETTLING` | Sample is within the first `SETTLE` (8) samples of a channel's dwell — **host should discard** (lead-off drive still ringing up). |
| `0x0002` | `LAST_SAMPLE` | Last sample of the current channel's dwell. |
| `0x0004` | `DONE` | Sweep complete. Set on the **final** sample, OR-ed with that sample's normal flags; the channel field still holds the last channel (no usable sample is lost). |
| `0x0100` | `DC_STATP` | (only if `m1Z`) `LOFF_STATP` comparator set for the active channel. |
| `0x0200` | `DC_STATN` | (only if `m1Z`) `LOFF_STATN` comparator set for the active channel. |

### Host segmentation algorithm (what PROMPT_02 implements)

```
for each packet in the streamed window:
    if stop_byte_type != RAW_AUX: continue
    magic, channel, flags = three big-endian int16 from the 6 aux bytes
    if magic != 0x0FC5: continue              # not a fast-cycle packet
    if flags & SETTLING: continue             # discard ring-up samples
    append this packet's 24-byte channel data to window[channel]
    if flags & DONE: break                    # sweep finished
# then, for each channel, band-pass 5-50 Hz, RMS, apply the ohms formula:
#   Z(ohms) = (sqrt(2) * Vrms) / 6.0e-9 - 2200   (clamp >= 0)
# Only the active channel's own data column is meaningful for that channel's segment.
```

The active channel's impedance is read from **its own** data column during its segment
(the channel being driven shows the lead-off tone on its trace). Because only one channel is
driven at a time, there is no shared-bias cross-talk — the fast-cycle numbers should track
the slow/sequential numbers, not the all-at-once ones.

## 2b. Extended commands (desktop-app versatility)

A second additive family, all under **one** top-level initiator `n` followed by a context-local
sub-op character, ASCII-hex args, and the `Z` latch. The sub-op chars (`p`/`a`/`c`/`t`/`e`) are
parsed only after `n`, so they collide with nothing. Gain code maps `0..6 -> x1/x2/x4/x6/x8/x12/x24`;
input code is the `ADSINPUT_*` low nibble (`0`=normal, `1`=shorted, `5`=test signal, …); flags bit0
= include in bias, bit1 = connect SRB2.

| Feature | Command | Effect |
|---|---|---|
| Define acquisition profile | `n p <slot:1h><mask:4h><gain:1h><input:1h><flags:1h> Z` | Store a profile in RAM slot 0–3 |
| Apply acquisition profile | `n a <slot:1h> Z` | Atomically reconfigure all 16 channels (one `writeChannelSettings` pass, stream-preserving) |
| Channel power mask | `n c <mask:4h> Z` | Atomically power on masked channels / power down the rest (keeps each channel's gain/input) |
| Contact/railing telemetry | `n t <cadence:2h> Z` | Emit a status frame every `cadence` samples (`00` = off) |
| Event code | `n e <code:4h> Z` | Latch a 16-bit event code into the **next** emitted sample |

Examples:
- `np000FF603Z` — define slot 0: channels 1–8, gain x24, normal input, bias+SRB2 on.
- `na0Z` — apply slot 0.
- `np10001403Z` — define slot 1: channel 1 only, gain x8 (good for a jaw-clench electromyography
  click channel that would clip at x24), normal input, bias+SRB2 on. `na1Z` applies it.
- `nc000FZ` — power only channels 1–4 (e.g. an occipital steady-state montage), rest off.
- `nt08Z` — contact/railing telemetry every 8 samples. `nt00Z` turns it off.
- `ne0142Z` — tag the next sample with event code `0x0142` (e.g. paradigm 1, trial 0x42).

### 2b.1 Contact/railing telemetry frame (`auxData[0]` magic `0x0FC6`)

Emitted every `cadence` samples as a RAW_AUX packet. Costs **no extra SPI**: lead-off contact comes
from the ADS1299 status word already read each sample; railing comes from the channel sample
magnitudes already in memory.

| AUX short | Meaning |
|---|---|
| `auxData[0]` | `0x0FC6` telemetry magic |
| `auxData[1]` | **railing** bitmask — bit i set ⇒ channel i+1 at/over ±99.2% full scale (`|sample| ≥ 0x7F0000`) |
| `auxData[2]` | **contact** bitmask — bit i set ⇒ channel i+1 lead-off comparator tripped (`LOFF_STATP` OR `LOFF_STATN`); i.e. likely poor electrode contact |

Bits 0–7 are the board ADS (channels 1–8); bits 8–15 are the Daisy ADS (channels 9–16). The desktop
app reads these for a live per-channel green/yellow/red without stopping to run an impedance check —
useful for catching an electrode that degrades mid-session before it corrupts a covariance estimate.

### 2b.2 Event-code frame (`auxData[0]` magic `0x0FC7`)

The single sample immediately following an `n e <code> Z` command is emitted as a RAW_AUX packet with
`auxData[0] = 0x0FC7`, `auxData[1] = the 16-bit event code`, `auxData[2] = 0`. Sample-accurate onset
(the firmware places it on the very next conversion), and the 16-bit code packs paradigm / trial /
condition for a clean map onto a Brain Imaging Data Structure `events.tsv`. This is independent of the
stock 1-byte `BOARD_MODE_MARKER` marker and does not require switching board modes.

### 2b.3 Marker-magic disambiguation

All custom side channels ride the RAW_AUX aux field and are told apart by `auxData[0]`:

| `auxData[0]` | Frame |
|---|---|
| `0x0FC5` | Fast-cycle impedance marker (§2) |
| `0x0FC6` | Contact/railing telemetry (§2b.1) |
| `0x0FC7` | Event code (§2b.2) |

These are mutually exclusive per packet; fast-cycle impedance and live paradigms are not run at the
same time. Sample-rate is intentionally **not** part of the profile-apply path (changing it restarts
the ADS via `initialize_ads`, not stream-preserving) — use the stock `~` command if needed.

## 3. Validation status

- Parser state machine, sequencer walk, marker tagging, and the host segmentation above are
  validated host-side (g++) by
  `firmware/OpenBCI_Cyton_Library_custom/tests-ptw-assert/host_logic/impedance_fastcycle_logic_test.cpp`
  (35 assertions, all passing).
- The extended commands (profiles / channel mask / telemetry / event codes) are validated by
  `firmware/OpenBCI_Cyton_Library_custom/tests-ptw-assert/host_logic/extended_commands_logic_test.cpp`
  (27 assertions, all passing).
- On-device compile (Arduino IDE + chipKIT-core, PIC32MX250F128B target) is required to flash;
  no PIC32 toolchain was available in the authoring environment, so the on-device build is the
  one remaining step before hardware use.

## 4. Out of scope (documented hardware limits)

- **Per-channel lead-off frequencies** — the ADS1299 `FLEAD_OFF` field is a single global
  setting; frequency-multiplexed simultaneous measurement is not possible on this silicon.
- **Removing shared-bias cross-talk** — a board-analog-topology problem (needs per-channel
  reference electrodes), not solvable in firmware. The fast-cycle style is the firmware-level
  mitigation: keep the clean single-channel return path, just sequence fast.
