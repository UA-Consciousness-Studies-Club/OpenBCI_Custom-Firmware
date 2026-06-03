/*
OpenBCI 32bit Library
Place the containing folder into your libraries folder insdie the arduino folder in your Documents folder

This library will work with a single OpenBCI 32bit board, or
an OpenBCI 32bit board with an OpenBCI Daisy Module attached.

*/

#include "OpenBCI_32bit_Library.h"

/***************************************************/
/** PUBLIC METHODS *********************************/
/***************************************************/
// CONSTRUCTOR
OpenBCI_32bit_Library::OpenBCI_32bit_Library()
{
  initializeVariables();
}

/**
* @description: The function the OpenBCI board will call in setup.
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::begin(void)
{
  // Bring the board up
  boardBegin();
}

/**
* @description: The function the OpenBCI board will call in setup. Turns sniff mode
*  on and allows you to tap into the serial port that is broken out on the OpenBCI
*  32bit board. You must alter this file:
*   On Mac:
*     `/Users/username/Documents/Arduino/hardware/chipkit-core/pic32/variants/openbci/Board_Defs.h`
*   On Windows:
*     `C:\Users\username\Documents\Arduino\hardware\chipkit-core\pic32\variants\openbci\Board_Defs.h`
* Specifically lines `311` and `313` from `7` and `10` to `11` and `12` for
*   `_SER1_TX_PIN` and `_SER1_RX_PIN` respectively. Check out this sweet gif if
*   you are a visual person http://g.recordit.co/3jH01sMD6Y.gif
*  You will need to reflash your board! But now you can connect to pins `11`
*    `12` via a FTDI serial port driver, really any serial to USB driver would
*    work. Remember to use 3V3, 115200 baud, and have a common ground!
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::beginDebug(void)
{
  beginDebug(OPENBCI_BAUD_RATE);
}

void OpenBCI_32bit_Library::beginDebug(uint32_t baudRate)
{
  // Bring the board up
  boolean started = boardBeginDebug(baudRate);

  if (started)
  {
    Serial1.println("Board up");
    sendEOT();
  }
  else
  {
    Serial1.println("Board err");
    sendEOT();
  }
}

/**
* @description Called in every `loop()` and checks `Serial0`
* @returns {boolean} - `true` if there is data ready to be read
*/
boolean OpenBCI_32bit_Library::hasDataSerial0(void)
{
  // TODO: Need to undo this comment out
  // if (!Serial0) return false;
  // if (!iSerial0.rx) return false;
  if (iSerial0.rx && Serial0.available())
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @description Called in every `loop()` and checks `Serial0`
* @returns {boolean} - `true` if there is data ready to be read
*/
boolean OpenBCI_32bit_Library::hasDataSerial1(void)
{
  // TODO: Need to undo this comment out
  // if (Serial1) return false;
  // if (!iSerial1.rx) return false;
  if (iSerial1.rx && Serial1.available())
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @description Called if `hasDataSerial0` is true, returns a char from `Serial0`
* @returns {char} - A char from the serial port
*/
char OpenBCI_32bit_Library::getCharSerial0(void)
{
  return Serial0.read();
}

/**
* @description Called if `hasDataSerial1` is true, returns a char from `Serial1`
* @returns {char} - A char from the serial port
*/
char OpenBCI_32bit_Library::getCharSerial1(void)
{
  return Serial1.read();
}

/**
* @description Process one char at a time from serial port. This is the main
*  command processor for the OpenBCI system. Considered mission critical for
*  normal operation.
* @param `character` {char} - The character to process.
* @return {boolean} - `true` if the command was recognized, `false` if not
*/
boolean OpenBCI_32bit_Library::processChar(char character)
{
  if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
  {
    Serial1.print("pC: ");
    Serial1.println(character);
  }

  if (checkMultiCharCmdTimer())
  { // we are in a multi char command
    switch (getMultiCharCommand())
    {
    case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL:
      processIncomingChannelSettings(character);
      break;
    case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF:
      processIncomingLeadOffSettings(character);
      break;
    case MULTI_CHAR_CMD_SETTINGS_BOARD_MODE:
      processIncomingBoardMode(character);
      break;
    case MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE:
      processIncomingSampleRate(character);
      break;
    case MULTI_CHAR_CMD_INSERT_MARKER:
      processInsertMarker(character);
      break;
    case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_IMPEDANCE:
      processIncomingImpedanceSettings(character);
      break;
    case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_EXTENDED:
      processIncomingExtendedSettings(character);
      break;
    default:
      break;
    }
  }
  else
  { // Normal...
    switch (character)
    {
    //TURN CHANNELS ON/OFF COMMANDS
    case OPENBCI_CHANNEL_OFF_1:
      streamSafeChannelDeactivate(1);
      break;
    case OPENBCI_CHANNEL_OFF_2:
      streamSafeChannelDeactivate(2);
      break;
    case OPENBCI_CHANNEL_OFF_3:
      streamSafeChannelDeactivate(3);
      break;
    case OPENBCI_CHANNEL_OFF_4:
      streamSafeChannelDeactivate(4);
      break;
    case OPENBCI_CHANNEL_OFF_5:
      streamSafeChannelDeactivate(5);
      break;
    case OPENBCI_CHANNEL_OFF_6:
      streamSafeChannelDeactivate(6);
      break;
    case OPENBCI_CHANNEL_OFF_7:
      streamSafeChannelDeactivate(7);
      break;
    case OPENBCI_CHANNEL_OFF_8:
      streamSafeChannelDeactivate(8);
      break;
    case OPENBCI_CHANNEL_OFF_9:
      streamSafeChannelDeactivate(9);
      break;
    case OPENBCI_CHANNEL_OFF_10:
      streamSafeChannelDeactivate(10);
      break;
    case OPENBCI_CHANNEL_OFF_11:
      streamSafeChannelDeactivate(11);
      break;
    case OPENBCI_CHANNEL_OFF_12:
      streamSafeChannelDeactivate(12);
      break;
    case OPENBCI_CHANNEL_OFF_13:
      streamSafeChannelDeactivate(13);
      break;
    case OPENBCI_CHANNEL_OFF_14:
      streamSafeChannelDeactivate(14);
      break;
    case OPENBCI_CHANNEL_OFF_15:
      streamSafeChannelDeactivate(15);
      break;
    case OPENBCI_CHANNEL_OFF_16:
      streamSafeChannelDeactivate(16);
      break;

    case OPENBCI_CHANNEL_ON_1:
      streamSafeChannelActivate(1);
      break;
    case OPENBCI_CHANNEL_ON_2:
      streamSafeChannelActivate(2);
      break;
    case OPENBCI_CHANNEL_ON_3:
      streamSafeChannelActivate(3);
      break;
    case OPENBCI_CHANNEL_ON_4:
      streamSafeChannelActivate(4);
      break;
    case OPENBCI_CHANNEL_ON_5:
      streamSafeChannelActivate(5);
      break;
    case OPENBCI_CHANNEL_ON_6:
      streamSafeChannelActivate(6);
      break;
    case OPENBCI_CHANNEL_ON_7:
      streamSafeChannelActivate(7);
      break;
    case OPENBCI_CHANNEL_ON_8:
      streamSafeChannelActivate(8);
      break;
    case OPENBCI_CHANNEL_ON_9:
      streamSafeChannelActivate(9);
      break;
    case OPENBCI_CHANNEL_ON_10:
      streamSafeChannelActivate(10);
      break;
    case OPENBCI_CHANNEL_ON_11:
      streamSafeChannelActivate(11);
      break;
    case OPENBCI_CHANNEL_ON_12:
      streamSafeChannelActivate(12);
      break;
    case OPENBCI_CHANNEL_ON_13:
      streamSafeChannelActivate(13);
      break;
    case OPENBCI_CHANNEL_ON_14:
      streamSafeChannelActivate(14);
      break;
    case OPENBCI_CHANNEL_ON_15:
      streamSafeChannelActivate(15);
      break;
    case OPENBCI_CHANNEL_ON_16:
      streamSafeChannelActivate(16);
      break;

    // TEST SIGNAL CONTROL COMMANDS
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
      activateAllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
      activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);
      break;

    // CHANNEL SETTING COMMANDS
    case OPENBCI_CHANNEL_CMD_SET: // This is a multi char command with a timeout
      startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL);
      numberOfIncomingSettingsProcessedChannel = 1;
      break;

    // LEAD OFF IMPEDANCE DETECTION COMMANDS
    case OPENBCI_CHANNEL_IMPEDANCE_SET:
      startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF);
      numberOfIncomingSettingsProcessedLeadOff = 1;
      break;

    // CUSTOM FAST-CYCLE / BATCHED IMPEDANCE COMMANDS (VSC_OpenBCI_Custom-Firmware)
    case OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET: // 'g' + 4 hex P-mask + 'Z'
    case OPENBCI_IMPEDANCE_FAST_CYCLE_SET:  // 'k' + 4 hex P-mask + 2 hex dwell + 'Z'
    case OPENBCI_IMPEDANCE_DC_STATUS_SET:   // 'm' + ('1'|'0') + 'Z'
      impedanceMultiCharMode = character;
      impedanceArgCounter = 0;
      startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_IMPEDANCE);
      break;
    case OPENBCI_IMPEDANCE_FAST_CYCLE_STOP: // 'l' single char, immediate stop
      impedanceFastCycleStop();
      if (!streaming) { printSuccess(); printAll("Fast-cycle stopped"); sendEOT(); }
      break;

    // CUSTOM EXTENDED COMMANDS (VSC_OpenBCI_Custom-Firmware): 'n' <subop> args 'Z'
    case OPENBCI_EXTENDED_CMD_SET:
      extArgCounter = 0;
      startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_EXTENDED);
      break;

    case OPENBCI_CHANNEL_DEFAULT_ALL_SET: // reset all channel settings to default
      if (!streaming)
      {
        printAll("updating channel settings to");
        printAll(" default");
        sendEOT();
      }
      streamSafeSetAllChannelsToDefault();
      break;
    case OPENBCI_CHANNEL_DEFAULT_ALL_REPORT: // report the default settings
      reportDefaultChannelSettings();
      break;

    // DAISY MODULE COMMANDS
    case OPENBCI_CHANNEL_MAX_NUMBER_8: // use 8 channel mode
      if (daisyPresent)
      {
        removeDaisy();
      }
      else if (wifi.present && wifi.tx)
      {
        wifi.sendStringLast("No daisy to remove");
      }
      break;
    case OPENBCI_CHANNEL_MAX_NUMBER_16: // use 16 channel mode
      if (daisyPresent == false)
      {
        attachDaisy();
      }
      if (daisyPresent)
      {
        printAll("16");
      }
      else
      {
        printAll("8");
      }
      sendEOT();
      break;

    // STREAM DATA AND FILTER COMMANDS
    case OPENBCI_STREAM_START: // stream data
      if (curAccelMode == ACCEL_MODE_ON)
      {
        enable_accel(RATE_25HZ);
      } // fire up the accelerometer if you want it
      wifi.tx = commandFromSPI;
      if (wifi.present && wifi.tx)
      {
        wifi.sendStringLast("Stream started");
        iSerial0.tx = false;
      }
      // Reads if the command is not from the SPI port and we are not in debug mode
      if (!commandFromSPI && !iSerial1.tx)
      {
        // If the sample rate is higher than 250, we need to drop down to 250Hz
        //  to not break the RFduino system that can't handle above 250SPS.
        if (curSampleRate != SAMPLE_RATE_250)
        {
          streamSafeSetSampleRate(SAMPLE_RATE_250);
          delay(50);
        }
      }
      streamStart(); // turn on the fire hose
      break;
    case OPENBCI_STREAM_STOP: // stop streaming data
      if (curAccelMode == ACCEL_MODE_ON)
      {
        disable_accel();
      } // shut down the accelerometer if you're using it
      wifi.tx = true;
      streamStop();
      if (wifi.present && wifi.tx)
      {
        wifi.sendStringLast("Stream stopped");
      }
      break;

    //  INITIALIZE AND VERIFY
    case OPENBCI_MISC_SOFT_RESET:
      boardReset(); // initialize ADS and read device IDs
      break;
    //  QUERY THE ADS AND ACCEL REGITSTERS
    case OPENBCI_MISC_QUERY_REGISTER_SETTINGS:
      if (!streaming)
      {
        printAllRegisters(); // print the ADS and accelerometer register values
      }
      break;

    // TIME SYNC
    case OPENBCI_TIME_SET:
      // Set flag to send time packet
      if (!streaming)
      {
        printAll("Time stamp ON");
        sendEOT();
      }
      curTimeSyncMode = TIME_SYNC_MODE_ON;
      setCurPacketType();
      break;

    case OPENBCI_TIME_STOP:
      // Stop the Sync
      if (!streaming)
      {
        printAll("Time stamp OFF");
        sendEOT();
      }
      curTimeSyncMode = TIME_SYNC_MODE_OFF;
      setCurPacketType();
      break;

    // BOARD TYPE SET TYPE
    case OPENBCI_BOARD_MODE_SET:
      startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_BOARD_MODE);
      optionalArgCounter = 0;
      break;

    // Sample rate set
    case OPENBCI_SAMPLE_RATE_SET:
      startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE);
      break;

    // Insert Marker into the EEG data stream
    case OPENBCI_INSERT_MARKER:
      startMultiCharCmdTimer(MULTI_CHAR_CMD_INSERT_MARKER);
      break;

    case OPENBCI_WIFI_ATTACH:
      if (wifi.attach())
      {
        printSuccess();
        printSerial("Wifi attached");
        sendEOT();
      }
      else
      {
        printFailure();
        printSerial("Wifi not attached");
        sendEOT();
      }
      break;
    case OPENBCI_WIFI_REMOVE:
      if (wifi.remove())
      {
        printSuccess();
        printSerial("Wifi removed");
      }
      else
      {
        printFailure();
        printSerial("Wifi not removed");
      }
      sendEOT();
      break;
    case OPENBCI_WIFI_STATUS:
      if (wifi.present)
      {
        printAll("Wifi present");
      }
      else
      {
        printAll("Wifi not present, send {");
        printAll(" to attach the shield");
      }
      sendEOT();
      break;
    case OPENBCI_WIFI_RESET:
      wifi.reset();
      printSerial("Wifi soft reset");
      sendEOT();
      break;
    case OPENBCI_GET_VERSION:
      printAll("v3.3.0-vsc");
      sendEOT();
      break;
    default:
      return false;
    }
  }
  return true;
}

/**
 * Start the timer on multi char commands
 * @param cmd {char} the command received on the serial stream. See enum MULTI_CHAR_COMMAND
 * @returns void
 */
void OpenBCI_32bit_Library::startMultiCharCmdTimer(char cmd)
{
  if (curDebugMode == DEBUG_MODE_ON)
  {
    Serial1.printf("Start multi char: %c\n", cmd);
  }
  isMultiCharCmd = true;
  multiCharCommand = cmd;
  multiCharCmdTimeout = millis() + MULTI_CHAR_COMMAND_TIMEOUT_MS;
}

/**
 * End the timer on multi char commands
 * @param None
 * @returns void
 */
void OpenBCI_32bit_Library::endMultiCharCmdTimer(void)
{
  isMultiCharCmd = false;
  multiCharCommand = MULTI_CHAR_CMD_NONE;
}

/**
 * Check for valid on multi char commands
 * @param None
 * @returns {boolean} true if a multi char commands is active and the timer is running, otherwise False
 */
boolean OpenBCI_32bit_Library::checkMultiCharCmdTimer(void)
{
  if (isMultiCharCmd)
  {
    if (millis() < multiCharCmdTimeout)
      return true;
    else
    { // the timer has timed out - reset the multi char timeout
      endMultiCharCmdTimer();
      printAll("Timeout processing multi byte");
      printAll(" message - please send all");
      printAll(" commands at once as of v2");
      sendEOT();
    }
  }
  return false;
}

/**
 * To be called at some point in every loop function
 */
void OpenBCI_32bit_Library::loop(void)
{
  if (isMultiCharCmd)
  {
    checkMultiCharCmdTimer();
  }
}

/**
 * Gets the active multi char command
 * @param None
 * @returns {char} multiCharCommand
 */
char OpenBCI_32bit_Library::getMultiCharCommand(void)
{
  return multiCharCommand;
}

boolean OpenBCI_32bit_Library::processCharWifi(char character)
{
  commandFromSPI = true;
  boolean retVal = processChar(character);
  commandFromSPI = false;
  return retVal;
}

/**
 * Used to turn on or off the accel, will change the current packet type!
 * @param yes {boolean} - True if you want to use it
 */
void OpenBCI_32bit_Library::useAccel(boolean yes)
{
  curAccelMode = yes ? ACCEL_MODE_ON : ACCEL_MODE_OFF;
  setCurPacketType();
}

/**
 * Used to turn on or off time syncing/stamping, will change the current packet type!
 * @param yes {boolean} - True if you want to use it
 */
void OpenBCI_32bit_Library::useTimeStamp(boolean yes)
{
  curTimeSyncMode = yes ? TIME_SYNC_MODE_ON : TIME_SYNC_MODE_OFF;
  setCurPacketType();
}

/**
* @description Reads a status register to see if there is new accelerometer
*  data. This also takes into account if using accel or not.
* @returns {boolean} `true` if the accelerometer has new data.
*/
boolean OpenBCI_32bit_Library::accelHasNewData(void)
{
  return LIS3DH_DataAvailable();
}

/**
* @description Reads from the accelerometer to get new X, Y, and Z data. Updates
*  the global array `axisData`.
*/
void OpenBCI_32bit_Library::accelUpdateAxisData(void)
{
  LIS3DH_updateAxisData();
}

/**
* @description Reads from the accelerometer to get new X, Y, and Z data.
*/
void OpenBCI_32bit_Library::accelWriteAxisDataSerial(void)
{
  LIS3DH_writeAxisDataSerial();
}

/**
* @description Reads from the accelerometer to get new X, Y, and Z data.
*/
void OpenBCI_32bit_Library::accelWriteAxisDataWifi(void)
{
  LIS3DH_writeAxisDataWifi();
}

/**
* @description: This is a function that is called once and confiures all pins on
*                 the PIC32 uC
* @author: AJ Keller (@pushtheworldllc)
*/
boolean OpenBCI_32bit_Library::boardBegin(void)
{
  // Initalize the serial port baud rate
  // Set serial 0 to true for rx and tx
  beginPinsDefault();
  beginSerial0();

  delay(10);

  // Startup the interrupt
  boardBeginADSInterrupt();

  // Do a soft reset
  boardReset();

  return true;
}

void OpenBCI_32bit_Library::boardBeginADSInterrupt(void)
{
  // Startup for interrupt
  setIntVector(_EXTERNAL_4_VECTOR, ADS_DRDY_Service); // connect interrupt to ISR
  setIntPriority(_EXTERNAL_4_VECTOR, 4, 0);           // set interrupt priority and sub priority
  clearIntFlag(_EXTERNAL_4_IRQ);                      // these two need to be done together
  setIntEnable(_EXTERNAL_4_IRQ);                      // clear any flags before enabing the irq
}

/**
* @description: This is a function that is called once and confiures all pins on
*                 the PIC32 uC
* @author: AJ Keller (@pushtheworldllc)
*/
boolean OpenBCI_32bit_Library::boardBeginDebug(void)
{
  boardBeginDebug(iSerial1.baudRate);
}

/**
* @description: This is a function that is called once and confiures the Pic to run in secondary serial mode
* @param baudRate {int} - The baudRate you want the secondary serial port to run at.
* @author: AJ Keller (@pushtheworldllc)
*/
boolean OpenBCI_32bit_Library::boardBeginDebug(int baudRate)
{
  beginPinsDebug();
  beginSerial0();
  beginSerial1(baudRate);

  curBoardMode = BOARD_MODE_DEBUG;
  curDebugMode = DEBUG_MODE_ON;

  // Startup for interrupt
  boardBeginADSInterrupt();

  // Do a soft reset
  boardReset();

  return true;
}

void OpenBCI_32bit_Library::beginPinsAnalog(void)
{
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  if (!wifi.present)
    pinMode(13, INPUT);
}

void OpenBCI_32bit_Library::beginPinsDebug(void)
{
  pinMode(OPENBCI_PIN_SERIAL1_TX, OUTPUT);
  pinMode(OPENBCI_PIN_SERIAL1_RX, INPUT);
}

void OpenBCI_32bit_Library::beginPinsDefault(void)
{
  pinMode(OPENBCI_PIN_LED, OUTPUT);
  digitalWrite(OPENBCI_PIN_LED, HIGH);
  pinMode(OPENBCI_PIN_PGC, OUTPUT);
}

void OpenBCI_32bit_Library::beginPinsDigital(void)
{
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(17, INPUT);
  if (!wifi.present)
    pinMode(13, INPUT);
  if (!wifi.present)
    pinMode(18, INPUT);
}

