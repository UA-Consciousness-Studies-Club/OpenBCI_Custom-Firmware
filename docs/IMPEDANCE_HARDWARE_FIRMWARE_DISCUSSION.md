---
title: OpenBCI Impedance — Hardware vs Firmware vs Software (the worked-out analysis)
date: 2026-06-02
sources_verified_this_session: [ADS1299 datasheet SBAS499C, OpenBCI_GUI source (MIT), OpenBCI_Cyton_Library source (MIT, v3.1.5), docs.openbci.com Cyton SDK]
---

# OpenBCI Impedance: where the limits actually live

This captures the analysis behind this project: how OpenBCI measures electrode impedance,
why it does one channel at a time, and which layer (silicon / firmware / host software) each
limit lives in — so the firmware rewrite targets the right thing.

## How impedance is measured on the Cyton

There is no impedance "library call." Impedance is **derived** from the ADS1299 lead-off
detection:

1. The host sends the serial command **`z CH P N Z`** — apply a lead-off test signal to a
   channel's P and/or N input (P/N = 1 applied / 0 not). The ADS1299 injects a **~31.25 Hz**
   AC current (fCLK/2¹⁶) through the electrode. Source: docs.openbci.com/Cyton/CytonSDK.
2. Band-pass the channel 5–50 Hz, take RMS, and convert to ohms with the OpenBCI_GUI formula
   (verbatim from `OpenBCI_GUI/DataProcessing.pde:46-52` + `BoardCyton.pde:47,51`, MIT):

   ```
   Z(ohms) = (sqrt(2) * Vrms_volts) / 6.0e-9  -  2200      # clamp >= 0
   ```
   lead-off drive = 6 nA, series resistor = 2.2 kOhm.

3. Channel-address codes for the command: 1–8 → `1`–`8`; Daisy 9–16 → `Q,W,E,R,T,Y,U,I`.

## Why OpenBCI cycles one channel at a time — and which layer owns each limit

There are **three** layers. The "one at a time" behavior is not where most people assume.

### Layer 1 — ADS1299 silicon (the analog front end)
- **Simultaneous-sampling** delta-sigma converter: every channel on a chip is digitized at the
  same instant, always. Reading all 16 at once (two chips on Cyton+Daisy) is native.
- `LOFF_SENSP` / `LOFF_SENSN` are **per-channel** registers — "any configuration of input
  channels can be selected" (datasheet). Each selected channel gets its own 6 nA source.
- **The one true silicon limit:** the lead-off frequency is the `FLEAD_OFF[1:0]` field inside
  the single global `LOFF` register (address 0x04). It is **one frequency for the whole chip**
  (7.8 Hz *or* 31.2 Hz). So you cannot give each channel a *different* frequency — which means
  frequency-multiplexed simultaneous measurement (the cross-talk-free way) is **not possible on
  the ADS1299**. That needs a different/added analog front end, not firmware.

### Layer 2 — Cyton firmware (PIC32MX250F128B, `OpenBCI_Cyton_Library`, MIT, v3.1.5, reflashable)
Verified symbols in `OpenBCI_32bit_Library.cpp` / `.h`:
- `processIncomingLeadOffSettings(char)` — the `z CH P N Z` parser (multi-byte state machine;
  latch char `OPENBCI_CHANNEL_IMPEDANCE_LATCH` = `Z`).
- `leadOffSettings[OPENBCI_NUMBER_OF_CHANNELS_DAISY][...]` — per-channel P/N lead-off state (16 ch).
- `streamSafeLeadOffSetForChannel(channel, pchan, nchan)` — applies one channel's lead-off but
  **stops and restarts the stream around the register write** (`wasStreaming` guard). This
  per-channel stop/restart is the **latency tax** that makes a host-driven sweep slow.
- `leadOffSetForAllChannels(void)` and `leadOffConfigureSignalForAll(byte, byte)` — **already
  exist**: a batch path is half-built in the stock firmware.
- `configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ)` — sets the global magnitude
  (6 nA) + frequency (31.2 Hz) in `LOFF`.
- `LOFF_STATP` (0x12) / `LOFF_STATN` (0x13) — the **DC comparator status registers**: an instant
  per-channel connected/not-connected flag, separate from the AC magnitude measurement, and
  routable into the output data stream.

**So the firmware owns:** the per-channel stop/restart latency (removable), whether a batched
all-channel lead-off is exposed as a command (half-present), and whether the DC contact-status
comparators are surfaced (not currently).

### Layer 3 — host software (BrainFlow / OpenBCI_GUI / your Python)
This is where the "measure one channel, wait, measure the next" loop actually runs today.
**Same-frequency all-at-once is purely a host choice** — enable lead-off on all channels, read
one simultaneous window, done. No firmware change needed (already implemented host-side in
OpenLab Recorder and MentalMouseLab).

## The shared-bias cross-talk (why all-at-once is biased)
When several channels are driven at once, each channel's 6 nA current returns through the
**shared bias / SRB reference electrode**. The summed return raises common-mode and couples
channels through the shared return-path impedance, biasing each channel's derived ohms. This is
**board analog topology** — neither the ADC chip nor firmware. Removing it cleanly would require
per-channel references (a board redesign). The practical mitigation is the **fast-cycle** style:
keep the clean single-channel return path, just sequence channels fast in firmware.

## Bottom line (which layer fixes what)
| Goal | Layer | Fix |
|---|---|---|
| Same-frequency all-at-once sweep | host software | done (host enables all `z`, one read) |
| Low-latency clean sequential ("fast-cycle") | **firmware** | sequence on PIC32, drop per-channel stop/restart; reflash existing Cyton |
| Instant connected/not flag | firmware | surface `LOFF_STATP/STATN` DC comparators in the stream |
| Per-channel-frequency simultaneous (cross-talk-free) | ADS1299 silicon | not possible on this chip — needs different/added AFE |
| Remove shared-bias cross-talk | board analog topology | per-channel reference — board redesign |

**Rewritten firmware runs on the existing hardware:** `OpenBCI_Cyton_Library` is MIT and the
Cyton is field-reflashable (PIC32 via Arduino IDE + chipKIT-core; `DefaultBoard.ino` is the
shipped firmware). So the fast-cycle + batched modes are reachable on the boards you already own;
only the per-channel-frequency idea is out of reach without new analog hardware.
