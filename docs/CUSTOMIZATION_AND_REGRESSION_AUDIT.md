# Customization Inventory & Regression Audit

**Scope:** `firmware/OpenBCI_Cyton_Library_custom/` vs the untouched base clone
`firmware/OpenBCI_Cyton_Library/` (OpenBCI Cyton Library v3.1.5, MIT, target PIC32MX250F128B).
**Version:** `3.3.0-vsc`. **Audit date:** 2026-06-03 (verified from source diff this session).

All customization is **additive**. The stock single-channel `z` impedance path and the default
streaming / 33-byte packet format are unchanged. Custom code executes only when an explicit
`g` / `k` / `l` / `m` / `n` command is sent.

---

## 1. What is built out

### Source growth (line deltas vs base clone)
| File | Base | Custom | Delta |
|---|---|---|---|
| `OpenBCI_32bit_Library.cpp` | 4483 | 5038 | +555 |
| `OpenBCI_32bit_Library.h` | 390 | 437 | +47 |
| `OpenBCI_32bit_Library_Definitions.h` | 396 | 465 | +69 |

New non-source deliverables: `examples/ImpedanceModes/`, `docs/WIRE_FORMAT.md`,
`tests-ptw-assert/OpenBCI_Impedance_FastCycle_Test/`, `tests-ptw-assert/host_logic/`.

### Family 1 — fast-cycle / batched impedance
Routed through new multi-char parser state `MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_IMPEDANCE`.
ASCII-hex args decoded by new helper `getHexForAsciiChar()`.

| Command | Form | Firmware action |
|---|---|---|
| `g` | `g <4hex P-mask> Z` | `leadOffBatchSetMask()` — set P lead-off on all masked channels in one SDATAC/RDATAC pass (all-at-once) |
| `k` | `k <4hex P-mask> <2hex dwell> Z` | `impedanceFastCycleStart()` — firmware walks channels one step per emitted sample via `serviceImpedanceFastCycle()`, using stream-preserving `leadOffInlineSetChannel()` (SDATAC→WREG→RDATAC) |
| `l` | `l` | immediate `impedanceFastCycleStop()` |
| `m` | `m (1\|0) Z` | toggle surfacing of `LOFF_STATP` / `LOFF_STATN` DC-comparator bits |

Wire marker (magic **0x0FC5**, rides RAW_AUX): per sample `[magic][active channel 1-based][flags]`.
Flags: `SETTLING` (8-sample settle window the host discards), `LAST_SAMPLE`, `DONE`,
`DC_STATP`, `DC_STATN`. Dwell default 32 samples (~128 ms @ 250 SPS), range 1–255.

### Family 2 — extended commands (`n` + sub-op)
Routed through new multi-char parser state `MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_EXTENDED`.
Implements feature backlog items 2, 3, 4, 5.

| Command | Form | Firmware action |
|---|---|---|
| `n p` | `n p <slot:1h><mask:4h><gain:1h><input:1h><flags:1h> Z` | `extProfileDefine()` — write one of 4 RAM-resident `AcqProfile` slots (mask, gain reg, input code, bias-include bit, SRB2 bit). Host is source of truth; nothing in EEPROM |
| `n a` | `n a <slot:1h> Z` | `extProfileApply()` — atomic, stream-preserving apply of all 16 channels in one pass; returns false on invalid slot |
| `n c` | `n c <mask:4h> Z` | `extChannelPowerMask()` — atomic active-channel power mask; packet stays 33 bytes (no true bandwidth subsetting) |
| `n t` | `n t <cadence:2h> Z` | contact/railing telemetry every N samples via `buildContactTelemetryFrame()` (magic **0x0FC6**); lead-off from per-sample ADS status word, railing when \|sample\| > 8323072 (~99.2% full scale); zero extra SPI |
| `n e` | `n e <code:4h> Z` | queue a 16-bit event code latched into the next sample (magic **0x0FC7**) |

### Aux-field marker magics (RAW_AUX side channels)
- `0x0FC5` — fast-cycle impedance
- `0x0FC6` — contact/railing telemetry
- `0x0FC7` — sample-locked event code

---

## 2. Regression audit — every change to original lines

Audited every diff hunk that **removes or alters** an original line (the only way stock
accuracy or operation could change). Complete list across all three source files:

1. **Version string** — `printAll("v3.1.5")` → the `3.3.0-vsc` response string. Cosmetic;
   only the `V` command output changes.
2. **Four reinstated lead-off functions** — `leadOffSetForChannel`, `leadOffSetForAllChannels`,
   `leadOffConfigureSignalForAll`, `leadOffConfigureSignalForTargetSS` were **commented-out dead
   code** (`//`) in the base clone; the build un-commented them. Net-additive — nothing stock
   called them, so no existing path changed.
3. **One enum trailing comma** — `MULTI_CHAR_CMD_INSERT_MARKER` gained a comma so two new enum
   members could follow. List extension, no value reassignment.
4. **`Definitions.h`** — zero removed/changed lines. Purely additive.

### Per-sample hot path is gated
The custom hooks sit at the top of `sendChannelData(PACKET_TYPE)`
(`OpenBCI_32bit_Library.cpp:1500`) as one `if / else if / else if` chain:

```
if (impedanceFastCycleActive)              { serviceImpedanceFastCycle(); packetType = RAW_AUX; }
else if (extEventCodePending)              { ...emit 0x0FC7...;            packetType = RAW_AUX; }
else if (extTelemetryCadence > 0 && ...)   { buildContactTelemetryFrame(); packetType = RAW_AUX; }
```

All three gates initialize **inactive** (`impedanceFastCycleActive = false`,
`extEventCodePending = false`, `extTelemetryCadence = 0`; init at lines ~1416–1417). When nothing
is armed, the chain falls through, `packetType` keeps its passed-in value, and execution drops
into the **byte-for-byte original** `wifi.present` streaming block. Residual cost on a stock
sample is three failed integer comparisons — no effect on data, packet format, or timing.

### Conclusion
Nothing alters stock accuracy or operationality. The default `z` impedance path and the normal
streaming / packet path are untouched and run custom code only on an explicit `g`/`k`/`l`/`m`/`n`
command.

---

## 3. Known "may be affected" items (unchanged from prior records)

- **The new `g`/`k`/`l`/`m`/`n` commands themselves** — the intended new behavior.
- **Feature 1, photodiode hardware stimulus trigger** — deferred; needs a physical add.
- **Never compiled for the PIC32 target** — no Arduino IDE + chipKIT-core toolchain in this
  environment. Verified at source/logic level and by host logic tests
  (`impedance_fastcycle_logic_test.cpp` 35 assertions, `extended_commands_logic_test.cpp` 27
  assertions — all pass, re-run 2026-06-03). A clean target build is the one item not asserted.