/**
 * Used to start Serial0
 */
void OpenBCI_32bit_Library::beginSerial0(void)
{
  beginSerial0(OPENBCI_BAUD_RATE);
}

/**
 * Used to start Serial0
 */
void OpenBCI_32bit_Library::beginSerial0(uint32_t baudRate)
{
  // Initalize the serial port baud rate
  if (Serial0)
    Serial0.end();
  Serial0.begin(baudRate);
  iSerial0.tx = true;
  iSerial0.rx = true;
  iSerial0.baudRate = baudRate;
}

/**
* @description: The function the OpenBCI board will call in setup. This sets up the hardware serial port on D11 and D12
* @param `baudRate` {uint32_t} - The baud rate of the new secondary serial port
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::beginSerial1(void)
{
  beginSerial1(OPENBCI_BAUD_RATE);
}

/**
* @description: The function the OpenBCI board will call in setup. This sets up the hardware serial port on D11 and D12
* @param `baudRate` {uint32_t} - The baud rate of the new secondary serial port
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::beginSerial1(uint32_t baudRate)
{
  // Initalize the serial 1 port
  if (Serial1)
    Serial1.end();
  Serial1.begin(baudRate);
  iSerial1.tx = true;
  iSerial1.rx = true;
  iSerial1.baudRate = baudRate;
}

/**
 * Used to safely end a serial0 port
 */
void OpenBCI_32bit_Library::endSerial0(void)
{
  if (Serial0)
    Serial0.end();
  iSerial0.tx = false;
  iSerial0.rx = false;
}

/**
 * Used to safely end a serial1 port
 */
void OpenBCI_32bit_Library::endSerial1(void)
{
  if (Serial1)
    Serial1.end();
  iSerial1.tx = false;
  iSerial1.rx = false;
  iSerial1.baudRate = OPENBCI_BAUD_RATE;
}

/**
* @description: This is a function that can be called multiple times, this is
*                 what we refer to as a `soft reset`. You will hear/see this
*                 many times.
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::boardReset(void)
{
  initialize(); // initalizes accelerometer and on-board ADS and on-daisy ADS if present
  delay(500);
  configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ);
  printlnAll("OpenBCI V3 8-16 channel");
  printAll("On Board ADS1299 Device ID: 0x");
  printlnHex(ADS_getDeviceID(ON_BOARD));
  if (daisyPresent)
  { // library will set this in initialize() if daisy present and functional
    printAll("On Daisy ADS1299 Device ID: 0x");
    printlnHex(ADS_getDeviceID(ON_DAISY));
  }
  printAll("LIS3DH Device ID: 0x");
  printlnHex(LIS3DH_getDeviceID());
  printlnAll("Firmware: v3.1.2");
  sendEOT();
  delay(5);
  wifi.reset();
}

/**
* @description: Simple method to send the EOT over serial...
* @author: AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::sendEOT(void)
{
  printSerial("$$$");
  wifi.sendStringLast();
}

void OpenBCI_32bit_Library::activateAllChannelsToTestCondition(byte testInputCode, byte amplitudeCode, byte freqCode)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  //set the test signal to the desired state
  configureInternalTestSignal(amplitudeCode, freqCode);
  //change input type settings for all channels
  changeInputType(testInputCode);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
  else
  {
    printSuccess();
    printAll("Configured internal");
    printAll(" test signal.");
    sendEOT();
  }
}

void OpenBCI_32bit_Library::processIncomingBoardMode(char c)
{
  if (c == OPENBCI_BOARD_MODE_SET)
  {
    printSuccess();
    printAll(getBoardMode());
    sendEOT();
  }
  else if (isDigit(c))
  {
    uint8_t digit = c - '0';
    if (digit < BOARD_MODE_END_OF_MODES)
    {
      setBoardMode(digit);
      printSuccess();
      printAll(getBoardMode());
      sendEOT();
    }
    else
    {
      printFailure();
      printAll("board mode value");
      printAll(" out of bounds.");
      sendEOT();
    }
  }
  else
  {
    printFailure();
    printAll("invalid board mode value.");
    sendEOT();
  }
  endMultiCharCmdTimer();
}

/**
 * Used to set the board mode of the system.
 * @param newBoardMode The board mode to swtich to
 */
void OpenBCI_32bit_Library::setBoardMode(uint8_t newBoardMode)
{
  if (curBoardMode == (BOARD_MODE)newBoardMode)
    return;
  curBoardMode = (BOARD_MODE)newBoardMode;
  switch (curBoardMode)
  {
  case BOARD_MODE_ANALOG:
    curAccelMode = ACCEL_MODE_OFF;
    beginPinsAnalog();
    break;
  case BOARD_MODE_DIGITAL:
    curAccelMode = ACCEL_MODE_OFF;
    beginPinsDigital();
    break;
  case BOARD_MODE_DEBUG:
    curDebugMode = DEBUG_MODE_ON;
    beginPinsDebug();
    beginSerial1();
    break;
  case BOARD_MODE_DEFAULT:
    curAccelMode = ACCEL_MODE_ON;
    endSerial1();
    beginPinsDefault();
    endSerial0();
    beginSerial0(OPENBCI_BAUD_RATE);
    break;
  case BOARD_MODE_MARKER:
    curAccelMode = ACCEL_MODE_OFF;
    break;
  case BOARD_MODE_BLE:
    endSerial0();
    beginSerial0(OPENBCI_BAUD_RATE_BLE);
  default:
    break;
  }
  delay(10);
  setCurPacketType();
}

void OpenBCI_32bit_Library::setSampleRate(uint8_t newSampleRateCode)
{
  curSampleRate = (SAMPLE_RATE)newSampleRateCode;
  initialize_ads();
}

const char *OpenBCI_32bit_Library::getSampleRate()
{
  switch (curSampleRate)
  {
  case SAMPLE_RATE_16000:
    return "16000";
  case SAMPLE_RATE_8000:
    return "8000";
  case SAMPLE_RATE_4000:
    return "4000";
  case SAMPLE_RATE_2000:
    return "2000";
  case SAMPLE_RATE_1000:
    return "1000";
  case SAMPLE_RATE_500:
    return "500";
  case SAMPLE_RATE_250:
  default:
    return "250";
  }
}

const char *OpenBCI_32bit_Library::getBoardMode(void)
{
  switch (curBoardMode)
  {
  case BOARD_MODE_DEBUG:
    return "debug";
  case BOARD_MODE_ANALOG:
    return "analog";
  case BOARD_MODE_DIGITAL:
    return "digital";
  case BOARD_MODE_MARKER:
    return "marker";
  case BOARD_MODE_BLE:
    return "BLE";
  case BOARD_MODE_DEFAULT:
  default:
    return "default";
  }
}

void OpenBCI_32bit_Library::processIncomingSampleRate(char c)
{
  if (c == OPENBCI_SAMPLE_RATE_SET)
  {
    printSuccess();
    printAll("Sample rate is ");
    printAll(getSampleRate());
    printAll("Hz");
    sendEOT();
  }
  else if (isDigit(c))
  {
    uint8_t digit = c - '0';
    if (digit <= SAMPLE_RATE_250)
    {
      streamSafeSetSampleRate((SAMPLE_RATE)digit);
      if (!streaming)
      {
        printSuccess();
        printAll("Sample rate is ");
        printAll(getSampleRate());
        printAll("Hz");
        sendEOT();
      }
      else if (wifi.present && wifi.tx)
      {
        wifi.sendStringMulti("Success: Sample rate is ");
        delay(1);
        wifi.sendStringMulti(getSampleRate());
        delay(1);
        wifi.sendStringLast("Hz");
      }
    }
    else
    {
      if (!streaming)
      {
        printFailure();
        printAll("sample value out of bounds");
        sendEOT();
      }
      else if (wifi.present && wifi.tx)
      {
        wifi.sendStringMulti("Failure: sample value");
        delay(1);
        wifi.sendStringLast(" out of bounds");
      }
    }
  }
  else
  {
    if (!streaming)
    {
      printFailure();
      printAll("invalid sample value");
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      wifi.sendStringLast("Failure: invalid sample value");
      delay(1);
    }
  }
  endMultiCharCmdTimer();
}

/**
 * @description When a '`x' is found on the serial port it is a signal to insert a marker
 *      of value x into the AUX1 stream (auxData[0]). This function sets the flag to indicate that a new marker
 *      is available. The marker will be inserted during the serial and sd write functions
 * @param character {char} - The character that will be inserted into the data stream
 */
void OpenBCI_32bit_Library::processInsertMarker(char c)
{
  markerValue = c;
  newMarkerReceived = true;
  endMultiCharCmdTimer();
  if (wifi.present && wifi.tx)
  {
    wifi.sendStringLast("Marker recieved");
  }
}

/**
* @description When a 'x' is found on the serial port, we jump to this function
*                  where we continue to read from the serial port and read the
*                  remaining 7 bytes.
*/
void OpenBCI_32bit_Library::processIncomingChannelSettings(char character)
{

  if (character == OPENBCI_CHANNEL_CMD_LATCH && numberOfIncomingSettingsProcessedChannel < OPENBCI_NUMBER_OF_BYTES_SETTINGS_CHANNEL - 1)
  {
    // We failed somehow and should just abort
    numberOfIncomingSettingsProcessedChannel = 0;

    // put flag back down
    endMultiCharCmdTimer();

    if (!streaming)
    {
      printFailure();
      printAll("too few chars");
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      wifi.sendStringLast("Failure: too few chars");
    }

    return;
  }
  switch (numberOfIncomingSettingsProcessedChannel)
  {
  case 1: // channel number
    currentChannelSetting = getChannelCommandForAsciiChar(character);
    break;
  case 2: // POWER_DOWN
    optionalArgBuffer7[0] = getNumberForAsciiChar(character);
    break;
  case 3: // GAIN_SET
    optionalArgBuffer7[1] = getGainForAsciiChar(character);
    break;
  case 4: // INPUT_TYPE_SET
    optionalArgBuffer7[2] = getNumberForAsciiChar(character);
    break;
  case 5: // BIAS_SET
    optionalArgBuffer7[3] = getNumberForAsciiChar(character);
    break;
  case 6: // SRB2_SET
    optionalArgBuffer7[4] = getNumberForAsciiChar(character);

    break;
  case 7: // SRB1_SET
    optionalArgBuffer7[5] = getNumberForAsciiChar(character);
    break;
  case 8: // 'X' latch
    if (character != OPENBCI_CHANNEL_CMD_LATCH)
    {
      if (!streaming)
      {
        printFailure();
        printAll("Err: 9th char not X");
        sendEOT();
      }
      else if (wifi.present && wifi.tx)
      {
        wifi.sendStringLast("Failure: Err: 9th char not X");
      }
      // We failed somehow and should just abort
      numberOfIncomingSettingsProcessedChannel = 0;

      // put flag back down
      endMultiCharCmdTimer();
    }
    break;
  default: // should have exited
    if (!streaming)
    {
      printFailure();
      printAll("Err: too many chars");
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      wifi.sendStringLast("Failure: Err: too many chars");
    }
    // We failed somehow and should just abort
    numberOfIncomingSettingsProcessedChannel = 0;

    // put flag back down
    endMultiCharCmdTimer();
    return;
  }

  // increment the number of bytes processed
  numberOfIncomingSettingsProcessedChannel++;

  if (numberOfIncomingSettingsProcessedChannel == (OPENBCI_NUMBER_OF_BYTES_SETTINGS_CHANNEL))
  {
    // We are done processing channel settings...
    if (!streaming)
    {
      char buf[2];
      printSuccess();
      printAll("Channel set for ");
      printAll(itoa(currentChannelSetting + 1, buf, 10));
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      char buf[3];
      wifi.sendStringMulti("Success: Channel set for ");
      delay(1);
      wifi.sendStringLast(itoa(currentChannelSetting + 1, buf, 10));
    }

    channelSettings[currentChannelSetting][POWER_DOWN] = optionalArgBuffer7[0];
    channelSettings[currentChannelSetting][GAIN_SET] = optionalArgBuffer7[1];
    channelSettings[currentChannelSetting][INPUT_TYPE_SET] = optionalArgBuffer7[2];
    channelSettings[currentChannelSetting][BIAS_SET] = optionalArgBuffer7[3];
    channelSettings[currentChannelSetting][SRB2_SET] = optionalArgBuffer7[4];
    channelSettings[currentChannelSetting][SRB1_SET] = optionalArgBuffer7[5];

    // Set channel settings
    streamSafeChannelSettingsForChannel(currentChannelSetting + 1, channelSettings[currentChannelSetting][POWER_DOWN], channelSettings[currentChannelSetting][GAIN_SET], channelSettings[currentChannelSetting][INPUT_TYPE_SET], channelSettings[currentChannelSetting][BIAS_SET], channelSettings[currentChannelSetting][SRB2_SET], channelSettings[currentChannelSetting][SRB1_SET]);

    // Reset
    numberOfIncomingSettingsProcessedChannel = 0;

    // put flag back down
    endMultiCharCmdTimer();
  }
}

