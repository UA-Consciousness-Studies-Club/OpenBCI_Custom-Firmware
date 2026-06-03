/*
 * ImpedanceModes.ino  --  VSC_OpenBCI_Custom-Firmware
 *
 * Demonstrates the three electrode-impedance measurement styles supported by this
 * custom Cyton firmware. Flash this onto a Cyton (PIC32MX250F128B) with the Arduino
 * IDE + chipKIT-core, exactly like DefaultBoard.ino. The board then accepts the
 * commands below over its serial link (USB dongle / RFduino / wifi shield).
 *
 * The three styles -- send these byte sequences from the host:
 *
 *   1) SLOW / SEQUENTIAL  (stock, unchanged, clean single-channel return path, ~40 s/16ch)
 *        z CH P N Z         e.g.  z 1 1 0 Z   -> drive channel 1 P-input lead-off on
 *      Host loops one channel at a time, reading a window between each.
 *
 *   2) ALL-AT-ONCE / PARALLEL  (stock-compatible, ~2.5 s, carries shared-bias cross-talk)
 *        g <4 hex mask> Z   e.g.  gFFFFZ      -> drive P-input lead-off on all 16 channels
 *                                  g00FFZ      -> channels 1-8 only
 *      Host then reads ONE simultaneous window and computes every channel from it.
 *
 *   3) FAST-CYCLE  (NEW, firmware-sequenced, clean return path + low latency)
 *        k <4 hex mask> <2 hex dwell> Z
 *                           e.g.  kFFFF20Z    -> sweep all 16 channels, 0x20 = 32 samples each
 *                                  k00FF10Z    -> sweep ch 1-8, 16 samples each
 *      The firmware drives one channel at a time on the PIC32 (no per-channel stream
 *      stop/restart) and tags every streamed sample with the active channel in the AUX
 *      field. Start the stream ('b') BEFORE or AFTER arming -- the sweep runs while
 *      streaming. Send 'l' to stop a sweep early.
 *
 *   Optional: DC lead-off status surfacing (instant connected/not flag)
 *        m 1 Z   -> ON     m 0 Z   -> OFF
 *      When ON, each fast-cycle sample's aux flags include the ADS1299 LOFF_STATP/STATN
 *      comparator bit for the active channel.
 *
 * Per-channel marker wire format during a fast-cycle impedance sweep (see docs/WIRE_FORMAT.md):
 *   Each 33-byte packet is a RAW_AUX packet whose 6 aux bytes are three big-endian shorts:
 *     auxData[0] = 0x0FC5            (segmentation marker magic)
 *     auxData[1] = active channel   (1-based; 0 on the final DONE sample's channel field
 *                                    is NOT used -- the final sample keeps its channel and
 *                                    ORs the DONE flag instead)
 *     auxData[2] = flags: bit0 SETTLING, bit1 LAST_SAMPLE, bit2 DONE,
 *                         bit8 DC_STATP, bit9 DC_STATN
 *   The host segments the single streamed window by watching auxData[1] transitions and
 *   discards samples with the SETTLING bit set.
 *
 * EXTENDED COMMANDS (desktop-app versatility -- one initiator 'n' + sub-op + hex args + 'Z'):
 *   n p <slot:1h><mask:4h><gain:1h><input:1h><flags:1h> Z   define acquisition profile
 *   n a <slot:1h> Z                                          apply profile (atomic, all channels)
 *   n c <mask:4h> Z                                          set active-channel power mask
 *   n t <cadence:2h> Z                                       contact/railing telemetry every N samples (00=off)
 *   n e <code:4h> Z                                          latch a 16-bit event code into the next sample
 *   e.g.  np000FF603Z then na0Z  -> ch1-8 gain x24 normal bias+srb2
 *         nc000FZ                 -> power only ch1-4
 *         nt08Z                   -> telemetry every 8 samples (aux magic 0x0FC6)
 *         ne0142Z                 -> tag next sample with event 0x0142 (aux magic 0x0FC7)
 *   See docs/WIRE_FORMAT.md sections 2b.1-2b.3 for the telemetry/event aux frame layouts.
 *
 * NOTE: this example omits the SD-card glue for clarity. Use DefaultBoard.ino if you need
 * SD logging; the impedance and extended commands work identically there.
 */

#include <DSPI.h>
#include <OpenBCI_Wifi_Master_Definitions.h>
#include <OpenBCI_Wifi_Master.h>
#include <OpenBCI_32bit_Library.h>
#include <OpenBCI_32bit_Library_Definitions.h>

void setup() {
  // Bring up the OpenBCI board (ADS1299 + accelerometer + serial)
  board.begin();

  // Bring up wifi (no-op if no shield attached)
  wifi.begin(true, true);
}

void loop() {
  // Stream + emit samples. While a fast-cycle impedance sweep is armed, sendChannelData() steps the
  // sequencer and tags each sample's aux field automatically -- nothing extra to do here.
  if (board.streaming) {
    if (board.channelDataAvailable) {
      board.updateChannelData();

      if (board.curAccelMode == board.ACCEL_MODE_ON) {
        if (board.accelHasNewData()) {
          board.accelUpdateAxisData();
        }
      }

      board.sendChannelData();
    }
  }

  // Route inbound serial chars to the board command parser (this is what handles
  // z / g / k / l / m and every other command).
  if (board.hasDataSerial0()) {
    board.processChar(board.getCharSerial0());
  }
  if (board.hasDataSerial1()) {
    board.processChar(board.getCharSerial1());
  }

  board.loop();
  wifi.loop();

  if (wifi.hasData()) {
    board.processCharWifi(wifi.getChar());
  }

  if (!wifi.sentGains) {
    if (wifi.present && wifi.tx) {
      wifi.sendGains(board.numChannels, board.getGains());
    }
  }
}
