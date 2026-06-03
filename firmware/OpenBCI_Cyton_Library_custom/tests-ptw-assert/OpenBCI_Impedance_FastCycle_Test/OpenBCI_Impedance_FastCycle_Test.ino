// OpenBCI_Impedance_FastCycle_Test.ino  --  VSC_OpenBCI_Custom-Firmware
//
// On-device (PIC32) PTW-Arduino-Assert test for the custom fast-cycle / batched
// impedance command parser + sequencer. Mirrors the conventions in
// OpenBCI_32bit_Libraries_Test.ino. Flash, then send any char on Serial1 to run.
//
// The pure logic is also covered host-side (no hardware) by
// tests-ptw-assert/host_logic/impedance_fastcycle_logic_test.cpp -- prefer that for CI.

#include <DSPI.h>
#include <EEPROM.h>
#include "OpenBCI_32bit.h"
#include "PTW-Arduino-Assert.h"

void setup() {
  Serial1.begin(115200);
  test.setSerial(Serial1);
  board.begin();
}

void loop() {
  if (Serial1.available()) {
    Serial1.read();
    go();
  }
}

// Feed a command string to the board one char at a time, as the serial loop would.
void feed(const char *s) {
  for (const char *p = s; *p; p++) {
    board.processChar(*p);
  }
}

void go() {
  test.begin();
  testAllAtOnceParser();
  testFastCycleParser();
  testDcStatusParser();
  testFastCycleStop();
  testProfileDefineApply();
  testChannelPowerMask();
  testTelemetryAndEventParsers();
  test.end();
}

void testAllAtOnceParser() {
  test.describe("g all-at-once parser sets leadOffSettings from hex mask");
  // channels 1-8 on (0x00FF)
  feed("g00FFZ");
  boolean ok = true;
  for (int i = 0; i < 8; i++) {
    if (board.leadOffSettings[i][PCHAN] != ON) ok = false;
    if (board.leadOffSettings[i][NCHAN] != OFF) ok = false;
  }
  test.assertBoolean(ok, true, "channels 1-8 P on / N off", __LINE__);
}

void testFastCycleParser() {
  test.describe("k fast-cycle parser arms sequencer with mask + dwell");
  feed("k00FF20Z");
  test.assertBoolean(board.impedanceFastCycleActive, true, "fast-cycle armed", __LINE__);
  test.assertEqual((int)board.impedanceChannelMask, 0x00FF, "mask 0x00FF", __LINE__);
  test.assertEqual((int)board.impedanceDwellSamples, 0x20, "dwell 32", __LINE__);
  test.assertEqual((int)board.impedanceActiveChannel, 1, "first active channel 1", __LINE__);
  board.impedanceFastCycleStop(); // clean up
}

void testDcStatusParser() {
  test.describe("m DC-status parser toggles flag");
  feed("m1Z");
  test.assertBoolean(board.impedanceDcStatusActive, true, "DC status ON", __LINE__);
  feed("m0Z");
  test.assertBoolean(board.impedanceDcStatusActive, false, "DC status OFF", __LINE__);
}

void testFastCycleStop() {
  test.describe("l stops a sweep and clears lead-off");
  feed("kFFFF10Z");
  board.processChar('l');
  test.assertBoolean(board.impedanceFastCycleActive, false, "sweep stopped", __LINE__);
  boolean cleared = true;
  for (int i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
    if (board.leadOffSettings[i][PCHAN] != OFF) cleared = false;
  }
  test.assertBoolean(cleared, true, "all lead-off cleared", __LINE__);
}

void testProfileDefineApply() {
  test.describe("n p / n a define and apply an acquisition profile");
  // slot 0: channels 1-8 (mask 0x00FF), gain code 6 (x24), input 0, flags 3 (bias+srb2)
  feed("np000FF603Z");
  feed("na0Z");
  test.assertEqual((int)board.channelSettings[0][POWER_DOWN], NO, "ch1 active", __LINE__);
  test.assertEqual((int)board.channelSettings[0][GAIN_SET], ADS_GAIN24, "ch1 gain x24", __LINE__);
  test.assertEqual((int)board.channelSettings[0][BIAS_SET], YES, "ch1 in bias", __LINE__);
}

void testChannelPowerMask() {
  test.describe("n c sets the active-channel power mask");
  feed("nc000FZ"); // channels 1-4 only
  test.assertEqual((int)board.channelSettings[0][POWER_DOWN], NO, "ch1 powered", __LINE__);
  test.assertEqual((int)board.channelSettings[4][POWER_DOWN], YES, "ch5 down", __LINE__);
}

void testTelemetryAndEventParsers() {
  test.describe("n t sets telemetry cadence; n e queues an event code");
  feed("nt08Z");
  test.assertEqual((int)board.extTelemetryCadence, 8, "telemetry cadence 8", __LINE__);
  feed("nt00Z");
  test.assertEqual((int)board.extTelemetryCadence, 0, "telemetry off", __LINE__);
  feed("ne0142Z");
  test.assertBoolean(board.extEventCodePending, true, "event queued", __LINE__);
  test.assertEqual((int)board.extEventCode, 0x0142, "event code 0x0142", __LINE__);
}