/**
* @description When a 'z' is found on the serial port, we jump to this function
*                  where we continue to read from the serial port and read the
*                  remaining 4 bytes.
* @param `character` - {char} - The character you want to process...
*/
void OpenBCI_32bit_Library::processIncomingLeadOffSettings(char character)
{

  if (character == OPENBCI_CHANNEL_IMPEDANCE_LATCH && numberOfIncomingSettingsProcessedLeadOff < OPENBCI_NUMBER_OF_BYTES_SETTINGS_LEAD_OFF - 1)
  {
    // We failed somehow and should just abort
    // reset numberOfIncomingSettingsProcessedLeadOff
    numberOfIncomingSettingsProcessedLeadOff = 0;

    // put flag back down
    endMultiCharCmdTimer();

    if (!streaming)
    {
      printFailure();
      printAll("too few chars");
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      wifi.sendStringLast("Failure: too few chars");
    }

    return;
  }
  switch (numberOfIncomingSettingsProcessedLeadOff)
  {
  case 1: // channel number
    currentChannelSetting = getChannelCommandForAsciiChar(character);
    break;
  case 2: // pchannel setting
    optionalArgBuffer7[0] = getNumberForAsciiChar(character);
    break;
  case 3: // nchannel setting
    optionalArgBuffer7[1] = getNumberForAsciiChar(character);
    break;
  case 4: // 'Z' latch
    if (character != OPENBCI_CHANNEL_IMPEDANCE_LATCH)
    {
      if (!streaming)
      {
        printFailure();
        printAll("Err: 5th char not Z");
        sendEOT();
      }
      else if (wifi.present && wifi.tx)
      {
        wifi.sendStringLast("Failure: Err: 5th char not Z");
      }
      // We failed somehow and should just abort
      // reset numberOfIncomingSettingsProcessedLeadOff
      numberOfIncomingSettingsProcessedLeadOff = 0;

      // put flag back down
      endMultiCharCmdTimer();
    }
    break;
  default: // should have exited
    if (!streaming)
    {
      printFailure();
      printAll("Err: too many chars");
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      wifi.sendStringLast("Failure: Err: too many chars");
    }
    // We failed somehow and should just abort
    // reset numberOfIncomingSettingsProcessedLeadOff
    numberOfIncomingSettingsProcessedLeadOff = 0;

    // put flag back down
    endMultiCharCmdTimer();
    return;
  }

  // increment the number of bytes processed
  numberOfIncomingSettingsProcessedLeadOff++;

  if (numberOfIncomingSettingsProcessedLeadOff == (OPENBCI_NUMBER_OF_BYTES_SETTINGS_LEAD_OFF))
  {
    // We are done processing lead off settings...

    if (!streaming)
    {
      char buf[3];
      printSuccess();
      printAll("Lead off set for ");
      printAll(itoa(currentChannelSetting + 1, buf, 10));
      sendEOT();
    }
    else if (wifi.present && wifi.tx)
    {
      char buf[3];
      wifi.sendStringMulti("Success: Lead off set for ");
      delay(1);
      wifi.sendStringLast(itoa(currentChannelSetting + 1, buf, 10));
    }

    leadOffSettings[currentChannelSetting][PCHAN] = optionalArgBuffer7[0];
    leadOffSettings[currentChannelSetting][NCHAN] = optionalArgBuffer7[1];

    // Set lead off settings
    streamSafeLeadOffSetForChannel(currentChannelSetting + 1, leadOffSettings[currentChannelSetting][PCHAN], leadOffSettings[currentChannelSetting][NCHAN]);

    // reset numberOfIncomingSettingsProcessedLeadOff
    numberOfIncomingSettingsProcessedLeadOff = 0;

    // put flag back down
    endMultiCharCmdTimer();
  }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<  BOARD WIDE FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void OpenBCI_32bit_Library::initialize()
{
  pinMode(SD_SS, OUTPUT);
  digitalWrite(SD_SS, HIGH); // de-select SDcard if present
  pinMode(BOARD_ADS, OUTPUT);
  digitalWrite(BOARD_ADS, HIGH);
  pinMode(DAISY_ADS, OUTPUT);
  digitalWrite(DAISY_ADS, HIGH);
  pinMode(LIS3DH_SS, OUTPUT);
  digitalWrite(LIS3DH_SS, HIGH);

  spi.begin();
  spi.setSpeed(4000000);      // use 4MHz for ADS and LIS3DH
  spi.setMode(DSPI_MODE0);    // default to SD card mode!
  initialize_ads();           // hard reset ADS, set pin directions
  initialize_accel(SCALE_4G); // set pin directions, G scale, DRDY interrupt, power down
}

// void __USER_ISR ADS_DRDY_Service() {
void __USER_ISR ADS_DRDY_Service()
{
  clearIntFlag(_EXTERNAL_4_IRQ); // clear the irq, or else it will continually interrupt!
  if (bitRead(PORTA, 0) == 0)
  {
    board.channelDataAvailable = true;
  }
}

void OpenBCI_32bit_Library::initializeVariables(void)
{
  // Bools
  channelDataAvailable = false;
  commandFromSPI = false;
  daisyPresent = false;
  endMultiCharCmdTimer(); // this initializes and resets the variables
  streaming = false;
  verbosity = false; // when verbosity is true, there will be Serial feedback

  // Nums
  ringBufBLEHead = 0;
  ringBufBLETail = 0;
  currentChannelSetting = 0;
  lastSampleTime = 0;
  numberOfIncomingSettingsProcessedChannel = 0;
  numberOfIncomingSettingsProcessedLeadOff = 0;
  sampleCounter = 0;
  sampleCounterBLE = 0;
  timeOfLastRead = 0;
  timeOfMultiByteMsgStart = 0;

  // Enums
  curAccelMode = ACCEL_MODE_ON;
  curBoardMode = BOARD_MODE_DEFAULT;
  curDebugMode = DEBUG_MODE_OFF;
  curPacketType = PACKET_TYPE_ACCEL;
  curSampleRate = SAMPLE_RATE_250;
  curTimeSyncMode = TIME_SYNC_MODE_OFF;

  // Structs
  initializeSerialInfo(iSerial0);
  initializeSerialInfo(iSerial1);
  bufferBLEReset();

  // custom: fast-cycle / batched impedance state (VSC_OpenBCI_Custom-Firmware)
  impedanceFastCycleActive = false;
  impedanceDcStatusActive = false;
  impedanceChannelMask = 0;
  impedanceActiveChannel = 0;
  impedanceActiveChannelIdx = 0;
  impedanceDwellSamples = OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_DEFAULT;
  impedanceDwellCounter = 0;
  impedanceMultiCharMode = 0;
  impedanceArgCounter = 0;
  impedancePrevPacketType = PACKET_TYPE_ACCEL;

  // custom: extended-command state (VSC_OpenBCI_Custom-Firmware)
  extArgCounter = 0;
  extTelemetryCadence = 0;
  extEventCodePending = false;
  extEventCode = 0;
  for (byte i = 0; i < OPENBCI_NUMBER_OF_PROFILE_SLOTS; i++) {
    extProfiles[i].valid = false;
    extProfiles[i].mask = 0;
    extProfiles[i].gainReg = ADS_GAIN24;
    extProfiles[i].inputReg = ADSINPUT_NORMAL;
    extProfiles[i].bias = true;
    extProfiles[i].srb2 = true;
  }
}

void OpenBCI_32bit_Library::initializeSerialInfo(SerialInfo si)
{
  setSerialInfo(si, false, false, OPENBCI_BAUD_RATE);
}

void OpenBCI_32bit_Library::setSerialInfo(SerialInfo si, boolean rx, boolean tx, uint32_t baudRate)
{
  si.baudRate = baudRate;
  si.rx = rx;
  si.tx = tx;
}

/**
 * Reset all the ble buffers
 */
void OpenBCI_32bit_Library::bufferBLEReset()
{
  for (uint8_t i = 0; i < BLE_RING_BUFFER_SIZE; i++)
  {
    bufferBLEReset(bufferBLE + i);
  }
}

/**
 * Reset only the given BLE buffer
 * @param ble {BLE} - A BLE struct to be reset
 */
void OpenBCI_32bit_Library::bufferBLEReset(BLE *ble)
{
  ble->bytesFlushed = 0;
  ble->bytesLoaded = 0;
  ble->ready = false;
  ble->flushing = false;
}

void OpenBCI_32bit_Library::printAllRegisters()
{
  if (!isRunning)
  {
    printlnAll();
    printlnAll("Board ADS Registers");
    // printlnAll("");
    printADSregisters(BOARD_ADS);
    if (daisyPresent)
    {
      printlnAll();
      printlnAll("Daisy ADS Registers");
      printADSregisters(DAISY_ADS);
    }
    printlnAll();
    printlnAll("LIS3DH Registers");
    LIS3DH_readAllRegs();
    sendEOT();
  }
}

/**
 * Called from the .ino file as the main sender. Driven by board mode,
 *  sample number, and ultimately the current packer type.
 *
 */
void OpenBCI_32bit_Library::sendChannelData()
{
  sendChannelData(curPacketType);
}

/**
* @description Writes data to wifi and/or serial port.
*
*  Adds stop byte `OPENBCI_EOP_STND_ACCEL`. See `OpenBCI_32bit_Library_Definitions.h`
*/
void OpenBCI_32bit_Library::sendChannelData(PACKET_TYPE packetType)
{
  // custom side channels (VSC_OpenBCI_Custom-Firmware), all gated so default streaming is
  // unchanged. Each rides the RAW_AUX 6-byte aux field with a distinct marker magic.
  if (impedanceFastCycleActive)
  {
    // fast-cycle impedance sweep marker (0x0FC5)
    serviceImpedanceFastCycle();
    packetType = PACKET_TYPE_RAW_AUX;
  }
  else if (extEventCodePending)
  {
    // sample-locked event code (0x0FC7) -- one sample, then clears
    auxData[0] = (short)OPENBCI_EVENT_MARKER_MAGIC;
    auxData[1] = (short)extEventCode;
    auxData[2] = 0;
    extEventCodePending = false;
    packetType = PACKET_TYPE_RAW_AUX;
  }
  else if (extTelemetryCadence > 0 && (sampleCounter % extTelemetryCadence == 0))
  {
    // contact/railing telemetry frame (0x0FC6)
    buildContactTelemetryFrame();
    packetType = PACKET_TYPE_RAW_AUX;
  }

  if (wifi.present && wifi.tx)
  {
    sendChannelDataWifi(packetType, false);
    if (daisyPresent)
      sendChannelDataWifi(packetType, true);
  }
  else
  {
    // Send over bluetooth
    if (curBoardMode == BOARD_MODE_BLE)
    {
      if (sampleCounter % 2 != 0)
      { //CHECK SAMPLE ODD-EVEN AND SEND THE APPROPRIATE ADS DATA
        if (!(bufferBLE + ringBufBLEHead)->flushing)
        {
          for (int i = 0; i < 6; i++)
          {
            // Serial1.printf("\n<- h: %d t: %d i: %d c->bL: %d\n", ringBufBLEHead, ringBufBLETail, i, (bufferBLE + ringBufBLEHead)->bytesLoaded);
            if ((bufferBLE + ringBufBLEHead)->bytesLoaded == 0)
            {
              (bufferBLE + ringBufBLEHead)->sampleNumber = sampleCounterBLE;
            }
            (bufferBLE + ringBufBLEHead)->data[(bufferBLE + ringBufBLEHead)->bytesLoaded++] = meanBoardDataRaw[i];
            if ((bufferBLE + ringBufBLEHead)->bytesLoaded >= BLE_TOTAL_DATA_BYTES)
            {
              // Serial1.println("Moving head");
              (bufferBLE + ringBufBLEHead)->ready = true;
              sampleCounterBLE += 3;
              ringBufBLEHead++;
              if (ringBufBLEHead >= BLE_RING_BUFFER_SIZE)
              {
                ringBufBLEHead = 0;
              }
            }
          }
        }
      }
      sendChannelDataSerialBLE(packetType);
    }
    else
    {
      if (iSerial0.tx || iSerial1.tx)
        sendChannelDataSerial(packetType);
    }
  }

  if (packetType == PACKET_TYPE_ACCEL)
    LIS3DH_zeroAxisData();
  if (packetType == PACKET_TYPE_RAW_AUX || packetType == PACKET_TYPE_RAW_AUX_TIME_SYNC)
    zeroAuxData();

  sampleCounter++;
}

/**
* @description Writes smaller BLE channel data to serial port in the correct stream packet format.
* @param `packetType` {PACKET_TYPE} - The type of packet to send
*  Adds stop byte see `OpenBCI_32bit_Library.h` enum PACKET_TYPE
*/
void OpenBCI_32bit_Library::sendChannelDataSerialBLE(PACKET_TYPE packetType)
{
  static int delayPeriod = 0;
  unsigned long startTime = micros();
  if ((bufferBLE + ringBufBLETail)->ready && (bufferBLE + ringBufBLETail)->bytesFlushed == 0)
  {
    (bufferBLE + ringBufBLETail)->flushing = true;
  }
  if ((bufferBLE + ringBufBLETail)->flushing)
  {
    // Serial1.printf("\n-> t: %d c->bF: %d\n", ringBufBLETail, (bufferBLE + ringBufBLETail)->bytesFlushed);
    for (int i = 0; i < 4; i++)
    {
      switch ((bufferBLE + ringBufBLETail)->bytesFlushed)
      {
      case 0:
        writeSerial(OPENBCI_BOP); // 1 byte - 0x41
        (bufferBLE + ringBufBLETail)->bytesFlushed++;
        delayMicroseconds(delayPeriod);
        break;
      case 1:
        writeSerial((bufferBLE + ringBufBLETail)->sampleNumber); // 1 byte
        (bufferBLE + ringBufBLETail)->bytesFlushed++;
        delayMicroseconds(delayPeriod);
        break;
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
        writeSerial((bufferBLE + ringBufBLETail)->data[(bufferBLE + ringBufBLETail)->bytesFlushed - 2]);
        (bufferBLE + ringBufBLETail)->bytesFlushed++;
        delayMicroseconds(delayPeriod);
        break;
      case 20:
        writeSerial((uint8_t)(PCKT_END | packetType)); // 1 byte
        delayMicroseconds(delayPeriod);
        bufferBLEReset((bufferBLE + ringBufBLETail));
        ringBufBLETail++;
        if (ringBufBLETail >= BLE_RING_BUFFER_SIZE)
        {
          ringBufBLETail = 0;
        }
        return;
      default:
        if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
        {
          Serial1.printf("\nCritical error bF: %d\n", (bufferBLE + ringBufBLETail)->bytesFlushed);
        }
        bufferBLEReset((bufferBLE + ringBufBLETail));
        ringBufBLETail++;
        if (ringBufBLETail >= BLE_RING_BUFFER_SIZE)
        {
          ringBufBLETail = 0;
        }

        break;
      }
    }
  }
}

/**
* @description Writes channel data to serial port in the correct stream packet format.
* @param `packetType` {PACKET_TYPE} - The type of packet to send
*  Adds stop byte see `OpenBCI_32bit_Library.h` enum PACKET_TYPE
*/
void OpenBCI_32bit_Library::sendChannelDataSerial(PACKET_TYPE packetType)
{

  writeSerial(OPENBCI_BOP);   // 1 byte - 0x41
  writeSerial(sampleCounter); // 1 byte
  ADS_writeChannelData();     // 24 bytes

  switch (packetType)
  {
  case PACKET_TYPE_ACCEL:
    accelWriteAxisDataSerial(); // 6 bytes
    break;
  case PACKET_TYPE_ACCEL_TIME_SET:
    sendTimeWithAccelSerial();
    curPacketType = PACKET_TYPE_ACCEL_TIME_SYNC;
    break;
  case PACKET_TYPE_ACCEL_TIME_SYNC:
    sendTimeWithAccelSerial();
    break;
  case PACKET_TYPE_RAW_AUX_TIME_SET:
    sendTimeWithRawAuxSerial();
    curPacketType = PACKET_TYPE_RAW_AUX_TIME_SYNC;
    break;
  case PACKET_TYPE_RAW_AUX_TIME_SYNC:
    sendTimeWithRawAuxSerial();
    break;
  case PACKET_TYPE_RAW_AUX:
  default:
    writeAuxDataSerial(); // 6 bytes
    break;
  }

  writeSerial((uint8_t)(PCKT_END | packetType)); // 1 byte
}

/**
* @description Writes channel data to wifi in the correct stream packet format.
* @param `packetType` {PACKET_TYPE} - The type of packet to send
* @param `daisy` {boolean} - If this packet for the daisy
*  Adds stop byte see `OpenBCI_32bit_Library.h` enum PACKET_TYPE
*/
void OpenBCI_32bit_Library::sendChannelDataWifi(PACKET_TYPE packetType, boolean daisy)
{

  wifi.storeByteBufTx((uint8_t)(PCKT_END | packetType)); // 1 byte
  wifi.storeByteBufTx(sampleCounter);                    // 1 byte
  ADS_writeChannelDataWifi(daisy);                       // 24 bytes

  switch (packetType)
  {
  case PACKET_TYPE_ACCEL:
    accelWriteAxisDataWifi(); // 6 bytes
    break;
  case PACKET_TYPE_ACCEL_TIME_SET:
    sendTimeWithAccelWifi();
    curPacketType = PACKET_TYPE_ACCEL_TIME_SYNC;
    break;
  case PACKET_TYPE_ACCEL_TIME_SYNC:
    sendTimeWithAccelWifi();
    break;
  case PACKET_TYPE_RAW_AUX_TIME_SET:
    sendTimeWithRawAuxWifi();
    curPacketType = PACKET_TYPE_RAW_AUX_TIME_SYNC;
    break;
  case PACKET_TYPE_RAW_AUX_TIME_SYNC:
    sendTimeWithRawAuxWifi();
    break;
  case PACKET_TYPE_RAW_AUX:
  default:
    writeAuxDataWifi(); // 6 bytes
    break;
  }
  wifi.flushBufferTx();
}
/**
* @description Writes channel data and `auxData` array to serial port in
*  the correct stream packet format.
*
*  Adds stop byte `OPENBCI_EOP_STND_RAW_AUX`. See `OpenBCI_32bit_Library_Definitions.h`
*/
void OpenBCI_32bit_Library::sendRawAuxWifi(void)
{
  writeAuxDataWifi(); // 6 bytes
}

/**
* @description Writes channel data, `axisData` array, and 4 byte unsigned time
*  stamp in ms to serial port in the correct stream packet format.
*
*  `axisData` will be split up and sent on the samples with `sampleCounter` of
*   7, 8, and 9 for X, Y, and Z respectively. Driver writers parse accordingly.
*
*  If the global variable `sendTimeSyncUpPacket` is `true` (set by `processChar`
*   getting a time sync set `<` command) then:
*      Adds stop byte `OPENBCI_EOP_ACCEL_TIME_SET` and sets `sendTimeSyncUpPacket`
*      to `false`.
*  Else if `sendTimeSyncUpPacket` is `false` then:
*      Adds stop byte `OPENBCI_EOP_ACCEL_TIME_SYNCED`
*/
void OpenBCI_32bit_Library::sendTimeWithAccelSerial(void)
{
  // send two bytes of either accel data or blank
  switch (sampleCounter % 10)
  {
  case ACCEL_AXIS_X: // 7
    LIS3DH_writeAxisDataForAxisSerial(0);
    break;
  case ACCEL_AXIS_Y: // 8
    LIS3DH_writeAxisDataForAxisSerial(1);
    break;
  case ACCEL_AXIS_Z: // 9
    LIS3DH_writeAxisDataForAxisSerial(2);
    break;
  default:
    writeSerial((byte)0x00); // high byte
    writeSerial((byte)0x00); // low byte
    break;
  }
  writeTimeCurrentSerial(lastSampleTime); // 4 bytes
}

/**
* @description Writes channel data, `axisData` array, and 4 byte unsigned time
*  stamp in ms to serial port in the correct stream packet format.
*
*  `axisData` will be split up and sent on the samples with `sampleCounter` of
*   7, 8, and 9 for X, Y, and Z respectively. Driver writers parse accordingly.
*
*  If the global variable `sendTimeSyncUpPacket` is `true` (set by `processChar`
*   getting a time sync set `<` command) then:
*      Adds stop byte `OPENBCI_EOP_ACCEL_TIME_SET` and sets `sendTimeSyncUpPacket`
*      to `false`.
*  Else if `sendTimeSyncUpPacket` is `false` then:
*      Adds stop byte `OPENBCI_EOP_ACCEL_TIME_SYNCED`
*/
void OpenBCI_32bit_Library::sendTimeWithAccelWifi(void)
{
  // send two bytes of either accel data or blank
  switch (sampleCounter % 10)
  {
  case ACCEL_AXIS_X: // 7
    LIS3DH_writeAxisDataForAxisWifi(0);
    break;
  case ACCEL_AXIS_Y: // 8
    LIS3DH_writeAxisDataForAxisWifi(1);
    break;
  case ACCEL_AXIS_Z: // 9
    LIS3DH_writeAxisDataForAxisWifi(2);
    break;
  default:
    wifi.storeByteBufTx((byte)0x00); // high byte
    wifi.storeByteBufTx((byte)0x00); // low byte
    break;
  }
  writeTimeCurrentWifi(lastSampleTime); // 4 bytes
}

/**
 * Using publically available state variables to drive packet type settings
 */
void OpenBCI_32bit_Library::setCurPacketType(void)
{
  if (curAccelMode == ACCEL_MODE_ON && curTimeSyncMode == TIME_SYNC_MODE_ON)
  {
    curPacketType = PACKET_TYPE_ACCEL_TIME_SET;
  }
  else if (curAccelMode == ACCEL_MODE_OFF && curTimeSyncMode == TIME_SYNC_MODE_ON)
  {
    curPacketType = PACKET_TYPE_RAW_AUX_TIME_SET;
  }
  else if (curAccelMode == ACCEL_MODE_OFF && curTimeSyncMode == TIME_SYNC_MODE_OFF)
  {
    curPacketType = PACKET_TYPE_RAW_AUX;
  }
  else
  { // default accel on mode
    // curAccelMode == ACCEL_MODE_ON && curTimeSyncMode == TIME_SYNC_MODE_OFF
    curPacketType = PACKET_TYPE_ACCEL;
  }
}

/**
* @description Writes channel data, `auxData[0]` 2 bytes, and 4 byte unsigned
*  time stamp in ms to serial port in the correct stream packet format.
*
*  If the global variable `sendTimeSyncUpPacket` is `true` (set by `processChar`
*   getting a time sync set `<` command) then:
*      Adds stop byte `OPENBCI_EOP_RAW_AUX_TIME_SET` and sets `sendTimeSyncUpPacket`
*      to `false`.
*  Else if `sendTimeSyncUpPacket` is `false` then:
*      Adds stop byte `OPENBCI_EOP_RAW_AUX_TIME_SYNCED`
*/
void OpenBCI_32bit_Library::sendTimeWithRawAuxSerial(void)
{
  writeSerial(highByte(auxData[0])); // 2 bytes of aux data
  writeSerial(lowByte(auxData[0]));
  writeTimeCurrentSerial(lastSampleTime); // 4 bytes
}

void OpenBCI_32bit_Library::writeAuxDataSerial(void)
{
  for (int i = 0; i < 3; i++)
  {
    writeSerial((uint8_t)highByte(auxData[i])); // write 16 bit axis data MSB first
    writeSerial((uint8_t)lowByte(auxData[i]));  // axisData is array of type short (16bit)
  }
}

/**
* @description Writes channel data, `auxData[0]` 2 bytes, and 4 byte unsigned
*  time stamp in ms to serial port in the correct stream packet format.
*/
void OpenBCI_32bit_Library::sendTimeWithRawAuxWifi(void)
{
  wifi.storeByteBufTx(highByte(auxData[0])); // 2 bytes of aux data
  wifi.storeByteBufTx(lowByte(auxData[0]));
  writeTimeCurrentWifi(lastSampleTime); // 4 bytes
}

void OpenBCI_32bit_Library::writeAuxDataWifi(void)
{
  for (int i = 0; i < 3; i++)
  {
    wifi.storeByteBufTx((uint8_t)highByte(auxData[i])); // write 16 bit axis data MSB first
    wifi.storeByteBufTx((uint8_t)lowByte(auxData[i]));  // axisData is array of type short (16bit)
  }
}

void OpenBCI_32bit_Library::zeroAuxData(void)
{
  for (int i = 0; i < 3; i++)
  {
    auxData[i] = 0; // reset auxData bytes to 0
  }
}

void OpenBCI_32bit_Library::writeTimeCurrent(void)
{
  uint32_t newTime = millis(); // serialize the number, placing the MSB in lower packets
  for (int j = 3; j >= 0; j--)
  {
    write((uint8_t)(newTime >> (j * 8)));
  }
}

void OpenBCI_32bit_Library::writeTimeCurrentSerial(uint32_t newTime)
{
  // serialize the number, placing the MSB in lower packets
  for (int j = 3; j >= 0; j--)
  {
    writeSerial((uint8_t)(newTime >> (j * 8)));
  }
}

void OpenBCI_32bit_Library::writeTimeCurrentWifi(uint32_t newTime)
{
  // serialize the number, placing the MSB in lower packets
  for (int j = 3; j >= 0; j--)
  {
    wifi.storeByteBufTx((uint8_t)(newTime >> (j * 8)));
  }
}

//SPI communication method
byte OpenBCI_32bit_Library::xfer(byte _data)
{
  byte inByte;
  inByte = spi.transfer(_data);
  return inByte;
}

//SPI chip select method
void OpenBCI_32bit_Library::csLow(int SS)
{ // select an SPI slave to talk to
  switch (SS)
  {
  case BOARD_ADS:
    spi.setMode(DSPI_MODE1);
    spi.setSpeed(4000000);
    digitalWrite(BOARD_ADS, LOW);
    break;
  case LIS3DH_SS:
    spi.setMode(DSPI_MODE3);
    spi.setSpeed(4000000);
    digitalWrite(LIS3DH_SS, LOW);
    break;
  case SD_SS:
    spi.setMode(DSPI_MODE0);
    spi.setSpeed(20000000);
    digitalWrite(SD_SS, LOW);
    break;
  case DAISY_ADS:
    spi.setMode(DSPI_MODE1);
    spi.setSpeed(4000000);
    digitalWrite(DAISY_ADS, LOW);
    break;
  case BOTH_ADS:
    spi.setMode(DSPI_MODE1);
    spi.setSpeed(4000000);
    digitalWrite(BOARD_ADS, LOW);
    digitalWrite(DAISY_ADS, LOW);
    break;
  default:
    break;
  }
}

void OpenBCI_32bit_Library::csHigh(int SS)
{ // deselect SPI slave
  switch (SS)
  {
  case BOARD_ADS:
    digitalWrite(BOARD_ADS, HIGH);
    break;
  case LIS3DH_SS:
    digitalWrite(LIS3DH_SS, HIGH);
    break;
  case SD_SS:
    digitalWrite(SD_SS, HIGH);
    break;
  case DAISY_ADS:
    digitalWrite(DAISY_ADS, HIGH);
    break;
  case BOTH_ADS:
    digitalWrite(BOARD_ADS, HIGH);
    digitalWrite(DAISY_ADS, HIGH);
    break;
  default:
    break;
  }
  spi.setSpeed(20000000);
  spi.setMode(DSPI_MODE0); // DEFAULT TO SD MODE!
}

// <<<<<<<<<<<<<<<<<<<<<<<<<  END OF BOARD WIDE FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// *************************************************************************************
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  ADS1299 FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void OpenBCI_32bit_Library::initialize_ads()
{
  // recommended power up sequence requiers >Tpor (~32mS)
  delay(50);
  pinMode(ADS_RST, OUTPUT);
  digitalWrite(ADS_RST, LOW);  // reset pin connected to both ADS ICs
  delayMicroseconds(4);        // toggle reset pin
  digitalWrite(ADS_RST, HIGH); // this will reset the Daisy if it is present
  delayMicroseconds(20);       // recommended to wait 18 Tclk before using device (~8uS);
  // initalize the  data ready chip select and reset pins:
  pinMode(ADS_DRDY, INPUT); // we get DRDY asertion from the on-board ADS
  delay(40);
  resetADS(BOARD_ADS); // reset the on-board ADS registers, and stop DataContinuousMode
  delay(10);
  WREG(CONFIG1, (ADS1299_CONFIG1_DAISY | curSampleRate), BOARD_ADS); // tell on-board ADS to output its clk, set the data rate to 250SPS
  delay(40);
  resetADS(DAISY_ADS); // software reset daisy module if present
  delay(10);
  daisyPresent = smellDaisy(); // check to see if daisy module is present
  if (!daisyPresent)
  {
    WREG(CONFIG1, (ADS1299_CONFIG1_DAISY_NOT | curSampleRate), BOARD_ADS); // turn off clk output if no daisy present
    numChannels = 8;                                                       // expect up to 8 ADS channels
  }
  else
  {
    numChannels = 16;                                                      // expect up to 16 ADS channels
    WREG(CONFIG1, (ADS1299_CONFIG1_DAISY_NOT | curSampleRate), DAISY_ADS); // tell on-board ADS to output its clk, set the data rate to 250SPS
    delay(40);
  }

  // DEFAULT CHANNEL SETTINGS FOR ADS
  defaultChannelSettings[POWER_DOWN] = NO;                  // on = NO, off = YES
  defaultChannelSettings[GAIN_SET] = ADS_GAIN24;            // Gain setting
  defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL; // input muxer setting
  defaultChannelSettings[BIAS_SET] = YES;                   // add this channel to bias generation
  defaultChannelSettings[SRB2_SET] = YES;                   // connect this P side to SRB2
  defaultChannelSettings[SRB1_SET] = NO;                    // don't use SRB1

  for (int i = 0; i < numChannels; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      channelSettings[i][j] = defaultChannelSettings[j]; // assign default settings
    }
    useInBias[i] = true; // keeping track of Bias Generation
    useSRB2[i] = true;   // keeping track of SRB2 inclusion
  }
  boardUseSRB1 = daisyUseSRB1 = false;

  writeChannelSettings(); // write settings to the on-board and on-daisy ADS if present

  WREG(CONFIG3, 0b11101100, BOTH_ADS);
  delay(1); // enable internal reference drive and etc.
  for (int i = 0; i < numChannels; i++)
  { // turn off the impedance measure signal
    leadOffSettings[i][PCHAN] = OFF;
    leadOffSettings[i][NCHAN] = OFF;
  }
  verbosity = false; // when verbosity is true, there will be Serial feedback
  firstDataPacket = true;

  streaming = false;
}

//////////////////////////////////////////////
///////////// STREAM METHODS /////////////////
//////////////////////////////////////////////

/**
* @description Used to activate a channel, if running must stop and start after...
* @param channelNumber int the channel you want to change
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeChannelActivate(byte channelNumber)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  // Activate the channel
  activateChannel(channelNumber);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to deactivate a channel, if running must stop and start after...
* @param channelNumber int the channel you want to change
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeChannelDeactivate(byte channelNumber)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  // deactivate the channel
  deactivateChannel(channelNumber);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to set lead off for a channel, if running must stop and start after...
* @param `channelNumber` - [byte] - The channel you want to change
* @param `pInput` - [byte] - Apply signal to P input, either ON (1) or OFF (0)
* @param `nInput` - [byte] - Apply signal to N input, either ON (1) or OFF (0)
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeLeadOffSetForChannel(byte channelNumber, byte pInput, byte nInput)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  changeChannelLeadOffDetect(channelNumber);

  // leadOffSetForChannel(channelNumber, pInput, nInput);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to set lead off for a channel, if running must stop and start after...
* @param see `.channelSettingsSetForChannel()` for parameters
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeChannelSettingsForChannel(byte channelNumber, byte powerDown, byte gain, byte inputType, byte bias, byte srb2, byte srb1)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  writeChannelSettings(channelNumber);

  // channelSettingsSetForChannel(channelNumber, powerDown, gain, inputType, bias, srb2, srb1);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to report (Serial0.print) the default channel settings
*                  if running must stop and start after...
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeReportAllChannelDefaults(void)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  reportDefaultChannelSettings();

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to set all channels on Board (and Daisy) to the default
*                  channel settings if running must stop and start after...
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeSetAllChannelsToDefault(void)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  setChannelsToDefault();

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
* @description Used to set the sample rate
* @param sr {SAMPLE_RATE} - The sample rate to set to.
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::streamSafeSetSampleRate(SAMPLE_RATE sr)
{
  boolean wasStreaming = streaming;

  // Stop streaming if you are currently streaming
  if (streaming)
  {
    streamStop();
  }

  setSampleRate(sr);

  // Restart stream if need be
  if (wasStreaming)
  {
    streamStart();
  }
}

/**
 * Return an array of gains in coded ADS form i.e. 0-6 where 6 is x24 and so on.
 * @return  [description]
 */
uint8_t *OpenBCI_32bit_Library::getGains(void)
{
  uint8_t gains[numChannels];
  for (uint8_t i = 0; i < numChannels; i++)
  {
    gains[i] = channelSettings[i][GAIN_SET];
  }
  return gains;
}

/**
* @description Call this to start the streaming data from the ADS1299
* @returns boolean if able to start streaming
*/
void OpenBCI_32bit_Library::streamStart()
{ // needs daisy functionality

  if (wifi.present && wifi.tx)
  {
    // TODO: Remove this debug line
    wifi.sendGains(numChannels, getGains());
  }
  streaming = true;
  startADS();
  if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
  {
    Serial1.println("ADS Started");
  }
}

/**
* @description Call this to stop streaming from the ADS1299
* @returns boolean if able to stop streaming
*/
void OpenBCI_32bit_Library::streamStop()
{
  streaming = false;
  stopADS();
  if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
  {
    Serial1.println("ADS Stopped");
  }
}

//////////////////////////////////////////////
////////////// DAISY METHODS /////////////////
//////////////////////////////////////////////
boolean OpenBCI_32bit_Library::smellDaisy(void)
{ // check if daisy present
  boolean isDaisy = false;
  byte setting = RREG(ID_REG, DAISY_ADS); // try to read the daisy product ID
  if (verbosity)
  {
    printAll("Daisy ID 0x");
    printlnHex(setting);
    sendEOT();
  }
  if (setting == ADS_ID)
  {
    isDaisy = true;
  } // should read as 0x3E
  return isDaisy;
}

void OpenBCI_32bit_Library::removeDaisy(void)
{
  if (daisyPresent)
  {
    // Daisy removed
    SDATAC(DAISY_ADS);
    RESET(DAISY_ADS);
    STANDBY(DAISY_ADS);
    daisyPresent = false;
    if (!isRunning)
    {
      printAll("daisy removed");
      sendEOT();
    }
  }
  else
  {
    if (!isRunning)
    {
      printAll("no daisy to remove!");
      sendEOT();
    }
  }
}

void OpenBCI_32bit_Library::attachDaisy(void)
{
  WREG(CONFIG1, (ADS1299_CONFIG1_DAISY | curSampleRate), BOARD_ADS); // tell on-board ADS to output the clk, set the data rate to 250SPS
  delay(40);
  resetADS(DAISY_ADS); // software reset daisy module if present
  delay(10);
  daisyPresent = smellDaisy();
  if (!daisyPresent)
  {
    WREG(CONFIG1, (ADS1299_CONFIG1_DAISY_NOT | curSampleRate), BOARD_ADS); // turn off clk output if no daisy present
    numChannels = 8;                                                       // expect up to 8 ADS channels
    if (!isRunning)
    {
      printAll("no daisy to attach!");
    }
  }
  else
  {
    numChannels = 16; // expect up to 16 ADS channels
    if (!isRunning)
    {
      printAll("daisy attached");
    }
  }
}

//reset all the ADS1299's settings. Stops all data acquisition
void OpenBCI_32bit_Library::resetADS(int targetSS)
{
  int startChan, stopChan;
  if (targetSS == BOARD_ADS)
  {
    startChan = 1;
    stopChan = 8;
  }
  if (targetSS == DAISY_ADS)
  {
    startChan = 9;
    stopChan = 16;
  }
  RESET(targetSS);  // send RESET command to default all registers
  SDATAC(targetSS); // exit Read Data Continuous mode to communicate with ADS
  delay(100);
  // turn off all channels
  for (int chan = startChan; chan <= stopChan; chan++)
  {
    deactivateChannel(chan);
  }
}

void OpenBCI_32bit_Library::setChannelsToDefault(void)
{
  for (int i = 0; i < numChannels; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      channelSettings[i][j] = defaultChannelSettings[j];
    }
    useInBias[i] = true; // keeping track of Bias Generation
    useSRB2[i] = true;   // keeping track of SRB2 inclusion
  }
  boardUseSRB1 = daisyUseSRB1 = false;

  writeChannelSettings(); // write settings to on-board ADS

  for (int i = 0; i < numChannels; i++)
  { // turn off the impedance measure signal
    leadOffSettings[i][PCHAN] = OFF;
    leadOffSettings[i][NCHAN] = OFF;
  }
  changeChannelLeadOffDetect(); // write settings to all ADS

  WREG(MISC1, 0x00, BOARD_ADS); // open SRB1 switch on-board
  if (daisyPresent)
  {
    WREG(MISC1, 0x00, DAISY_ADS);
  } // open SRB1 switch on-daisy
}

// void OpenBCI_32bit_Library::setChannelsToDefault(void){

//     // Reset the global channel settings array to default
//     resetChannelSettingsArrayToDefault(channelSettings);
//     // Write channel settings to board (and daisy) ADS
//     channelSettingsArraySetForAll();

//     // Reset the global lead off settings array to default
//     resetLeadOffArrayToDefault(leadOffSettings);
//     // Write lead off settings to board (and daisy) ADS
//     leadOffSetForAllChannels();

//     WREG(MISC1,0x00,BOARD_ADS);  // open SRB1 switch on-board
//     if(daisyPresent){  // open SRB1 switch on-daisy
//         WREG(MISC1,0x00,DAISY_ADS);
//     }
// }

/**
* @description Writes the default channel settings over the serial port
*/
void OpenBCI_32bit_Library::reportDefaultChannelSettings(void)
{
  char buf[7];
  buf[0] = getDefaultChannelSettingForSettingAscii(POWER_DOWN);     // on = NO, off = YES
  buf[1] = getDefaultChannelSettingForSettingAscii(GAIN_SET);       // Gain setting
  buf[2] = getDefaultChannelSettingForSettingAscii(INPUT_TYPE_SET); // input muxer setting
  buf[3] = getDefaultChannelSettingForSettingAscii(BIAS_SET);       // add this channel to bias generation
  buf[4] = getDefaultChannelSettingForSettingAscii(SRB2_SET);       // connect this P side to SRB2
  buf[5] = getDefaultChannelSettingForSettingAscii(SRB1_SET);       // don't use SRB1
  printAll((const char *)buf);
  sendEOT();
}

/**
* @description Set all channels using global channelSettings array
* @author AJ Keller (@pushtheworldllc)
*/
// void OpenBCI_32bit_Library::channelSettingsArraySetForAll(void) {
//     byte channelNumberUpperLimit;

//     // The upper limit of the channels, either 8 or 16
//     channelNumberUpperLimit = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;

//     // Loop through all channels
//     for (byte i = 1; i <= channelNumberUpperLimit; i++) {
//         // Set for this channel
//         channelSettingsSetForChannel(i, channelSettings[i][POWER_DOWN], channelSettings[i][GAIN_SET], channelSettings[i][INPUT_TYPE_SET], channelSettings[i][BIAS_SET], channelSettings[i][SRB2_SET], channelSettings[i][SRB1_SET]);
//     }
// }

/**
* @description Set channel using global channelSettings array for channelNumber
* @param `channelNumber` - [byte] - 1-16 channel number
* @author AJ Keller (@pushtheworldllc)
*/
// void OpenBCI_32bit_Library::channelSettingsArraySetForChannel(byte channelNumber) {
//     // contstrain the channel number to 0-15
//     char index = getConstrainedChannelNumber(channelNumber);

//     // Set for this channel
//     channelSettingsSetForChannel(channelNumber, channelSettings[index][POWER_DOWN], channelSettings[index][GAIN_SET], channelSettings[index][INPUT_TYPE_SET], channelSettings[index][BIAS_SET], channelSettings[index][SRB2_SET], channelSettings[index][SRB1_SET]);
// }

/**
* @description To add a usability abstraction layer above channel setting commands. Due to the
*          extensive and highly specific nature of the channel setting command chain.
* @param `channelNumber` - [byte] (1-16) for index, so convert channel to array prior
* @param `powerDown` - [byte] - YES (1) or NO (0)
*          Powers channel down
* @param `gain` - [byte] - Sets the gain for the channel
*          ADS_GAIN01 (0b00000000)	// 0x00
*          ADS_GAIN02 (0b00010000)	// 0x10
*          ADS_GAIN04 (0b00100000)	// 0x20
*          ADS_GAIN06 (0b00110000)	// 0x30
*          ADS_GAIN08 (0b01000000)	// 0x40
*          ADS_GAIN12 (0b01010000)	// 0x50
*          ADS_GAIN24 (0b01100000)	// 0x60
* @param `inputType` - [byte] - Selects the ADC channel input source, either:
*          ADSINPUT_NORMAL     (0b00000000)
*          ADSINPUT_SHORTED    (0b00000001)
*          ADSINPUT_BIAS_MEAS  (0b00000010)
*          ADSINPUT_MVDD       (0b00000011)
*          ADSINPUT_TEMP       (0b00000100)
*          ADSINPUT_TESTSIG    (0b00000101)
*          ADSINPUT_BIAS_DRP   (0b00000110)
*          ADSINPUT_BIAL_DRN   (0b00000111)
* @param `bias` - [byte] (YES (1) -> Include in bias (default), NO (0) -> remove from bias)
*          selects to include the channel input in bias generation
* @param `srb2` - [byte] (YES (1) -> Connect this input to SRB2 (default),
*                     NO (0) -> Disconnect this input from SRB2)
*          Select to connect (YES) this channel's P input to the SRB2 pin. This closes
*              a switch between P input and SRB2 for the given channel, and allows the
*              P input to also remain connected to the ADC.
* @param `srb1` - [byte] (YES (1) -> connect all N inputs to SRB1,
*                     NO (0) -> Disconnect all N inputs from SRB1 (default))
*          Select to connect (YES) all channels' N inputs to SRB1. This effects all pins,
*              and disconnects all N inputs from the ADC.
* @author AJ Keller (@pushtheworldllc)
*/
// void OpenBCI_32bit_Library::channelSettingsSetForChannel(byte channelNumber, byte powerDown, byte gain, byte inputType, byte bias, byte srb2, byte srb1) {
//     byte setting, targetSS;

//     // contstrain the channel number to 0-15
//     char index = getConstrainedChannelNumber(channelNumber);

//     // Get the slave select pin for this channel
//     targetSS = getTargetSSForConstrainedChannelNumber(index);

//     if (sniffMode && Serial1) {
//         if (targetSS == BOARD_ADS) {
//             Serial1.print("Set channel "); Serial1.print(channelNumber); Serial1.println(" settings");
//         }
//     }

//     // first, disable any data collection
//     SDATAC(targetSS); delay(1);      // exit Read Data Continuous mode to communicate with ADS

//     setting = 0x00;

//     // Set the power down bit
//     if(powerDown == YES) {
//         setting |= 0x80;
//     }

//     // Set the gain bits
//     setting |= gain;

//     // Set input type bits
//     setting |= inputType;

//     if(srb2 == YES){
//         setting |= 0x08; // close this SRB2 switch
//         useSRB2[index] = true;  // keep track of SRB2 usage
//     }else{
//         useSRB2[index] = false;
//     }

//     byte channelNumberRegister = 0x00;

//     // Since we are addressing 8 bit registers, we need to subtract 8 from the
//     //  channelNumber if we are addressing the Daisy ADS
//     if (targetSS == DAISY_ADS) {
//         channelNumberRegister = index - OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
//     } else {
//         channelNumberRegister = index;
//     }
//     WREG(CH1SET+channelNumberRegister, setting, targetSS);  // write this channel's register settings

//     // add or remove from inclusion in BIAS generation
//     setting = RREG(BIAS_SENSP,targetSS);       //get the current P bias settings
//     if(bias == YES){
//         useInBias[index] = true;
//         bitSet(setting,channelNumberRegister);    //set this channel's bit to add it to the bias generation
//     }else{
//         useInBias[index] = false;
//         bitClear(setting,channelNumberRegister);  // clear this channel's bit to remove from bias generation
//     }
//     WREG(BIAS_SENSP,setting,targetSS); delay(1); //send the modified byte back to the ADS

//     setting = RREG(BIAS_SENSN,targetSS);       //get the current N bias settings
//     if(bias == YES){
//         bitSet(setting,channelNumberRegister);    //set this channel's bit to add it to the bias generation
//     }else{
//         bitClear(setting,channelNumberRegister);  // clear this channel's bit to remove from bias generation
//     }
//     WREG(BIAS_SENSN,setting,targetSS); delay(1); //send the modified byte back to the ADS

//     byte startChan = targetSS == BOARD_ADS ? 0 : OPENBCI_CHANNEL_MAX_NUMBER_8 - 1;
//     byte endChan = targetSS == BOARD_ADS ? OPENBCI_CHANNEL_MAX_NUMBER_8 : OPENBCI_CHANNEL_MAX_NUMBER_16 - 1;
//     // if SRB1 is closed or open for one channel, it will be the same for all channels
//     if(srb1 == YES){
//         for(int i=startChan; i<endChan; i++){
//             channelSettings[i][SRB1_SET] = YES;
//         }
//         if(targetSS == BOARD_ADS) boardUseSRB1 = true;
//         if(targetSS == DAISY_ADS) daisyUseSRB1 = true;
//         setting = 0x20;     // close SRB1 swtich
//     }
//     if(srb1 == NO){
//         for(int i=startChan; i<endChan; i++){
//             channelSettings[i][SRB1_SET] = NO;
//         }
//         if(targetSS == BOARD_ADS) boardUseSRB1 = false;
//         if(targetSS == DAISY_ADS) daisyUseSRB1 = false;
//         setting = 0x00;     // open SRB1 switch
//     }
//     WREG(MISC1,setting,targetSS);
// }

// write settings for ALL 8 channels for a given ADS board
// channel settings: powerDown, gain, inputType, SRB2, SRB1
void OpenBCI_32bit_Library::writeChannelSettings()
{
  boolean use_SRB1 = false;
  byte setting, startChan, endChan, targetSS;

  for (int b = 0; b < 2; b++)
  {
    if (b == 0)
    {
      targetSS = BOARD_ADS;
      startChan = 0;
      endChan = 8;
    }
    if (b == 1)
    {
      if (!daisyPresent)
      {
        return;
      }
      targetSS = DAISY_ADS;
      startChan = 8;
      endChan = 16;
    }

    SDATAC(targetSS);
    delay(1); // exit Read Data Continuous mode to communicate with ADS

    for (byte i = startChan; i < endChan; i++)
    { // write 8 channel settings
      setting = 0x00;
      if (channelSettings[i][POWER_DOWN] == YES)
      {
        setting |= 0x80;
      }
      setting |= channelSettings[i][GAIN_SET];       // gain
      setting |= channelSettings[i][INPUT_TYPE_SET]; // input code
      if (channelSettings[i][SRB2_SET] == YES)
      {
        setting |= 0x08;   // close this SRB2 switch
        useSRB2[i] = true; // remember SRB2 state for this channel
      }
      else
      {
        useSRB2[i] = false; // rememver SRB2 state for this channel
      }
      WREG(CH1SET + (i - startChan), setting, targetSS); // write this channel's register settings

      // add or remove this channel from inclusion in BIAS generation
      setting = RREG(BIAS_SENSP, targetSS); //get the current P bias settings
      if (channelSettings[i][BIAS_SET] == YES)
      {
        bitSet(setting, i - startChan);
        useInBias[i] = true; //add this channel to the bias generation
      }
      else
      {
        bitClear(setting, i - startChan);
        useInBias[i] = false; //remove this channel from bias generation
      }
      WREG(BIAS_SENSP, setting, targetSS);
      delay(1); //send the modified byte back to the ADS

      setting = RREG(BIAS_SENSN, targetSS); //get the current N bias settings
      if (channelSettings[i][BIAS_SET] == YES)
      {
        bitSet(setting, i - startChan); //set this channel's bit to add it to the bias generation
      }
      else
      {
        bitClear(setting, i - startChan); // clear this channel's bit to remove from bias generation
      }
      WREG(BIAS_SENSN, setting, targetSS);
      delay(1); //send the modified byte back to the ADS

      if (channelSettings[i][SRB1_SET] == YES)
      {
        use_SRB1 = true; // if any of the channel setting closes SRB1, it is closed for all
      }
    } // end of CHnSET and BIAS settings
  }   // end of board select loop
  if (use_SRB1)
  {
    for (int i = startChan; i < endChan; i++)
    {
      channelSettings[i][SRB1_SET] = YES;
    }
    WREG(MISC1, 0x20, targetSS); // close SRB1 swtich
    if (targetSS == BOARD_ADS)
    {
      boardUseSRB1 = true;
    }
    if (targetSS == DAISY_ADS)
    {
      daisyUseSRB1 = true;
    }
  }
  else
  {
    for (int i = startChan; i < endChan; i++)
    {
      channelSettings[i][SRB1_SET] = NO;
    }
    WREG(MISC1, 0x00, targetSS); // open SRB1 switch
    if (targetSS == BOARD_ADS)
    {
      boardUseSRB1 = false;
    }
    if (targetSS == DAISY_ADS)
    {
      daisyUseSRB1 = false;
    }
  }
}

// write settings for a SPECIFIC channel on a given ADS board
void OpenBCI_32bit_Library::writeChannelSettings(byte N)
{

  byte setting, startChan, endChan, targetSS;
  if (N < 9)
  { // channels 1-8 on board
    targetSS = BOARD_ADS;
    startChan = 0;
    endChan = 8;
  }
  else
  { // channels 9-16 on daisy module
    if (!daisyPresent)
    {
      return;
    }
    targetSS = DAISY_ADS;
    startChan = 8;
    endChan = 16;
  }
  // function accepts channel 1-16, must be 0 indexed to work with array
  N = constrain(N - 1, startChan, endChan - 1); //subtracts 1 so that we're counting from 0, not 1
  // first, disable any data collection
  SDATAC(targetSS);
  delay(1); // exit Read Data Continuous mode to communicate with ADS

  setting = 0x00;
  if (channelSettings[N][POWER_DOWN] == YES)
    setting |= 0x80;
  setting |= channelSettings[N][GAIN_SET];       // gain
  setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
  if (channelSettings[N][SRB2_SET] == YES)
  {
    setting |= 0x08;   // close this SRB2 switch
    useSRB2[N] = true; // keep track of SRB2 usage
  }
  else
  {
    useSRB2[N] = false;
  }
  WREG(CH1SET + (N - startChan), setting, targetSS); // write this channel's register settings

  // add or remove from inclusion in BIAS generation
  setting = RREG(BIAS_SENSP, targetSS); //get the current P bias settings
  if (channelSettings[N][BIAS_SET] == YES)
  {
    useInBias[N] = true;
    bitSet(setting, N - startChan); //set this channel's bit to add it to the bias generation
  }
  else
  {
    useInBias[N] = false;
    bitClear(setting, N - startChan); // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSP, setting, targetSS);
  delay(1);                             //send the modified byte back to the ADS
  setting = RREG(BIAS_SENSN, targetSS); //get the current N bias settings
  if (channelSettings[N][BIAS_SET] == YES)
  {
    bitSet(setting, N - startChan); //set this channel's bit to add it to the bias generation
  }
  else
  {
    bitClear(setting, N - startChan); // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSN, setting, targetSS);
  delay(1); //send the modified byte back to the ADS

  // if SRB1 is closed or open for one channel, it will be the same for all channels
  if (channelSettings[N][SRB1_SET] == YES)
  {
    for (int i = startChan; i < endChan; i++)
    {
      channelSettings[i][SRB1_SET] = YES;
    }
    if (targetSS == BOARD_ADS)
      boardUseSRB1 = true;
    if (targetSS == DAISY_ADS)
      daisyUseSRB1 = true;
    setting = 0x20; // close SRB1 swtich
  }
  if (channelSettings[N][SRB1_SET] == NO)
  {
    for (int i = startChan; i < endChan; i++)
    {
      channelSettings[i][SRB1_SET] = NO;
    }
    if (targetSS == BOARD_ADS)
      boardUseSRB1 = false;
    if (targetSS == DAISY_ADS)
      daisyUseSRB1 = false;
    setting = 0x00; // open SRB1 switch
  }
  WREG(MISC1, setting, targetSS);
}

//  deactivate the given channel.
void OpenBCI_32bit_Library::deactivateChannel(byte N)
{
  byte setting, startChan, endChan, targetSS;
  if (N < 9)
  {
    targetSS = BOARD_ADS;
    startChan = 0;
    endChan = 8;
  }
  else
  {
    if (!daisyPresent)
    {
      return;
    }
    targetSS = DAISY_ADS;
    startChan = 8;
    endChan = 16;
  }
  SDATAC(targetSS);
  delay(1);                                     // exit Read Data Continuous mode to communicate with ADS
  N = constrain(N - 1, startChan, endChan - 1); //subtracts 1 so that we're counting from 0, not 1

  setting = RREG(CH1SET + (N - startChan), targetSS);
  delay(1);             // get the current channel settings
  bitSet(setting, 7);   // set bit7 to shut down channel
  bitClear(setting, 3); // clear bit3 to disclude from SRB2 if used
  WREG(CH1SET + (N - startChan), setting, targetSS);
  delay(1); // write the new value to disable the channel

  //remove the channel from the bias generation...
  setting = RREG(BIAS_SENSP, targetSS);
  delay(1);                         //get the current bias settings
  bitClear(setting, N - startChan); //clear this channel's bit to remove from bias generation
  WREG(BIAS_SENSP, setting, targetSS);
  delay(1); //send the modified byte back to the ADS

  setting = RREG(BIAS_SENSN, targetSS);
  delay(1);                         //get the current bias settings
  bitClear(setting, N - startChan); //clear this channel's bit to remove from bias generation
  WREG(BIAS_SENSN, setting, targetSS);
  delay(1); //send the modified byte back to the ADS

  leadOffSettings[N][0] = leadOffSettings[N][1] = NO;
  changeChannelLeadOffDetect(N + 1);
}

void OpenBCI_32bit_Library::activateChannel(byte N)
{
  byte setting, startChan, endChan, targetSS;
  if (N < 9)
  {
    targetSS = BOARD_ADS;
    startChan = 0;
    endChan = 8;
  }
  else
  {
    if (!daisyPresent)
    {
      return;
    }
    targetSS = DAISY_ADS;
    startChan = 8;
    endChan = 16;
  }

  N = constrain(N - 1, startChan, endChan - 1); // 0-7 or 8-15

  SDATAC(targetSS); // exit Read Data Continuous mode to communicate with ADS
  setting = 0x00;
  //  channelSettings[N][POWER_DOWN] = NO; // keep track of channel on/off in this array  REMOVE?
  setting |= channelSettings[N][GAIN_SET];       // gain
  setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
  if (useSRB2[N] == true)
  {
    channelSettings[N][SRB2_SET] = YES;
  }
  else
  {
    channelSettings[N][SRB2_SET] = NO;
  }
  if (channelSettings[N][SRB2_SET] == YES)
  {
    bitSet(setting, 3);
  } // close this SRB2 switch
  WREG(CH1SET + (N - startChan), setting, targetSS);
  // add or remove from inclusion in BIAS generation
  if (useInBias[N])
  {
    channelSettings[N][BIAS_SET] = YES;
  }
  else
  {
    channelSettings[N][BIAS_SET] = NO;
  }
  setting = RREG(BIAS_SENSP, targetSS); //get the current P bias settings
  if (channelSettings[N][BIAS_SET] == YES)
  {
    bitSet(setting, N - startChan); //set this channel's bit to add it to the bias generation
    useInBias[N] = true;
  }
  else
  {
    bitClear(setting, N - startChan); // clear this channel's bit to remove from bias generation
    useInBias[N] = false;
  }
  WREG(BIAS_SENSP, setting, targetSS);
  delay(1);                             //send the modified byte back to the ADS
  setting = RREG(BIAS_SENSN, targetSS); //get the current N bias settings
  if (channelSettings[N][BIAS_SET] == YES)
  {
    bitSet(setting, N - startChan); //set this channel's bit to add it to the bias generation
  }
  else
  {
    bitClear(setting, N - startChan); // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSN, setting, targetSS);
  delay(1); //send the modified byte back to the ADS

  setting = 0x00;
  if (targetSS == BOARD_ADS && boardUseSRB1 == true)
    setting = 0x20;
  if (targetSS == DAISY_ADS && daisyUseSRB1 == true)
    setting = 0x20;
  WREG(MISC1, setting, targetSS); // close all SRB1 swtiches
}

// change the lead off detect settings for all channels
void OpenBCI_32bit_Library::changeChannelLeadOffDetect()
{
  byte setting, startChan, endChan, targetSS;

  for (int b = 0; b < 2; b++)
  {
    if (b == 0)
    {
      targetSS = BOARD_ADS;
      startChan = 0;
      endChan = 8;
    }
    if (b == 1)
    {
      if (!daisyPresent)
      {
        return;
      }
      targetSS = DAISY_ADS;
      startChan = 8;
      endChan = 16;
    }

    SDATAC(targetSS);
    delay(1); // exit Read Data Continuous mode to communicate with ADS
    byte P_setting = RREG(LOFF_SENSP, targetSS);
    byte N_setting = RREG(LOFF_SENSN, targetSS);

    for (int i = startChan; i < endChan; i++)
    {
      if (leadOffSettings[i][PCHAN] == ON)
      {
        bitSet(P_setting, i - startChan);
      }
      else
      {
        bitClear(P_setting, i - startChan);
      }
      if (leadOffSettings[i][NCHAN] == ON)
      {
        bitSet(N_setting, i - startChan);
      }
      else
      {
        bitClear(N_setting, i - startChan);
      }
      WREG(LOFF_SENSP, P_setting, targetSS);
      WREG(LOFF_SENSN, N_setting, targetSS);
    }
  }
}

// change the lead off detect settings for specified channel
void OpenBCI_32bit_Library::changeChannelLeadOffDetect(byte N)
{
  byte setting, targetSS, startChan, endChan;

  if (N < 9)
  {
    targetSS = BOARD_ADS;
    startChan = 0;
    endChan = 8;
  }
  else
  {
    if (!daisyPresent)
    {
      return;
    }
    targetSS = DAISY_ADS;
    startChan = 8;
    endChan = 16;
  }

  N = constrain(N - 1, startChan, endChan - 1);
  SDATAC(targetSS);
  delay(1); // exit Read Data Continuous mode to communicate with ADS
  byte P_setting = RREG(LOFF_SENSP, targetSS);
  byte N_setting = RREG(LOFF_SENSN, targetSS);

  if (leadOffSettings[N][PCHAN] == ON)
  {
    bitSet(P_setting, N - startChan);
  }
  else
  {
    bitClear(P_setting, N - startChan);
  }
  if (leadOffSettings[N][NCHAN] == ON)
  {
    bitSet(N_setting, N - startChan);
  }
  else
  {
    bitClear(N_setting, N - startChan);
  }
  WREG(LOFF_SENSP, P_setting, targetSS);
  WREG(LOFF_SENSN, N_setting, targetSS);
}

void OpenBCI_32bit_Library::configureLeadOffDetection(byte amplitudeCode, byte freqCode)
{
  amplitudeCode &= 0b00001100; //only these two bits should be used
  freqCode &= 0b00000011;      //only these two bits should be used

  byte setting, targetSS;
  for (int i = 0; i < 2; i++)
  {
    if (i == 0)
    {
      targetSS = BOARD_ADS;
    }
    if (i == 1)
    {
      if (!daisyPresent)
      {
        return;
      }
      targetSS = DAISY_ADS;
    }
    setting = RREG(LOFF, targetSS); //get the current bias settings
    //reconfigure the byte to get what we want
    setting &= 0b11110000;    //clear out the last four bits
    setting |= amplitudeCode; //set the amplitude
    setting |= freqCode;      //set the frequency
    //send the config byte back to the hardware
    WREG(LOFF, setting, targetSS);
    delay(1); //send the modified byte back to the ADS
  }
}

// //  deactivate the given channel.
// void OpenBCI_32bit_Library::deactivateChannel(byte N)
// {
//     byte setting, startChan, endChan, targetSS;
//     if(N < 9){
//         targetSS = BOARD_ADS; startChan = 0; endChan = 8;
//     }else{
//         if(!daisyPresent) { return; }
//         targetSS = DAISY_ADS; startChan = 8; endChan = 16;
//     }
//     SDATAC(targetSS); delay(1);      // exit Read Data Continuous mode to communicate with ADS
//     N = constrain(N-1,startChan,endChan-1);  //subtracts 1 so that we're counting from 0, not 1
//
//     setting = RREG(CH1SET+(N-startChan),targetSS); delay(1); // get the current channel settings
//     bitSet(setting,7);     // set bit7 to shut down channel
//     bitClear(setting,3);   // clear bit3 to disclude from SRB2 if used
//     WREG(CH1SET+(N-startChan),setting,targetSS); delay(1);     // write the new value to disable the channel
//
//     //remove the channel from the bias generation...
//     setting = RREG(BIAS_SENSP,targetSS); delay(1); //get the current bias settings
//     bitClear(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
//     WREG(BIAS_SENSP,setting,targetSS); delay(1);   //send the modified byte back to the ADS
//
//     setting = RREG(BIAS_SENSN,targetSS); delay(1); //get the current bias settings
//     bitClear(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
//     WREG(BIAS_SENSN,setting,targetSS); delay(1);   //send the modified byte back to the ADS
//
//     leadOffSettings[N][PCHAN] = leadOffSettings[N][NCHAN] = NO;
//     leadOffSetForChannel(N+1, NO, NO);
// }

// void OpenBCI_32bit_Library::activateChannel(byte N)
// {
//     byte setting, startChan, endChan, targetSS;
//     if(N < 9){
//         targetSS = BOARD_ADS; startChan = 0; endChan = 8;
//     }else{
//         if(!daisyPresent) { return; }
//         targetSS = DAISY_ADS; startChan = 8; endChan = 16;
//     }

//     N = constrain(N-1,startChan,endChan-1);  // 0-7 or 8-15

//     SDATAC(targetSS);  // exit Read Data Continuous mode to communicate with ADS
//     setting = 0x00;
//     //  channelSettings[N][POWER_DOWN] = NO; // keep track of channel on/off in this array  REMOVE?
//     setting |= channelSettings[N][GAIN_SET]; // gain
//     setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
//     if(useSRB2[N] == true){channelSettings[N][SRB2_SET] = YES;}else{channelSettings[N][SRB2_SET] = NO;}
//     if(channelSettings[N][SRB2_SET] == YES) {bitSet(setting,3);} // close this SRB2 switch
//     WREG(CH1SET+(N-startChan),setting,targetSS);
//     // add or remove from inclusion in BIAS generation
//     if(useInBias[N]){channelSettings[N][BIAS_SET] = YES;}else{channelSettings[N][BIAS_SET] = NO;}
//     setting = RREG(BIAS_SENSP,targetSS);       //get the current P bias settings
//     if(channelSettings[N][BIAS_SET] == YES){
//         bitSet(setting,N-startChan);    //set this channel's bit to add it to the bias generation
//         useInBias[N] = true;
//     }else{
//         bitClear(setting,N-startChan);  // clear this channel's bit to remove from bias generation
//         useInBias[N] = false;
//     }
//     WREG(BIAS_SENSP,setting,targetSS); delay(1); //send the modified byte back to the ADS
//     setting = RREG(BIAS_SENSN,targetSS);       //get the current N bias settings
//     if(channelSettings[N][BIAS_SET] == YES){
//         bitSet(setting,N-startChan);    //set this channel's bit to add it to the bias generation
//     }else{
//         bitClear(setting,N-startChan);  // clear this channel's bit to remove from bias generation
//     }
//     WREG(BIAS_SENSN,setting,targetSS); delay(1); //send the modified byte back to the ADS

//     setting = 0x00;
//     if(targetSS == BOARD_ADS && boardUseSRB1 == true) setting = 0x20;
//     if(targetSS == DAISY_ADS && daisyUseSRB1 == true) setting = 0x20;
//     WREG(MISC1,setting,targetSS);     // close all SRB1 swtiches
// }

//////////////////////////////////////////////
///////////// LEAD OFF METHODS ///////////////
//////////////////////////////////////////////

/**
* @description Runs through the `leadOffSettings` global array to set/change
*                  the lead off signals for all inputs of all channels.
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::leadOffSetForAllChannels(void) {
    byte channelNumberUpperLimit;

    // The upper limit of the channels, either 8 or 16
    channelNumberUpperLimit = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;

    // Loop through all channels
    for (int i = 1; i <= channelNumberUpperLimit; i++) {
        leadOffSetForChannel((byte)i,leadOffSettings[i-1][PCHAN],leadOffSettings[i-1][NCHAN]);
    }
}

/**
* @description Used to set lead off for a channel
* @param `channelNumber` - [byte] - The channel you want to change
* @param `pInput` - [byte] - Apply signal to P input, either ON (1) or OFF (0)
* @param `nInput` - [byte] - Apply signal to N input, either ON (1) or OFF (0)
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::leadOffSetForChannel(byte channelNumber, byte pInput, byte nInput) {

    // contstrain the channel number to 0-15
    channelNumber = getConstrainedChannelNumber(channelNumber);

    // Get the slave select pin for this channel
    byte targetSS = getTargetSSForConstrainedChannelNumber(channelNumber);

    // exit Read Data Continuous mode to communicate with ADS
    SDATAC(targetSS);
    delay(1);

    // Read P register
    byte P_setting = RREG(LOFF_SENSP,targetSS);

    // Read N register
    byte N_setting = RREG(LOFF_SENSN,targetSS);

    // Since we are addressing 8 bit registers, we need to subtract 8 from the
    //  channelNumber if we are addressing the Daisy ADS
    if (targetSS == DAISY_ADS) {
        channelNumber -= OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
    }

    // If pInput is ON then we want to set, otherwise we want to clear
    if (pInput == ON) {
        bitSet(P_setting, channelNumber);
    } else {
        bitClear(P_setting, channelNumber);
    }
    // Write to the P register
    WREG(LOFF_SENSP,P_setting,targetSS);

    // If nInput is ON then we want to set, otherwise we want to clear
    if (nInput == ON) {
        bitSet(N_setting, channelNumber);
    } else {
        bitClear(N_setting, channelNumber);
    }
    // Write to the N register
    WREG(LOFF_SENSN,N_setting,targetSS);
}

/**
* @description This sets the LOFF register on the Board ADS and the Daisy ADS
* @param `amplitudeCode` - [byte] - The amplitude of the of impedance signal.
*                 See `.setleadOffForSS()` for complete description
* @param `freqCode` - [byte] - The frequency of the impedance signal can be either.
*                 See `.setleadOffForSS()` for complete description
* @author AJ Keller (@pushtheworldllc)
*/
void OpenBCI_32bit_Library::leadOffConfigureSignalForAll(byte amplitudeCode, byte freqCode)
{
    // Set the lead off detection for the on board ADS
    leadOffConfigureSignalForTargetSS(BOARD_ADS, amplitudeCode, freqCode);

    // if the daisy board is present, set that register as well
    if (daisyPresent) {
        leadOffConfigureSignalForTargetSS(DAISY_ADS, amplitudeCode, freqCode);
    }
}

/**
* @description This sets the LOFF (lead off) register for the given ADS with slave
*                  select
* @param `targetSS` - [byte] - The Slave Select pin.
* @param `amplitudeCode` - [byte] - The amplitude of the of impedance signal.
*          LOFF_MAG_6NA        (0b00000000)
*          LOFF_MAG_24NA       (0b00000100)
*          LOFF_MAG_6UA        (0b00001000)
*          LOFF_MAG_24UA       (0b00001100)
* @param `freqCode` - [byte] - The frequency of the impedance signal can be either.
*          LOFF_FREQ_DC        (0b00000000)
*          LOFF_FREQ_7p8HZ     (0b00000001)
*          LOFF_FREQ_31p2HZ    (0b00000010)
*          LOFF_FREQ_FS_4      (0b00000011)
* @author Joel/Leif/Conor (@OpenBCI) Summer 2014
*/
void OpenBCI_32bit_Library::leadOffConfigureSignalForTargetSS(byte targetSS, byte amplitudeCode, byte freqCode) {
    byte setting;

    amplitudeCode &= 0b00001100;  //only these two bits should be used
    freqCode &= 0b00000011;  //only these two bits should be used

    setting = RREG(LOFF,targetSS); //get the current bias settings
    //reconfigure the byte to get what we want
    setting &= 0b11110000;  //clear out the last four bits
    setting |= amplitudeCode;  //set the amplitude
    setting |= freqCode;    //set the frequency
    //send the config byte back to the hardware
    WREG(LOFF,setting,targetSS); delay(1);  //send the modified byte back to the ADS
}

//////////////////////////////////////////////////////////////////////////////
//  CUSTOM FAST-CYCLE / BATCHED IMPEDANCE (VSC_OpenBCI_Custom-Firmware)
//  Additive to the stock single-channel 'z CH P N Z' path, which is untouched.
//////////////////////////////////////////////////////////////////////////////

/**
* @description Write a single channel's P/N lead-off bits WITHOUT tearing down the
*   stream. Unlike streamSafeLeadOffSetForChannel (which calls streamStop/streamStart,
*   issuing STOP/START and toggling `streaming`), this only briefly leaves Read Data
*   Continuous mode to issue the register write, then re-enters it. The ADS1299 keeps
*   converting (START stays asserted); at most one DRDY sample is skipped. This is the
*   primitive that makes the firmware fast-cycle impedance sweep low-latency.
* @param channelNumber 1-based channel
* @param pInput ON/OFF for the P input lead-off
* @param nInput ON/OFF for the N input lead-off
*/
void OpenBCI_32bit_Library::leadOffInlineSetChannel(byte channelNumber, byte pInput, byte nInput) {
    // Keep the bookkeeping array in sync so other paths (e.g. changeChannelLeadOffDetect) agree
    byte idx = getConstrainedChannelNumber(channelNumber); // 0-15
    leadOffSettings[idx][PCHAN] = pInput;
    leadOffSettings[idx][NCHAN] = nInput;

    byte targetSS = getTargetSSForConstrainedChannelNumber(idx);
    byte bit = idx;
    if (targetSS == DAISY_ADS) {
        bit -= OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
    }

    SDATAC(targetSS);                       // leave continuous-read so we can write registers
    byte P_setting = RREG(LOFF_SENSP, targetSS);
    byte N_setting = RREG(LOFF_SENSN, targetSS);
    if (pInput == ON) { bitSet(P_setting, bit); } else { bitClear(P_setting, bit); }
    if (nInput == ON) { bitSet(N_setting, bit); } else { bitClear(N_setting, bit); }
    WREG(LOFF_SENSP, P_setting, targetSS);
    WREG(LOFF_SENSN, N_setting, targetSS);
    RDATAC(targetSS);                       // resume continuous-read; conversions never stopped
}

/**
* @description Set the P-input lead-off on a 16-channel bitmask in one SDATAC/RDATAC
*   pass (no per-channel stream stop/restart). Backs the host-side "all-at-once"
*   (parallel) impedance style on stock-compatible packets. N inputs are left OFF,
*   matching the typical single-channel 'z CH 1 0 Z' drive.
* @param pMask bit i (i in 0..15) drives channel i+1 P input when set
*/
void OpenBCI_32bit_Library::leadOffBatchSetMask(uint16_t pMask) {
    byte upper = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;

    // Update the bookkeeping array first
    for (byte i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
        boolean on = (i < upper) && ((pMask >> i) & 0x0001);
        leadOffSettings[i][PCHAN] = on ? ON : OFF;
        leadOffSettings[i][NCHAN] = OFF;
    }

    // Make sure the global lead-off drive is the standard 6 nA / 31.2 Hz
    configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ);

    // Compose and write LOFF_SENSP/SENSN for each ADS in a single continuous-read exit.
    for (byte b = 0; b < 2; b++) {
        byte targetSS, startChan;
        if (b == 0) { targetSS = BOARD_ADS; startChan = 0; }
        else        { if (!daisyPresent) break; targetSS = DAISY_ADS; startChan = 8; }

        SDATAC(targetSS);
        byte P_setting = 0x00;
        byte N_setting = 0x00; // N stays off for impedance drive
        for (byte i = 0; i < 8; i++) {
            if (leadOffSettings[startChan + i][PCHAN] == ON) { bitSet(P_setting, i); }
        }
        WREG(LOFF_SENSP, P_setting, targetSS);
        WREG(LOFF_SENSN, N_setting, targetSS);
        RDATAC(targetSS);
    }
}

/**
* @description Arm the on-chip fast-cycle sequencer. From here on, serviceImpedanceFastCycle()
*   (called once per emitted sample) drives one channel's P lead-off at a time, dwells
*   `dwellSamples` samples, tags every emitted sample with the active channel in the aux
*   field, then advances. Clean single-channel return path (no shared-bias cross-talk) at a
*   fraction of the host-driven sweep latency. Requires an active stream.
* @param pMask channels to sweep (bit i -> channel i+1)
* @param dwellSamples samples to hold each channel (1..255)
*/
void OpenBCI_32bit_Library::impedanceFastCycleStart(uint16_t pMask, byte dwellSamples) {
    if (dwellSamples < OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_MIN) dwellSamples = OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_DEFAULT;

    byte upper = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
    // Constrain the mask to existing channels
    if (upper < 16) { pMask &= (uint16_t)((1UL << upper) - 1); }

    impedanceChannelMask = pMask;
    impedanceDwellSamples = dwellSamples;
    impedanceDwellCounter = 0;
    impedanceActiveChannelIdx = 0;
    impedanceActiveChannel = 0;

    // Clear all lead-off first so we start from a known state
    for (byte i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
        leadOffSettings[i][PCHAN] = OFF;
        leadOffSettings[i][NCHAN] = OFF;
    }

    // Standard 6 nA / 31.2 Hz global drive
    configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ);

    if (pMask == 0) {            // nothing to sweep
        impedanceFastCycleActive = false;
        return;
    }

    // Remember the packet type so we can restore it when the sweep ends, then force
    // RAW_AUX so the per-channel marker rides the aux field.
    impedancePrevPacketType = curPacketType;

    // Find the first channel in the mask and drive it
    while (impedanceActiveChannelIdx < OPENBCI_NUMBER_OF_CHANNELS_DAISY &&
           !((impedanceChannelMask >> impedanceActiveChannelIdx) & 0x0001)) {
        impedanceActiveChannelIdx++;
    }
    impedanceActiveChannel = impedanceActiveChannelIdx + 1;
    leadOffInlineSetChannel(impedanceActiveChannel, ON, OFF);

    impedanceFastCycleActive = true;
}

/**
* @description Immediately stop a fast-cycle impedance sweep: clear all lead-off, restore the
*   previous packet type, and lower the gate. Safe to call when not sweeping.
*/
void OpenBCI_32bit_Library::impedanceFastCycleStop(void) {
    if (impedanceActiveChannel >= 1 && impedanceActiveChannel <= OPENBCI_NUMBER_OF_CHANNELS_DAISY) {
        leadOffInlineSetChannel(impedanceActiveChannel, OFF, OFF);
    }
    // Make sure every channel's lead-off is cleared in the registers
    for (byte i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
        leadOffSettings[i][PCHAN] = OFF;
        leadOffSettings[i][NCHAN] = OFF;
    }
    if (streaming) {
        changeChannelLeadOffDetect();   // SDATAC + WREG cleared bits for all channels
        RDATAC(BOTH_ADS);               // resume continuous read (changeChannelLeadOffDetect leaves SDATAC)
    }
    impedanceFastCycleActive = false;
    impedanceActiveChannel = 0;
    impedanceActiveChannelIdx = 0;
    impedanceDwellCounter = 0;
    curPacketType = impedancePrevPacketType;
}

/**
* @description Stepped once per emitted sample (from sendChannelData) while a fast-cycle
*   sweep is active. Tags the current sample with the active channel + flags in the aux
*   field, advances the dwell counter, and switches channels (inline LOFF write) when the
*   dwell elapses. Finishes the sweep after the last masked channel.
*/
void OpenBCI_32bit_Library::serviceImpedanceFastCycle(void) {
    if (!impedanceFastCycleActive) return;

    short flags = 0;
    if (impedanceDwellCounter < OPENBCI_IMPEDANCE_FAST_CYCLE_SETTLE) {
        flags |= OPENBCI_IMPEDANCE_FLAG_SETTLING;
    }

    // Optionally surface the DC lead-off comparator bit for the active channel
    if (impedanceDcStatusActive && impedanceActiveChannel >= 1) {
        byte idx = impedanceActiveChannel - 1;
        byte targetSS = (idx < 8) ? BOARD_ADS : DAISY_ADS;
        byte bit = (idx < 8) ? idx : (idx - 8);
        SDATAC(targetSS);
        byte statp = RREG(LOFF_STATP, targetSS);
        byte statn = RREG(LOFF_STATN, targetSS);
        RDATAC(targetSS);
        if ((statp >> bit) & 0x01) flags |= OPENBCI_IMPEDANCE_FLAG_DC_STATP;
        if ((statn >> bit) & 0x01) flags |= OPENBCI_IMPEDANCE_FLAG_DC_STATN;
    }

    boolean lastSampleOfChannel = (impedanceDwellCounter + 1 >= impedanceDwellSamples);
    if (lastSampleOfChannel) flags |= OPENBCI_IMPEDANCE_FLAG_LAST_SAMPLE;

    // Tag this sample (rides the RAW_AUX 6-byte aux field)
    auxData[0] = (short)OPENBCI_IMPEDANCE_MARKER_MAGIC;
    auxData[1] = (short)impedanceActiveChannel;
    auxData[2] = flags;

    impedanceDwellCounter++;

    if (impedanceDwellCounter >= impedanceDwellSamples) {
        // Done with this channel: turn it off, advance to the next masked channel
        leadOffInlineSetChannel(impedanceActiveChannel, OFF, OFF);
        impedanceDwellCounter = 0;
        impedanceActiveChannelIdx++;
        while (impedanceActiveChannelIdx < OPENBCI_NUMBER_OF_CHANNELS_DAISY &&
               !((impedanceChannelMask >> impedanceActiveChannelIdx) & 0x0001)) {
            impedanceActiveChannelIdx++;
        }
        if (impedanceActiveChannelIdx >= OPENBCI_NUMBER_OF_CHANNELS_DAISY) {
            // Sweep complete. Keep this final sample tagged with the channel that just
            // finished (auxData[1] already set above, with the LAST flag) and OR in DONE,
            // so the host does NOT lose the last usable sample of the final channel.
            impedanceFastCycleActive = false;
            curPacketType = impedancePrevPacketType;
            auxData[2] = (short)(auxData[2] | OPENBCI_IMPEDANCE_FLAG_DONE);
            impedanceActiveChannel = 0;
        } else {
            impedanceActiveChannel = impedanceActiveChannelIdx + 1;
            leadOffInlineSetChannel(impedanceActiveChannel, ON, OFF);
        }
    }
}

/**
* @description Multi-char parser for the custom impedance commands (g/k/m). Mirrors the
*   processIncomingLeadOffSettings state-machine style: it is fed one char per call while
*   the multi-char timer is live, accumulates ASCII-hex args, and acts on the 'Z' latch.
*   The initiator char is stored in impedanceMultiCharMode by processChar.
*/
void OpenBCI_32bit_Library::processIncomingImpedanceSettings(char character) {
    // Determine how many arg bytes (incl. the 'Z' latch) this initiator expects
    byte expected;
    switch (impedanceMultiCharMode) {
        case OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET: expected = OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_ALL_AT_ONCE; break;
        case OPENBCI_IMPEDANCE_FAST_CYCLE_SET:  expected = OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_FAST_CYCLE;  break;
        case OPENBCI_IMPEDANCE_DC_STATUS_SET:   expected = OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_DC_STATUS;   break;
        default:
            impedanceArgCounter = 0;
            endMultiCharCmdTimer();
            return;
    }

    // The final byte must be the 'Z' latch
    if (impedanceArgCounter == (byte)(expected - 1)) {
        if (character != OPENBCI_CHANNEL_IMPEDANCE_LATCH) {
            if (!streaming) { printFailure(); printAll("Err: impedance latch not Z"); sendEOT(); }
            impedanceArgCounter = 0;
            endMultiCharCmdTimer();
            return;
        }
        // Latched: act on the accumulated args
        if (impedanceMultiCharMode == OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET) {
            uint16_t mask = (uint16_t)(
                (getHexForAsciiChar(impedanceArgBuffer[0]) << 12) |
                (getHexForAsciiChar(impedanceArgBuffer[1]) << 8)  |
                (getHexForAsciiChar(impedanceArgBuffer[2]) << 4)  |
                (getHexForAsciiChar(impedanceArgBuffer[3])));
            leadOffBatchSetMask(mask);
            if (!streaming) { printSuccess(); printAll("All-at-once lead off set"); sendEOT(); }
        } else if (impedanceMultiCharMode == OPENBCI_IMPEDANCE_FAST_CYCLE_SET) {
            uint16_t mask = (uint16_t)(
                (getHexForAsciiChar(impedanceArgBuffer[0]) << 12) |
                (getHexForAsciiChar(impedanceArgBuffer[1]) << 8)  |
                (getHexForAsciiChar(impedanceArgBuffer[2]) << 4)  |
                (getHexForAsciiChar(impedanceArgBuffer[3])));
            byte dwell = (byte)((getHexForAsciiChar(impedanceArgBuffer[4]) << 4) |
                                 getHexForAsciiChar(impedanceArgBuffer[5]));
            impedanceFastCycleStart(mask, dwell);
            if (!streaming) { printSuccess(); printAll("Fast-cycle armed (start stream)"); sendEOT(); }
        } else if (impedanceMultiCharMode == OPENBCI_IMPEDANCE_DC_STATUS_SET) {
            impedanceDcStatusActive = (impedanceArgBuffer[0] == '1');
            if (!streaming) { printSuccess(); printAll(impedanceDcStatusActive ? "DC status ON" : "DC status OFF"); sendEOT(); }
        }
        impedanceArgCounter = 0;
        impedanceMultiCharMode = 0;
        endMultiCharCmdTimer();
        return;
    }

    // Otherwise accumulate the arg byte
    if (impedanceArgCounter < sizeof(impedanceArgBuffer)) {
        impedanceArgBuffer[impedanceArgCounter] = character;
    }
    impedanceArgCounter++;
}

//////////////////////////////////////////////////////////////////////////////
//  CUSTOM EXTENDED COMMANDS (VSC_OpenBCI_Custom-Firmware)
//  One top-level initiator 'n' + a context-local sub-op + ASCII-hex args + 'Z'.
//  Features: per-paradigm acquisition profiles, atomic channel power mask,
//  continuous contact/railing telemetry, sample-locked event codes.
//  All additive and gated -- default streaming behaviour is unchanged.
//////////////////////////////////////////////////////////////////////////////

/**
* @description Store an acquisition profile in a RAM slot. The host (desktop app) is the
*   source of truth for profiles; this just lets it pre-load a slot for instant atomic apply.
*/
void OpenBCI_32bit_Library::extProfileDefine(byte slot, uint16_t mask, byte gainReg, byte inputReg, boolean bias, boolean srb2) {
    if (slot >= OPENBCI_NUMBER_OF_PROFILE_SLOTS) return;
    extProfiles[slot].valid   = true;
    extProfiles[slot].mask    = mask;
    extProfiles[slot].gainReg = gainReg;
    extProfiles[slot].inputReg= inputReg;
    extProfiles[slot].bias    = bias;
    extProfiles[slot].srb2    = srb2;
}

/**
* @description Atomically apply a stored profile to all 16 channels in one writeChannelSettings()
*   pass (no per-channel round-trips), then resume continuous read if streaming. Active channels
*   get the profile's gain/input/bias/srb2; inactive channels are powered down and removed from bias.
* @returns false if the slot is empty.
*/
boolean OpenBCI_32bit_Library::extProfileApply(byte slot) {
    if (slot >= OPENBCI_NUMBER_OF_PROFILE_SLOTS || !extProfiles[slot].valid) return false;
    AcqProfile *p = &extProfiles[slot];
    byte upper = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;

    for (byte i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
        boolean active = (i < upper) && ((p->mask >> i) & 0x0001);
        if (active) {
            channelSettings[i][POWER_DOWN]    = NO;
            channelSettings[i][GAIN_SET]      = p->gainReg;
            channelSettings[i][INPUT_TYPE_SET]= p->inputReg;
            channelSettings[i][BIAS_SET]      = p->bias ? YES : NO;
            channelSettings[i][SRB2_SET]      = p->srb2 ? YES : NO;
            channelSettings[i][SRB1_SET]      = NO;
        } else {
            channelSettings[i][POWER_DOWN]    = YES;
            channelSettings[i][BIAS_SET]      = NO;
            channelSettings[i][SRB2_SET]      = NO;
            channelSettings[i][SRB1_SET]      = NO;
        }
    }
    writeChannelSettings();               // one SDATAC pass per ADS; leaves SDATAC
    if (streaming) { RDATAC(BOTH_ADS); }  // resume continuous read without a full stop/start
    return true;
}

/**
* @description Atomically set which channels are powered/active from a 16-bit mask, keeping each
*   channel's existing gain/input/srb2. In-mask channels are powered on and biased; out-of-mask
*   channels are powered down and removed from bias. One writeChannelSettings() pass.
*/
void OpenBCI_32bit_Library::extChannelPowerMask(uint16_t mask) {
    byte upper = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
    for (byte i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++) {
        boolean active = (i < upper) && ((mask >> i) & 0x0001);
        channelSettings[i][POWER_DOWN] = active ? NO : YES;
        channelSettings[i][BIAS_SET]   = active ? YES : NO;
    }
    writeChannelSettings();
    if (streaming) { RDATAC(BOTH_ADS); }
}

/**
* @description Fill auxData with a contact/railing telemetry frame (marker 0x0FC6). Costs no extra
*   SPI: lead-off status comes from the ADS status word already read every sample into boardStat/
*   daisyStat (format: 1100 + LOFF_STATP[7:0] + LOFF_STATN[7:0] + GPIO[7:4]); railing comes from the
*   sign-extended channel ints already in boardChannelDataInt/daisyChannelDataInt.
*   auxData[1] = railing bitmask (bit i -> channel i+1 at/over full scale)
*   auxData[2] = contact bitmask  (bit i -> channel i+1 lead-off detected, i.e. P or N comparator set)
*/
void OpenBCI_32bit_Library::buildContactTelemetryFrame(void) {
    uint16_t railing = 0;
    uint16_t contact = 0;

    for (byte i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++) {
        long v = boardChannelDataInt[i];
        if (v >= OPENBCI_RAILING_THRESHOLD || v <= -OPENBCI_RAILING_THRESHOLD) railing |= (1 << i);
    }
    byte pBoard = (boardStat >> 12) & 0xFF;  // LOFF_STATP[7:0]
    byte nBoard = (boardStat >> 4) & 0xFF;   // LOFF_STATN[7:0]
    contact |= (uint16_t)(pBoard | nBoard);

    if (daisyPresent) {
        for (byte i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++) {
            long v = daisyChannelDataInt[i];
            if (v >= OPENBCI_RAILING_THRESHOLD || v <= -OPENBCI_RAILING_THRESHOLD) railing |= (1 << (8 + i));
        }
        byte pD = (daisyStat >> 12) & 0xFF;
        byte nD = (daisyStat >> 4) & 0xFF;
        contact |= (uint16_t)((pD | nD) << 8);
    }

    auxData[0] = (short)OPENBCI_TELEMETRY_MARKER_MAGIC;
    auxData[1] = (short)railing;
    auxData[2] = (short)contact;
}

/**
* @description Multi-char parser for the extended 'n' commands. Fed one char per call while the
*   multi-char timer is live. The first char is the sub-op, which fixes the expected length; the
*   final char must be the 'Z' latch. Mirrors processIncomingImpedanceSettings' style.
*/
void OpenBCI_32bit_Library::processIncomingExtendedSettings(char character) {
    // Accumulate first
    if (extArgCounter < (byte)sizeof(extArgBuffer)) {
        extArgBuffer[extArgCounter] = character;
    }
    extArgCounter++;

    // Need the sub-op (first char) before we know the length
    if (extArgCounter < 1) return;

    byte expected;
    switch (extArgBuffer[0]) {
        case OPENBCI_EXT_SUBOP_PROFILE_DEFINE: expected = OPENBCI_EXT_LEN_PROFILE_DEFINE; break;
        case OPENBCI_EXT_SUBOP_PROFILE_APPLY:  expected = OPENBCI_EXT_LEN_PROFILE_APPLY;  break;
        case OPENBCI_EXT_SUBOP_CHANNEL_MASK:   expected = OPENBCI_EXT_LEN_CHANNEL_MASK;   break;
        case OPENBCI_EXT_SUBOP_TELEMETRY:      expected = OPENBCI_EXT_LEN_TELEMETRY;      break;
        case OPENBCI_EXT_SUBOP_EVENT_CODE:     expected = OPENBCI_EXT_LEN_EVENT_CODE;     break;
        default:
            // Unknown sub-op: abort cleanly
            extArgCounter = 0;
            endMultiCharCmdTimer();
            return;
    }

    if (extArgCounter < expected) return;  // still collecting

    // Full payload in hand; the final byte must be the 'Z' latch
    if (extArgBuffer[expected - 1] != OPENBCI_CHANNEL_IMPEDANCE_LATCH) {
        if (!streaming) { printFailure(); printAll("Err: ext latch not Z"); sendEOT(); }
        extArgCounter = 0;
        endMultiCharCmdTimer();
        return;
    }

    // Act per sub-op. Args begin at extArgBuffer[1].
    switch (extArgBuffer[0]) {
        case OPENBCI_EXT_SUBOP_PROFILE_DEFINE: {
            byte slot = getHexForAsciiChar(extArgBuffer[1]);
            uint16_t mask = (uint16_t)((getHexForAsciiChar(extArgBuffer[2]) << 12) |
                                       (getHexForAsciiChar(extArgBuffer[3]) << 8)  |
                                       (getHexForAsciiChar(extArgBuffer[4]) << 4)  |
                                       (getHexForAsciiChar(extArgBuffer[5])));
            byte gainReg  = (byte)(getHexForAsciiChar(extArgBuffer[6]) << 4); // code<<4 -> ADS_GAIN*
            byte inputReg = getHexForAsciiChar(extArgBuffer[7]);
            byte flags    = getHexForAsciiChar(extArgBuffer[8]);
            extProfileDefine(slot, mask, gainReg, inputReg, (flags & 0x01), ((flags >> 1) & 0x01));
            if (!streaming) { printSuccess(); printAll("Profile defined"); sendEOT(); }
            break;
        }
        case OPENBCI_EXT_SUBOP_PROFILE_APPLY: {
            byte slot = getHexForAsciiChar(extArgBuffer[1]);
            boolean ok = extProfileApply(slot);
            if (!streaming) { if (ok) { printSuccess(); printAll("Profile applied"); } else { printFailure(); printAll("Empty profile slot"); } sendEOT(); }
            break;
        }
        case OPENBCI_EXT_SUBOP_CHANNEL_MASK: {
            uint16_t mask = (uint16_t)((getHexForAsciiChar(extArgBuffer[1]) << 12) |
                                       (getHexForAsciiChar(extArgBuffer[2]) << 8)  |
                                       (getHexForAsciiChar(extArgBuffer[3]) << 4)  |
                                       (getHexForAsciiChar(extArgBuffer[4])));
            extChannelPowerMask(mask);
            if (!streaming) { printSuccess(); printAll("Channel mask set"); sendEOT(); }
            break;
        }
        case OPENBCI_EXT_SUBOP_TELEMETRY: {
            extTelemetryCadence = (byte)((getHexForAsciiChar(extArgBuffer[1]) << 4) |
                                          getHexForAsciiChar(extArgBuffer[2]));
            if (!streaming) { printSuccess(); printAll(extTelemetryCadence ? "Telemetry on" : "Telemetry off"); sendEOT(); }
            break;
        }
        case OPENBCI_EXT_SUBOP_EVENT_CODE: {
            extEventCode = (uint16_t)((getHexForAsciiChar(extArgBuffer[1]) << 12) |
                                      (getHexForAsciiChar(extArgBuffer[2]) << 8)  |
                                      (getHexForAsciiChar(extArgBuffer[3]) << 4)  |
                                      (getHexForAsciiChar(extArgBuffer[4])));
            extEventCodePending = true;  // emitted on the next sample
            if (!streaming) { printSuccess(); printAll("Event queued"); sendEOT(); }
            break;
        }
    }

    extArgCounter = 0;
    endMultiCharCmdTimer();
}

//Configure the test signals that can be inernally generated by the ADS1299
void OpenBCI_32bit_Library::configureInternalTestSignal(byte amplitudeCode, byte freqCode)
{
  byte setting, targetSS;
  for (int i = 0; i < 2; i++)
  {
    if (i == 0)
    {
      targetSS = BOARD_ADS;
    }
    if (i == 1)
    {
      if (daisyPresent == false)
      {
        return;
      }
      targetSS = DAISY_ADS;
    }
    if (amplitudeCode == ADSTESTSIG_NOCHANGE)
      amplitudeCode = (RREG(CONFIG2, targetSS) & (0b00000100));
    if (freqCode == ADSTESTSIG_NOCHANGE)
      freqCode = (RREG(CONFIG2, targetSS) & (0b00000011));
    freqCode &= 0b00000011;                               //only the last two bits are used
    amplitudeCode &= 0b00000100;                          //only this bit is used
    byte setting = 0b11010000 | freqCode | amplitudeCode; //compose the code
    WREG(CONFIG2, setting, targetSS);
    delay(1);
    if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
    {
      Serial1.print("Wrote to CONFIG2: ");
      Serial1.print(setting, BIN);
    }
  }
}

void OpenBCI_32bit_Library::changeInputType(byte inputCode)
{

  for (int i = 0; i < numChannels; i++)
  {
    channelSettings[i][INPUT_TYPE_SET] = inputCode;
  }

  // OLD CODE REVERT
  //channelSettingsArraySetForAll();

  writeChannelSettings();
}

// Start continuous data acquisition
void OpenBCI_32bit_Library::startADS(void) // NEEDS ADS ADDRESS, OR BOTH?
{
  sampleCounter = 0;
  sampleCounterBLE = 0;
  firstDataPacket = true;
  RDATAC(BOTH_ADS); // enter Read Data Continuous mode
  delay(1);
  START(BOTH_ADS); // start the data acquisition
  delay(1);
  isRunning = true;
}

/**
* @description Check status register to see if data is available from the ADS1299.
* @returns {boolean} - `true` if data is available
*/
boolean OpenBCI_32bit_Library::isADSDataAvailable(void)
{
  return (!(digitalRead(ADS_DRDY)));
}

// CALLED WHEN DRDY PIN IS ASSERTED. NEW ADS DATA AVAILABLE!
void OpenBCI_32bit_Library::updateChannelData(void)
{
  // this needs to be reset, or else it will constantly flag us
  channelDataAvailable = false;

  lastSampleTime = millis();

  boolean downsample = true;
  if (iSerial0.tx == false && iSerial1.baudRate > OPENBCI_BAUD_RATE_MIN_NO_AVG)
  {
    downsample = false;
  }

  updateBoardData(downsample);
  if (daisyPresent)
  {
    updateDaisyData(downsample);
  }

  switch (curBoardMode)
  {
  case BOARD_MODE_ANALOG:
    auxData[0] = analogRead(A5);
    auxData[1] = analogRead(A6);
    if (!wifi.present)
    {
      auxData[2] = analogRead(A7);
    }
    break;
  case BOARD_MODE_DIGITAL:
    auxData[0] = digitalRead(11) << 8 | digitalRead(12);
    auxData[1] = (wifi.present ? 0 : digitalRead(13) << 8) | digitalRead(17);
    auxData[2] = wifi.present ? 0 : digitalRead(18);
    break;
  case BOARD_MODE_MARKER:
    if (newMarkerReceived)
    {
      auxData[0] = (short)markerValue;
      newMarkerReceived = false;
    }
    break;
  case BOARD_MODE_BLE:
  case BOARD_MODE_DEBUG:
  case BOARD_MODE_DEFAULT:
    break;
  }
}

void OpenBCI_32bit_Library::updateBoardData(void)
{
  updateBoardData(true);
}

void OpenBCI_32bit_Library::updateBoardData(boolean downsample)
{
  byte inByte;
  int byteCounter = 0;

  if ((daisyPresent || curBoardMode == BOARD_MODE_BLE) && !firstDataPacket && downsample)
  {
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    {                                                      // shift and average the byte arrays
      lastBoardChannelDataInt[i] = boardChannelDataInt[i]; // remember the last samples
    }
  }

  csLow(BOARD_ADS); //  open SPI
  for (int i = 0; i < 3; i++)
  {
    inByte = xfer(0x00); //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    boardStat = (boardStat << 8) | inByte;
  }
  for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
  {
    for (int j = 0; j < OPENBCI_ADS_BYTES_PER_CHAN; j++)
    { //  read 24 bits of channel data in 8 3 byte chunks
      inByte = xfer(0x00);
      boardChannelDataRaw[byteCounter] = inByte; // raw data goes here
      byteCounter++;
      boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte; // int data goes here
    }
  }
  csHigh(BOARD_ADS); // close SPI

  // need to convert 24bit to 32bit if using the filter
  for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
  { // convert 3 byte 2's compliment to 4 byte 2's compliment
    if (bitRead(boardChannelDataInt[i], 23) == 1)
    {
      boardChannelDataInt[i] |= 0xFF000000;
    }
    else
    {
      boardChannelDataInt[i] &= 0x00FFFFFF;
    }
  }
  if ((daisyPresent || curBoardMode == BOARD_MODE_BLE) && !firstDataPacket && downsample)
  {
    byteCounter = 0;
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    { // take the average of this and the last sample
      meanBoardChannelDataInt[i] = (lastBoardChannelDataInt[i] + boardChannelDataInt[i]) / 2;
    }
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    { // place the average values in the meanRaw array
      for (int b = 2; b >= 0; b--)
      {
        meanBoardDataRaw[byteCounter] = (meanBoardChannelDataInt[i] >> (b * 8)) & 0xFF;
        byteCounter++;
      }
    }
  }

  if (firstDataPacket == true)
  {
    firstDataPacket = false;
  }
}

/**
* @description Read from the Daisy's ADS1299 chip and fill the core arrays with
*  new data. Defaults to downsampling if the daisy is present.
* @author AJ Keller (@aj-ptw)
*/
void OpenBCI_32bit_Library::updateDaisyData(void)
{
  updateDaisyData(true);
}

/**
* @description Read from the Daisy's ADS1299 chip and fill the core arrays with
*  new data.
* @param `downsample` {boolean} - Averages the last sample with the current to
*  cut the sample rate in half.
* @author AJ Keller (@aj-ptw)
*/
void OpenBCI_32bit_Library::updateDaisyData(boolean downsample)
{
  byte inByte;
  int byteCounter = 0;

  if (daisyPresent && !firstDataPacket && downsample)
  {
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    {                                                      // shift and average the byte arrays
      lastDaisyChannelDataInt[i] = daisyChannelDataInt[i]; // remember the last samples
    }
  }

  // Open SPI
  csLow(DAISY_ADS);
  // Read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
  // TODO: Do we really need to read this status register ever time?
  for (int i = 0; i < 3; i++)
  {
    inByte = xfer(0x00);
    daisyStat = (daisyStat << 8) | inByte;
  }

  // Read 24 bits of channel data in 8 3 byte chunks
  for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
  {
    for (int j = 0; j < OPENBCI_ADS_BYTES_PER_CHAN; j++)
    {
      inByte = xfer(0x00);
      daisyChannelDataRaw[byteCounter] = inByte; // raw data goes here
      byteCounter++;
      daisyChannelDataInt[i] = (daisyChannelDataInt[i] << 8) | inByte; // int data goes here
    }
  }

  // Close SPI
  csHigh(DAISY_ADS);

  // Convert 24bit to 32bit
  for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
  {
    // Convert 3 byte 2's compliment to 4 byte 2's compliment
    if (bitRead(daisyChannelDataInt[i], 23) == 1)
    {
      daisyChannelDataInt[i] |= 0xFF000000;
    }
    else
    {
      daisyChannelDataInt[i] &= 0x00FFFFFF;
    }
  }

  if (daisyPresent && !firstDataPacket && downsample)
  {
    byteCounter = 0;
    // Average this sample with the last sample
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    {
      meanDaisyChannelDataInt[i] = (lastDaisyChannelDataInt[i] + daisyChannelDataInt[i]) / 2;
    }
    // Place the average values in the meanRaw array
    for (int i = 0; i < OPENBCI_ADS_CHANS_PER_BOARD; i++)
    {
      for (int b = 2; b >= 0; b--)
      {
        meanDaisyDataRaw[byteCounter] = (meanDaisyChannelDataInt[i] >> (b * 8)) & 0xFF;
        byteCounter++;
      }
    }
  }

  if (firstDataPacket == true)
  {
    firstDataPacket = false;
  }
}

// Stop the continuous data acquisition
void OpenBCI_32bit_Library::stopADS()
{
  STOP(BOTH_ADS); // stop the data acquisition
  delay(1);
  SDATAC(BOTH_ADS); // stop Read Data Continuous mode to communicate with ADS
  delay(1);
  isRunning = false;
}

void OpenBCI_32bit_Library::printSerial(int i)
{
  if (iSerial0.tx && !commandFromSPI)
  {
    Serial0.print(i);
  }
  if (iSerial1.tx)
  {
    Serial1.print(i);
  }
}

void OpenBCI_32bit_Library::printSerial(char c)
{
  if (iSerial0.tx && !commandFromSPI)
  {
    Serial0.print(c);
  }
  if (iSerial1.tx)
  {
    Serial1.print(c);
  }
}

void OpenBCI_32bit_Library::printSerial(int c, int arg)
{
  if (iSerial0.tx && !commandFromSPI)
  {
    Serial0.print(c, arg);
  }
  if (iSerial1.tx)
  {
    Serial1.print(c, arg);
  }
}

void OpenBCI_32bit_Library::printSerial(const char *c)
{
  if (c != NULL)
  {
    for (int i = 0; i < strlen(c); i++)
    {
      printSerial(c[i]);
    }
  }
}

void OpenBCI_32bit_Library::printlnSerial(void)
{
  printSerial("\n");
}

void OpenBCI_32bit_Library::printlnSerial(char c)
{
  printSerial(c);
  printlnSerial();
}

void OpenBCI_32bit_Library::printlnSerial(int c)
{
  printSerial(c);
  printlnSerial();
}

void OpenBCI_32bit_Library::printlnSerial(int c, int arg)
{
  printSerial(c, arg);
  printlnSerial();
}

void OpenBCI_32bit_Library::printlnSerial(const char *c)
{
  printSerial(c);
  printlnSerial();
}

void OpenBCI_32bit_Library::write(uint8_t b)
{
  wifi.storeByteBufTx(b);
  writeSerial(b);
}

void OpenBCI_32bit_Library::writeSerial(uint8_t c)
{
  if (iSerial0.tx)
  {
    Serial0.write(c);
  }
  if (iSerial1.tx)
  {
    Serial1.write(c);
  }
}

void OpenBCI_32bit_Library::ADS_writeChannelData()
{
  ADS_writeChannelDataAvgDaisy();
  ADS_writeChannelDataNoAvgDaisy();
}

// #ifdef USE_WIFI
void OpenBCI_32bit_Library::ADS_writeChannelDataWifi(boolean daisy)
{
  if (daisy)
  {
    // Send daisy
    for (int i = 0; i < 24; i++)
    {
      wifi.storeByteBufTx(daisyChannelDataRaw[i]);
    }
  }
  else
  {
    // Send on board
    for (int i = 0; i < 24; i++)
    {
      wifi.storeByteBufTx(boardChannelDataRaw[i]);
    }
  }
}
// #endif
void OpenBCI_32bit_Library::ADS_writeChannelDataAvgDaisy()
{
  if (iSerial0.tx || (iSerial1.tx && iSerial1.baudRate <= OPENBCI_BAUD_RATE_MIN_NO_AVG))
  {
    if (daisyPresent)
    {
      // Code that runs with daisy present
      if (sampleCounter % 2 != 0)
      { //CHECK SAMPLE ODD-EVEN AND SEND THE APPROPRIATE ADS DATA
        for (int i = 0; i < OPENBCI_NUMBER_BYTES_PER_ADS_SAMPLE; i++)
        {
          writeSerial(meanBoardDataRaw[i]);
        }
      }
      else
      {
        for (int i = 0; i < OPENBCI_NUMBER_BYTES_PER_ADS_SAMPLE; i++)
        {
          writeSerial(meanDaisyDataRaw[i]);
        }
      }
      // Code that runs without the daisy present
    }
    else
    {
      for (int i = 0; i < 24; i++)
      {
        writeSerial(boardChannelDataRaw[i]);
      }
    }
  }
}

void OpenBCI_32bit_Library::ADS_writeChannelDataNoAvgDaisy()
{
  if (iSerial1.tx && iSerial1.baudRate > OPENBCI_BAUD_RATE_MIN_NO_AVG)
  {
    // Don't run this function if the serial baud rate is not greater then the
    // minimum
    // Always write board ADS data
    for (int i = 0; i < OPENBCI_NUMBER_BYTES_PER_ADS_SAMPLE; i++)
    {
      writeSerial(boardChannelDataRaw[i]);
    }

    // Only write daisy data if present
    if (daisyPresent)
    {
      for (int i = 0; i < OPENBCI_NUMBER_BYTES_PER_ADS_SAMPLE; i++)
      {
        writeSerial(daisyChannelDataRaw[i]);
      }
    }
  }
}

//print out the state of all the control registers
void OpenBCI_32bit_Library::printADSregisters(int targetSS)
{
  boolean prevverbosityState = verbosity;
  verbosity = true;                   // set up for verbosity output
  RREGS(0x00, 0x0C, targetSS);        // read out the first registers
  delay(10);                          // stall to let all that data get read by the PC
  RREGS(0x0D, 0x17 - 0x0D, targetSS); // read out the rest
  verbosity = prevverbosityState;
}

byte OpenBCI_32bit_Library::ADS_getDeviceID(int targetSS)
{ // simple hello world com check
  byte data = RREG(ID_REG, targetSS);
  if (verbosity)
  { // verbosity otuput
    printAll("On Board ADS ID ");
    printHex(data);
    printlnAll();
    sendEOT();
  }
  return data;
}

//System Commands
void OpenBCI_32bit_Library::WAKEUP(int targetSS)
{
  csLow(targetSS);
  xfer(_WAKEUP);
  csHigh(targetSS);
  delayMicroseconds(3); //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

void OpenBCI_32bit_Library::STANDBY(int targetSS)
{ // only allowed to send WAKEUP after sending STANDBY
  csLow(targetSS);
  xfer(_STANDBY);
  csHigh(targetSS);
}

void OpenBCI_32bit_Library::RESET(int targetSS)
{ // reset all the registers to default settings
  csLow(targetSS);
  xfer(_RESET);
  delayMicroseconds(12); //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  csHigh(targetSS);
}

void OpenBCI_32bit_Library::START(int targetSS)
{ //start data conversion
  csLow(targetSS);
  xfer(_START); // KEEP ON-BOARD AND ON-DAISY IN SYNC
  csHigh(targetSS);
}

void OpenBCI_32bit_Library::STOP(int targetSS)
{ //stop data conversion
  csLow(targetSS);
  xfer(_STOP); // KEEP ON-BOARD AND ON-DAISY IN SYNC
  csHigh(targetSS);
}

void OpenBCI_32bit_Library::RDATAC(int targetSS)
{
  csLow(targetSS);
  xfer(_RDATAC); // read data continuous
  csHigh(targetSS);
  delayMicroseconds(3);
}
void OpenBCI_32bit_Library::SDATAC(int targetSS)
{
  csLow(targetSS);
  xfer(_SDATAC);
  csHigh(targetSS);
  delayMicroseconds(10); //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}

//  THIS NEEDS CLEANING AND UPDATING TO THE NEW FORMAT
void OpenBCI_32bit_Library::RDATA(int targetSS)
{                  //  use in Stop Read Continuous mode when DRDY goes low
  byte inByte;     //  to read in one sample of the channels
  csLow(targetSS); //  open SPI
  xfer(_RDATA);    //  send the RDATA command
  for (int i = 0; i < 3; i++)
  { //  read in the status register and new channel data
    inByte = xfer(0x00);
    boardStat = (boardStat << 8) | inByte; //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
  }
  if (targetSS == BOARD_ADS)
  {
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 3; j++)
      { //  read in the new channel data
        inByte = xfer(0x00);
        boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
      }
    }
    for (int i = 0; i < 8; i++)
    {
      if (bitRead(boardChannelDataInt[i], 23) == 1)
      { // convert 3 byte 2's compliment to 4 byte 2's compliment
        boardChannelDataInt[i] |= 0xFF000000;
      }
      else
      {
        boardChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }
  else
  {
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 3; j++)
      { //  read in the new channel data
        inByte = xfer(0x00);
        daisyChannelDataInt[i] = (daisyChannelDataInt[i] << 8) | inByte;
      }
    }
    for (int i = 0; i < 8; i++)
    {
      if (bitRead(daisyChannelDataInt[i], 23) == 1)
      { // convert 3 byte 2's compliment to 4 byte 2's compliment
        daisyChannelDataInt[i] |= 0xFF000000;
      }
      else
      {
        daisyChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }
  csHigh(targetSS); //  close SPI
}

byte OpenBCI_32bit_Library::RREG(byte _address, int targetSS)
{                                 //  reads ONE register at _address
  byte opcode1 = _address + 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  csLow(targetSS);                //  open SPI
  xfer(opcode1);                  //  opcode1
  xfer(0x00);                     //  opcode2
  regData[_address] = xfer(0x00); //  update mirror location with returned byte
  csHigh(targetSS);               //  close SPI
  if (verbosity)
  { //  verbosity output
    printRegisterName(_address);
    printHex(_address);
    printAll(", ");
    printHex(regData[_address]);
    printAll(", ");
    for (byte j = 0; j < 8; j++)
    {
      char buf[3];
      printAll(itoa(bitRead(regData[_address], 7 - j), buf, DEC));
      if (j != 7)
        printAll(", ");
    }

    printlnAll();
  }
  return regData[_address]; // return requested register value
}

// Read more than one register starting at _address
void OpenBCI_32bit_Library::RREGS(byte _address, byte _numRegistersMinusOne, int targetSS)
{

  byte opcode1 = _address + 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  csLow(targetSS);                //  open SPI
  xfer(opcode1);                  //  opcode1
  xfer(_numRegistersMinusOne);    //  opcode2
  for (int i = 0; i <= _numRegistersMinusOne; i++)
  {
    regData[_address + i] = xfer(0x00); //  add register byte to mirror array
  }
  csHigh(targetSS); //  close SPI
  if (verbosity)
  { //  verbosity output
    for (int i = 0; i <= _numRegistersMinusOne; i++)
    {
      printRegisterName(_address + i);
      printHex(_address + i);
      printAll(", ");
      printHex(regData[_address + i]);
      printAll(", ");
      for (int j = 0; j < 8; j++)
      {
        char buf[3];
        printAll(itoa(bitRead(regData[_address + i], 7 - j), buf, DEC));
        if (j != 7)
          printAll(", ");
      }
      printlnAll();
      if (!commandFromSPI)
        delay(30);
    }
  }
}

void OpenBCI_32bit_Library::WREG(byte _address, byte _value, int target_SS)
{                                 //  Write ONE register at _address
  byte opcode1 = _address + 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow(target_SS);               //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(0x00);                     //  Send number of registers to read -1
  xfer(_value);                   //  Write the value to the register
  csHigh(target_SS);              //  close SPI
  regData[_address] = _value;     //  update the mirror array
  if (verbosity)
  { //  verbosity output
    printAll("Register ");
    printHex(_address);
    printlnAll(" modified.");
    sendEOT();
  }
}

void OpenBCI_32bit_Library::WREGS(byte _address, byte _numRegistersMinusOne, int targetSS)
{
  byte opcode1 = _address + 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow(targetSS);                //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(_numRegistersMinusOne);    //  Send number of registers to read -1
  for (int i = _address; i <= (_address + _numRegistersMinusOne); i++)
  {
    xfer(regData[i]); //  Write to the registers
  }
  csHigh(targetSS);
  if (verbosity)
  {
    printAll("Registers ");
    printHex(_address);
    printAll(" to ");
    printHex(_address + _numRegistersMinusOne);
    printlnAll(" modified");
    sendEOT();
  }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<  END OF ADS1299 FUNCTIONS  >>>>>>>>>>>>>>>>>>>>>>>>>
// ******************************************************************************
// <<<<<<<<<<<<<<<<<<<<<<<<<  LIS3DH FUNCTIONS  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void OpenBCI_32bit_Library::initialize_accel(byte g)
{
  byte setting = g | 0x08;          // mask the g range for REG4
  pinMode(LIS3DH_DRDY, INPUT);      // setup dataReady interupt from accelerometer
  LIS3DH_write(TMP_CFG_REG, 0x00);  // DISable ADC inputs, enable temperature sensor
  LIS3DH_write(CTRL_REG1, 0x08);    // disable accel, low power mode
  LIS3DH_write(CTRL_REG2, 0x00);    // don't use the high pass filter
  LIS3DH_write(CTRL_REG3, 0x00);    // no interrupts yet
  LIS3DH_write(CTRL_REG4, setting); // set scale to g, high resolution
  LIS3DH_write(CTRL_REG5, 0x00);    // no boot, no fifo
  LIS3DH_write(CTRL_REG6, 0x00);
  LIS3DH_write(REFERENCE, 0x00);
  DRDYpinValue = lastDRDYpinValue = digitalRead(LIS3DH_DRDY); // take a reading to seed these variables
}

void OpenBCI_32bit_Library::enable_accel(byte Hz)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0; // clear the axisData array so we don't get any stale news
  }
  byte setting = Hz | 0x07;         // mask the desired frequency
  LIS3DH_write(CTRL_REG1, setting); // set freq and enable all axis in normal mode
  LIS3DH_write(CTRL_REG3, 0x10);    // enable DRDY1 on INT1 (tied to PIC pin 0, LIS3DH_DRDY)
}

void OpenBCI_32bit_Library::disable_accel()
{
  LIS3DH_write(CTRL_REG1, 0x08); // power down, low power mode
  LIS3DH_write(CTRL_REG3, 0x00); // disable DRDY1 on INT1
}

byte OpenBCI_32bit_Library::LIS3DH_getDeviceID()
{
  return LIS3DH_read(WHO_AM_I);
}

boolean OpenBCI_32bit_Library::LIS3DH_DataAvailable()
{
  boolean x = false;
  if ((LIS3DH_read(STATUS_REG2) & 0x08) > 0)
    x = true; // read STATUS_REG
  return x;
}

boolean OpenBCI_32bit_Library::LIS3DH_DataReady()
{
  boolean r = false;
  DRDYpinValue = digitalRead(LIS3DH_DRDY); // take a look at LIS3DH_DRDY pin
  if (DRDYpinValue != lastDRDYpinValue)
  { // if the value has changed since last looking
    if (DRDYpinValue == HIGH)
    {           // see if this is the rising edge
      r = true; // if so, there is fresh data!
    }
    lastDRDYpinValue = DRDYpinValue; // keep track of the changing pin
  }
  return r;
}

void OpenBCI_32bit_Library::LIS3DH_writeAxisDataSerial(void)
{
  for (int i = 0; i < 3; i++)
  {
    writeSerial(highByte(axisData[i])); // write 16 bit axis data MSB first
    writeSerial(lowByte(axisData[i]));  // axisData is array of type short (16bit)
  }
}

void OpenBCI_32bit_Library::LIS3DH_writeAxisDataForAxisSerial(uint8_t axis)
{
  if (axis > 2)
    axis = 0;
  writeSerial(highByte(axisData[axis])); // write 16 bit axis data MSB first
  writeSerial(lowByte(axisData[axis]));  // axisData is array of type short (16bit)
}

// #ifdef USE_WIFI
void OpenBCI_32bit_Library::LIS3DH_writeAxisDataWifi(void)
{
  for (int i = 0; i < 3; i++)
  {
    wifi.storeByteBufTx(highByte(axisData[i])); // write 16 bit axis data MSB first
    wifi.storeByteBufTx(lowByte(axisData[i]));  // axisData is array of type short (16bit)
  }
}
void OpenBCI_32bit_Library::LIS3DH_writeAxisDataForAxisWifi(uint8_t axis)
{
  if (axis > 2)
    axis = 0;
  wifi.storeByteBufTx(highByte(axisData[axis])); // write 16 bit axis data MSB first
  wifi.storeByteBufTx(lowByte(axisData[axis]));  // axisData is array of type short (16bit)
}
// #endif

void OpenBCI_32bit_Library::LIS3DH_zeroAxisData(void)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0;
  }
}

byte OpenBCI_32bit_Library::LIS3DH_read(byte reg)
{
  reg |= READ_REG;                  // add the READ_REG bit
  csLow(LIS3DH_SS);                 // take spi
  spi.transfer(reg);                // send reg to read
  byte inByte = spi.transfer(0x00); // retrieve data
  csHigh(LIS3DH_SS);                // release spi
  return inByte;
}

void OpenBCI_32bit_Library::LIS3DH_write(byte reg, byte value)
{
  csLow(LIS3DH_SS);    // take spi
  spi.transfer(reg);   // send reg to write
  spi.transfer(value); // write value
  csHigh(LIS3DH_SS);   // release spi
}

int OpenBCI_32bit_Library::LIS3DH_read16(byte reg)
{ // use for reading axis data.
  int inData;
  reg |= READ_REG | READ_MULTI;                            // add the READ_REG and READ_MULTI bits
  csLow(LIS3DH_SS);                                        // take spi
  spi.transfer(reg);                                       // send reg to start reading from
  inData = spi.transfer(0x00) | (spi.transfer(0x00) << 8); // get the data and arrange it
  csHigh(LIS3DH_SS);                                       // release spi
  return inData;
}

int OpenBCI_32bit_Library::getX()
{
  return LIS3DH_read16(OUT_X_L);
}

int OpenBCI_32bit_Library::getY()
{
  return LIS3DH_read16(OUT_Y_L);
}

int OpenBCI_32bit_Library::getZ()
{
  return LIS3DH_read16(OUT_Z_L);
}

void OpenBCI_32bit_Library::LIS3DH_updateAxisData()
{
  axisData[0] = getX();
  axisData[1] = getY();
  axisData[2] = getZ();
}

void OpenBCI_32bit_Library::LIS3DH_readAllRegs()
{

  byte inByte;

  for (int i = STATUS_REG_AUX; i <= WHO_AM_I; i++)
  {
    inByte = LIS3DH_read(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
  printlnAll();

  for (int i = TMP_CFG_REG; i <= INT1_DURATION; i++)
  {
    inByte = LIS3DH_read(i);
    // printRegisterName(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
  printlnAll();

  for (int i = CLICK_CFG; i <= TIME_WINDOW; i++)
  {
    inByte = LIS3DH_read(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<  END OF LIS3DH FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// String-Byte converters for ADS
void OpenBCI_32bit_Library::printRegisterName(byte _address)
{
  switch (_address)
  {
  case ID_REG:
    printAll("ADS_ID, ");
    break;
  case CONFIG1:
    printAll("CONFIG1, ");
    break;
  case CONFIG2:
    printAll("CONFIG2, ");
    break;
  case CONFIG3:
    printAll("CONFIG3, ");
    break;
  case LOFF:
    printAll("LOFF, ");
    break;
  case CH1SET:
    printAll("CH1SET, ");
    break;
  case CH2SET:
    printAll("CH2SET, ");
    break;
  case CH3SET:
    printAll("CH3SET, ");
    break;
  case CH4SET:
    printAll("CH4SET, ");
    break;
  case CH5SET:
    printAll("CH5SET, ");
    break;
  case CH6SET:
    printAll("CH6SET, ");
    break;
  case CH7SET:
    printAll("CH7SET, ");
    break;
  case CH8SET:
    printAll("CH8SET, ");
    break;
  case BIAS_SENSP:
    printAll("BIAS_SENSP, ");
    break;
  case BIAS_SENSN:
    printAll("BIAS_SENSN, ");
    break;
  case LOFF_SENSP:
    printAll("LOFF_SENSP, ");
    break;
  case LOFF_SENSN:
    printAll("LOFF_SENSN, ");
    break;
  case LOFF_FLIP:
    printAll("LOFF_FLIP, ");
    break;
  case LOFF_STATP:
    printAll("LOFF_STATP, ");
    break;
  case LOFF_STATN:
    printAll("LOFF_STATN, ");
    break;
  case GPIO:
    printAll("GPIO, ");
    break;
  case MISC1:
    printAll("MISC1, ");
    break;
  case MISC2:
    printAll("MISC2, ");
    break;
  case CONFIG4:
    printAll("CONFIG4, ");
    break;
  default:
    break;
  }
}

// Used for printing HEX in verbosity feedback mode
void OpenBCI_32bit_Library::printHex(byte _data)
{
  if (_data < 0x10)
    printAll("0");
  char buf[4];
  // Serial.print(_data);
  printAll(itoa(_data, buf, HEX));
  if (commandFromSPI)
    delay(1);
}

void OpenBCI_32bit_Library::printlnHex(byte _data)
{
  printHex(_data);
  printlnAll();
}

void OpenBCI_32bit_Library::printFailure()
{
  printAll("Failure: ");
}

void OpenBCI_32bit_Library::printSuccess()
{
  printAll("Success: ");
}

void OpenBCI_32bit_Library::printAll(char c)
{
  printSerial(c);
  if (wifi.present && wifi.tx)
  {
    wifi.sendStringMulti(c);
    delay(1);
  }
}

void OpenBCI_32bit_Library::printAll(const char *arr)
{
  printSerial(arr);
  if (wifi.present && wifi.tx)
  {
    wifi.sendStringMulti(arr);
    delay(1);
  }
}

void OpenBCI_32bit_Library::printlnAll(const char *arr)
{
  printlnSerial(arr);
  if (wifi.present && wifi.tx)
  {
    wifi.sendStringMulti(arr);
    delay(1);
    wifi.sendStringMulti("\n");
    delay(1);
  }
}

void OpenBCI_32bit_Library::printlnAll(void)
{
  printlnSerial();
  if (wifi.present && wifi.tx)
  {
    wifi.sendStringMulti("\n");
    delay(1);
  }
}

/**
* @description Converts ascii character to byte value for channel setting bytes
* @param `asciiChar` - [char] - The ascii character to convert
* @return [char] - Byte number value of acsii character, defaults to 0
* @author AJ Keller (@pushtheworldllc)
*/
char OpenBCI_32bit_Library::getChannelCommandForAsciiChar(char asciiChar)
{
  switch (asciiChar)
  {
  case OPENBCI_CHANNEL_CMD_CHANNEL_1:
    return 0x00;
  case OPENBCI_CHANNEL_CMD_CHANNEL_2:
    return 0x01;
  case OPENBCI_CHANNEL_CMD_CHANNEL_3:
    return 0x02;
  case OPENBCI_CHANNEL_CMD_CHANNEL_4:
    return 0x03;
  case OPENBCI_CHANNEL_CMD_CHANNEL_5:
    return 0x04;
  case OPENBCI_CHANNEL_CMD_CHANNEL_6:
    return 0x05;
  case OPENBCI_CHANNEL_CMD_CHANNEL_7:
    return 0x06;
  case OPENBCI_CHANNEL_CMD_CHANNEL_8:
    return 0x07;
  case OPENBCI_CHANNEL_CMD_CHANNEL_9:
    return 0x08;
  case OPENBCI_CHANNEL_CMD_CHANNEL_10:
    return 0x09;
  case OPENBCI_CHANNEL_CMD_CHANNEL_11:
    return 0x0A;
  case OPENBCI_CHANNEL_CMD_CHANNEL_12:
    return 0x0B;
  case OPENBCI_CHANNEL_CMD_CHANNEL_13:
    return 0x0C;
  case OPENBCI_CHANNEL_CMD_CHANNEL_14:
    return 0x0D;
  case OPENBCI_CHANNEL_CMD_CHANNEL_15:
    return 0x0E;
  case OPENBCI_CHANNEL_CMD_CHANNEL_16:
    return 0x0F;
  default:
    return 0x00;
  }
}

/**
* @description Converts ascii '0' to number 0 and ascii '1' to number 1
* @param `asciiChar` - [char] - The ascii character to convert
* @return [char] - Byte number value of acsii character, defaults to 0
* @author AJ Keller (@pushtheworldllc)
*/
char OpenBCI_32bit_Library::getYesOrNoForAsciiChar(char asciiChar)
{
  switch (asciiChar)
  {
  case '1':
    return ACTIVATE;
  case '0':
  default:
    return DEACTIVATE;
  }
}

/**
* @description Converts ascii character to get gain from channel settings
* @param `asciiChar` - [char] - The ascii character to convert
* @return [char] - Byte number value of acsii character, defaults to 0
* @author AJ Keller (@pushtheworldllc)
*/
char OpenBCI_32bit_Library::getGainForAsciiChar(char asciiChar)
{

  char output = 0x00;

  if (asciiChar < '0' || asciiChar > '6')
  {
    asciiChar = '6'; // Default to 24
  }

  output = asciiChar - '0';

  return output << 4;
}

/**
* @description Converts ascii character to get gain from channel settings
* @param `asciiChar` - [char] - The ascii character to convert
* @return [char] - Byte number value of acsii character, defaults to 0
* @author AJ Keller (@pushtheworldllc)
*/
char OpenBCI_32bit_Library::getNumberForAsciiChar(char asciiChar)
{
  if (asciiChar < '0' || asciiChar > '9')
  {
    asciiChar = '0';
  }

  // Convert ascii char to number
  asciiChar -= '0';

  return asciiChar;
}

// custom: convert an ASCII hex char (0-9, A-F, a-f) to its 0..15 nibble value.
// Any non-hex char maps to 0 (matches getNumberForAsciiChar's lenient style).
byte OpenBCI_32bit_Library::getHexForAsciiChar(char asciiChar)
{
  if (asciiChar >= '0' && asciiChar <= '9') return (byte)(asciiChar - '0');
  if (asciiChar >= 'A' && asciiChar <= 'F') return (byte)(asciiChar - 'A' + 10);
  if (asciiChar >= 'a' && asciiChar <= 'f') return (byte)(asciiChar - 'a' + 10);
  return 0;
}

/**
* @description Used to set the channelSettings array to default settings
* @param `setting` - [byte] - The byte you need a setting for....
* @returns - [byte] - Retuns the proper byte for the input setting, defualts to 0
*/
byte OpenBCI_32bit_Library::getDefaultChannelSettingForSetting(byte setting)
{
  switch (setting)
  {
  case POWER_DOWN:
    return NO;
  case GAIN_SET:
    return ADS_GAIN24;
  case INPUT_TYPE_SET:
    return ADSINPUT_NORMAL;
  case BIAS_SET:
    return YES;
  case SRB2_SET:
    return YES;
  case SRB1_SET:
  default:
    return NO;
  }
}

/**
* @description Used to set the channelSettings array to default settings
* @param `setting` - [byte] - The byte you need a setting for....
* @returns - [char] - Retuns the proper ascii char for the input setting, defaults to '0'
*/
char OpenBCI_32bit_Library::getDefaultChannelSettingForSettingAscii(byte setting)
{
  switch (setting)
  {
  case GAIN_SET: // Special case where GAIN_SET needs to be shifted first
    return (ADS_GAIN24 >> 4) + '0';
  default: // All other settings are just adding the ascii value for '0'
    return getDefaultChannelSettingForSetting(setting) + '0';
  }
}

/**
* @description Convert user channelNumber for use in array indexs by subtracting 1,
*                  also make sure N is not greater than 15 or less than 0
* @param `channelNumber` - [byte] - The channel number
* @return [byte] - Constrained channel number
*/
char OpenBCI_32bit_Library::getConstrainedChannelNumber(byte channelNumber)
{
  return constrain(channelNumber - 1, 0, OPENBCI_NUMBER_OF_CHANNELS_DAISY - 1);
}

/**
* @description Get slave select pin for channelNumber
* @param `channelNumber` - [byte] - The channel number
* @return [byte] - Constrained channel number
*/
char OpenBCI_32bit_Library::getTargetSSForConstrainedChannelNumber(byte channelNumber)
{
  // Is channelNumber in the range of default [0,7]
  if (channelNumber < OPENBCI_NUMBER_OF_CHANNELS_DEFAULT)
  {
    return BOARD_ADS;
  }
  else
  {
    return DAISY_ADS;
  }
}

/**
* @description Used to set the channelSettings array to default settings
* @param `channelSettingsArray` - [byte **] - Takes a two dimensional array of
*          length OPENBCI_NUMBER_OF_CHANNELS_DAISY by 6 elements
*/
void OpenBCI_32bit_Library::resetChannelSettingsArrayToDefault(byte channelSettingsArray[][OPENBCI_NUMBER_OF_CHANNEL_SETTINGS])
{
  // Loop through all channels
  for (int i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++)
  {
    channelSettingsArray[i][POWER_DOWN] = getDefaultChannelSettingForSetting(POWER_DOWN);         // on = NO, off = YES
    channelSettingsArray[i][GAIN_SET] = getDefaultChannelSettingForSetting(GAIN_SET);             // Gain setting
    channelSettingsArray[i][INPUT_TYPE_SET] = getDefaultChannelSettingForSetting(INPUT_TYPE_SET); // input muxer setting
    channelSettingsArray[i][BIAS_SET] = getDefaultChannelSettingForSetting(BIAS_SET);             // add this channel to bias generation
    channelSettingsArray[i][SRB2_SET] = getDefaultChannelSettingForSetting(SRB2_SET);             // connect this P side to SRB2
    channelSettingsArray[i][SRB1_SET] = getDefaultChannelSettingForSetting(SRB1_SET);             // don't use SRB1

    useInBias[i] = true; // keeping track of Bias Generation
    useSRB2[i] = true;   // keeping track of SRB2 inclusion
  }

  boardUseSRB1 = daisyUseSRB1 = false;
}

/**
* @description Used to set the channelSettings array to default settings
* @param `channelSettingsArray` - [byte **] - A two dimensional array of
*          length OPENBCI_NUMBER_OF_CHANNELS_DAISY by 2 elements
*/
void OpenBCI_32bit_Library::resetLeadOffArrayToDefault(byte leadOffArray[][OPENBCI_NUMBER_OF_LEAD_OFF_SETTINGS])
{
  // Loop through all channels
  for (int i = 0; i < OPENBCI_NUMBER_OF_CHANNELS_DAISY; i++)
  {
    leadOffArray[i][PCHAN] = OFF;
    leadOffArray[i][NCHAN] = OFF;
  }
}

OpenBCI_32bit_Library board;
