// impedance_fastcycle_logic_test.cpp
//
// Host-runnable (g++) validation of the CUSTOM fast-cycle / batched impedance
// LOGIC added to OpenBCI_32bit_Library (VSC_OpenBCI_Custom-Firmware).
//
// This is NOT a compile of the firmware (that needs Arduino + chipKIT-core + the
// PIC32 target). It is a faithful PORT of the three pieces of pure logic that have
// no hardware dependency:
//   1. getHexForAsciiChar()                  (ASCII-hex arg decode)
//   2. processIncomingImpedanceSettings()    (the g/k/m multi-char parser state machine)
//   3. impedanceFastCycleStart() + serviceImpedanceFastCycle()  (the on-chip sequencer)
// plus the host-side window segmentation that PROMPT_02 (OpenLab Recorder) will run.
//
// Every register write the firmware would issue is recorded as an event so the test
// can assert the exact LOFF_SENSP transition sequence without hardware.
//
// Build & run:   g++ -std=c++11 -Wall -o /tmp/imp_test impedance_fastcycle_logic_test.cpp && /tmp/imp_test

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

// ---- constants mirrored verbatim from OpenBCI_32bit_Library_Definitions.h ----
static const char OPENBCI_CHANNEL_IMPEDANCE_LATCH = 'Z';
static const char OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET = 'g';
static const char OPENBCI_IMPEDANCE_FAST_CYCLE_SET  = 'k';
static const char OPENBCI_IMPEDANCE_DC_STATUS_SET   = 'm';
static const int  OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_ALL_AT_ONCE = 5;
static const int  OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_FAST_CYCLE  = 7;
static const int  OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_DC_STATUS   = 2;
static const uint16_t OPENBCI_IMPEDANCE_MARKER_MAGIC = 0x0FC5;
static const int  OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_DEFAULT = 32;
static const int  OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_MIN = 1;
static const int  OPENBCI_IMPEDANCE_FAST_CYCLE_SETTLE = 8;
static const short OPENBCI_IMPEDANCE_FLAG_SETTLING    = 0x0001;
static const short OPENBCI_IMPEDANCE_FLAG_LAST_SAMPLE = 0x0002;
static const short OPENBCI_IMPEDANCE_FLAG_DONE        = 0x0004;
static const int  OPENBCI_NUMBER_OF_CHANNELS_DAISY = 16;
static const int  OPENBCI_NUMBER_OF_CHANNELS_DEFAULT = 8;
static const int  PCHAN = 0, NCHAN = 1, ON = 1, OFF = 0;

// ---- minimal test harness ----
static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) { g_pass++; } else { g_fail++; \
    printf("  FAIL: %s   (line %d)\n", msg, __LINE__); } } while (0)

// ---- a faithful port of the firmware's impedance logic ----
struct RegWrite { int channel; int pOn; };  // recorded LOFF_SENSP transition (1-based ch)
struct Emitted  { short magic; short channel; short flags; };  // one emitted aux-tagged sample

struct FakeBoard {
    bool     daisyPresent = false;
    bool     streaming = true;
    int      leadOff[16][2];

    // impedance state (mirrors the new members)
    bool     impedanceFastCycleActive = false;
    bool     impedanceDcStatusActive = false;
    uint16_t impedanceChannelMask = 0;
    int      impedanceActiveChannel = 0;
    int      impedanceActiveChannelIdx = 0;
    int      impedanceDwellSamples = OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_DEFAULT;
    int      impedanceDwellCounter = 0;
    char     impedanceMultiCharMode = 0;
    int      impedanceArgCounter = 0;
    char     impedanceArgBuffer[8];
    bool     multiCharTimerActive = false;

    short auxData[3] = {0,0,0};
    std::vector<RegWrite> regWrites;     // every inline LOFF write
    std::vector<Emitted>  emitted;       // every aux-tagged sample

    FakeBoard() { for (int i=0;i<16;i++){ leadOff[i][0]=OFF; leadOff[i][1]=OFF; } }

    static uint8_t getHexForAsciiChar(char c) {
        if (c>='0'&&c<='9') return (uint8_t)(c-'0');
        if (c>='A'&&c<='F') return (uint8_t)(c-'A'+10);
        if (c>='a'&&c<='f') return (uint8_t)(c-'a'+10);
        return 0;
    }
    int getConstrained(int ch1) { int v=ch1-1; if(v<0)v=0; if(v>15)v=15; return v; }

    void leadOffInlineSetChannel(int ch1, int pOn, int nOn) {
        int idx = getConstrained(ch1);
        leadOff[idx][PCHAN]=pOn; leadOff[idx][NCHAN]=nOn;
        regWrites.push_back({ch1, pOn});
    }
    void leadOffBatchSetMask(uint16_t pMask) {
        int upper = daisyPresent ? OPENBCI_NUMBER_OF_CHANNELS_DAISY : OPENBCI_NUMBER_OF_CHANNELS_DEFAULT;
        for (int i=0;i<16;i++){
            bool on = (i<upper) && ((pMask>>i)&1);
            leadOff[i][PCHAN]= on?ON:OFF; leadOff[i][NCHAN]=OFF;
        }
    }
    void impedanceFastCycleStart(uint16_t pMask, int dwell) {
        if (dwell < OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_MIN) dwell = OPENBCI_IMPEDANCE_FAST_CYCLE_DWELL_DEFAULT;
        int upper = daisyPresent ? 16 : 8;
        if (upper<16) pMask &= (uint16_t)((1u<<upper)-1);
        impedanceChannelMask=pMask; impedanceDwellSamples=dwell;
        impedanceDwellCounter=0; impedanceActiveChannelIdx=0; impedanceActiveChannel=0;
        for(int i=0;i<16;i++){leadOff[i][PCHAN]=OFF;leadOff[i][NCHAN]=OFF;}
        if(pMask==0){impedanceFastCycleActive=false;return;}
        while(impedanceActiveChannelIdx<16 && !((impedanceChannelMask>>impedanceActiveChannelIdx)&1))
            impedanceActiveChannelIdx++;
        impedanceActiveChannel=impedanceActiveChannelIdx+1;
        leadOffInlineSetChannel(impedanceActiveChannel,ON,OFF);
        impedanceFastCycleActive=true;
    }
    // mirrors serviceImpedanceFastCycle + the sendChannelData hook that emits the sample
    void emitOneSample() {
        if(!impedanceFastCycleActive) return;
        short flags=0;
        if(impedanceDwellCounter<OPENBCI_IMPEDANCE_FAST_CYCLE_SETTLE) flags|=OPENBCI_IMPEDANCE_FLAG_SETTLING;
        bool lastOfChan = (impedanceDwellCounter+1>=impedanceDwellSamples);
        if(lastOfChan) flags|=OPENBCI_IMPEDANCE_FLAG_LAST_SAMPLE;
        auxData[0]=(short)OPENBCI_IMPEDANCE_MARKER_MAGIC;
        auxData[1]=(short)impedanceActiveChannel;
        auxData[2]=flags;
        impedanceDwellCounter++;
        if(impedanceDwellCounter>=impedanceDwellSamples){
            leadOffInlineSetChannel(impedanceActiveChannel,OFF,OFF);
            impedanceDwellCounter=0; impedanceActiveChannelIdx++;
            while(impedanceActiveChannelIdx<16 && !((impedanceChannelMask>>impedanceActiveChannelIdx)&1))
                impedanceActiveChannelIdx++;
            if(impedanceActiveChannelIdx>=16){
                impedanceFastCycleActive=false;
                auxData[2]=(short)(auxData[2]|OPENBCI_IMPEDANCE_FLAG_DONE);
                impedanceActiveChannel=0;
            } else {
                impedanceActiveChannel=impedanceActiveChannelIdx+1;
                leadOffInlineSetChannel(impedanceActiveChannel,ON,OFF);
            }
        }
        emitted.push_back({auxData[0],auxData[1],auxData[2]});
    }
    void processIncomingImpedanceSettings(char ch) {
        int expected;
        switch(impedanceMultiCharMode){
            case OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET: expected=OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_ALL_AT_ONCE; break;
            case OPENBCI_IMPEDANCE_FAST_CYCLE_SET:  expected=OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_FAST_CYCLE;  break;
            case OPENBCI_IMPEDANCE_DC_STATUS_SET:   expected=OPENBCI_NUMBER_OF_BYTES_IMPEDANCE_DC_STATUS;   break;
            default: impedanceArgCounter=0; multiCharTimerActive=false; return;
        }
        if(impedanceArgCounter==expected-1){
            if(ch!=OPENBCI_CHANNEL_IMPEDANCE_LATCH){ impedanceArgCounter=0; multiCharTimerActive=false; return; }
            if(impedanceMultiCharMode==OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET){
                uint16_t m=(getHexForAsciiChar(impedanceArgBuffer[0])<<12)|(getHexForAsciiChar(impedanceArgBuffer[1])<<8)
                          |(getHexForAsciiChar(impedanceArgBuffer[2])<<4)|(getHexForAsciiChar(impedanceArgBuffer[3]));
                leadOffBatchSetMask(m);
            } else if(impedanceMultiCharMode==OPENBCI_IMPEDANCE_FAST_CYCLE_SET){
                uint16_t m=(getHexForAsciiChar(impedanceArgBuffer[0])<<12)|(getHexForAsciiChar(impedanceArgBuffer[1])<<8)
                          |(getHexForAsciiChar(impedanceArgBuffer[2])<<4)|(getHexForAsciiChar(impedanceArgBuffer[3]));
                int d=(getHexForAsciiChar(impedanceArgBuffer[4])<<4)|getHexForAsciiChar(impedanceArgBuffer[5]);
                impedanceFastCycleStart(m,d);
            } else if(impedanceMultiCharMode==OPENBCI_IMPEDANCE_DC_STATUS_SET){
                impedanceDcStatusActive=(impedanceArgBuffer[0]=='1');
            }
            impedanceArgCounter=0; impedanceMultiCharMode=0; multiCharTimerActive=false; return;
        }
        if(impedanceArgCounter<(int)sizeof(impedanceArgBuffer)) impedanceArgBuffer[impedanceArgCounter]=ch;
        impedanceArgCounter++;
    }
    // mirror processChar's top-level dispatch for the new command chars
    void processChar(char c){
        if(multiCharTimerActive){ processIncomingImpedanceSettings(c); return; }
        if(c==OPENBCI_IMPEDANCE_ALL_AT_ONCE_SET||c==OPENBCI_IMPEDANCE_FAST_CYCLE_SET||c==OPENBCI_IMPEDANCE_DC_STATUS_SET){
            impedanceMultiCharMode=c; impedanceArgCounter=0; multiCharTimerActive=true;
        }
    }
    void feed(const std::string &s){ for(char c: s) processChar(c); }
};

int main() {
    printf("== fast-cycle / batched impedance LOGIC test ==\n");

    // A. getHexForAsciiChar
    printf("[A] getHexForAsciiChar\n");
    CHECK(FakeBoard::getHexForAsciiChar('0')==0, "hex '0'");
    CHECK(FakeBoard::getHexForAsciiChar('9')==9, "hex '9'");
    CHECK(FakeBoard::getHexForAsciiChar('A')==10,"hex 'A'");
    CHECK(FakeBoard::getHexForAsciiChar('F')==15,"hex 'F'");
    CHECK(FakeBoard::getHexForAsciiChar('a')==10,"hex 'a'");
    CHECK(FakeBoard::getHexForAsciiChar('f')==15,"hex 'f'");
    CHECK(FakeBoard::getHexForAsciiChar('Z')==0, "hex invalid -> 0");

    // B. All-at-once parser: "gFFFFZ" with daisy -> all 16 channels P on
    printf("[B] all-at-once parser 'gFFFFZ' (daisy present)\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("gFFFFZ");
      bool all=true; for(int i=0;i<16;i++) if(b.leadOff[i][PCHAN]!=ON||b.leadOff[i][NCHAN]!=OFF) all=false;
      CHECK(all, "all 16 P on, N off");
      CHECK(b.multiCharTimerActive==false, "timer ended after latch"); }

    // B2. All-at-once "g00FFZ" no daisy -> channels 1-8 only
    printf("[B2] all-at-once 'g00FFZ' (no daisy)\n");
    { FakeBoard b; b.daisyPresent=false; b.feed("g00FFZ");
      bool ok=true; for(int i=0;i<8;i++) if(b.leadOff[i][PCHAN]!=ON) ok=false;
      for(int i=8;i<16;i++) if(b.leadOff[i][PCHAN]!=OFF) ok=false;
      CHECK(ok, "channels 1-8 on, 9-16 off"); }

    // C. Fast-cycle parser arms with mask + dwell: "k00FF20Z" -> mask 0x00FF, dwell 0x20=32
    printf("[C] fast-cycle parser 'k00FF20Z'\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("k00FF20Z");
      CHECK(b.impedanceFastCycleActive==true, "fast-cycle armed");
      CHECK(b.impedanceChannelMask==0x00FF, "mask 0x00FF");
      CHECK(b.impedanceDwellSamples==0x20, "dwell 32");
      CHECK(b.impedanceActiveChannel==1, "first active channel is 1");
      CHECK(b.regWrites.size()==1 && b.regWrites[0].channel==1 && b.regWrites[0].pOn==ON, "ch1 driven on at start"); }

    // D. DC status parser + bad-latch abort
    printf("[D] DC status 'm1Z' and bad latch 'm1X'\n");
    { FakeBoard b; b.feed("m1Z"); CHECK(b.impedanceDcStatusActive==true, "DC status ON via m1Z"); }
    { FakeBoard b; b.feed("m0Z"); CHECK(b.impedanceDcStatusActive==false,"DC status OFF via m0Z"); }
    { FakeBoard b; b.feed("m1X"); CHECK(b.impedanceDcStatusActive==false && b.multiCharTimerActive==false,
                                        "bad latch aborts, state unchanged"); }

    // E. Sequencer walk: mask channels 1 and 3 (0x0005), dwell 10, settle 8
    printf("[E] sequencer walk: mask=0x0005, dwell=10\n");
    { FakeBoard b; b.daisyPresent=true; b.impedanceFastCycleStart(0x0005, 10);
      // emit until sweep ends (cap to avoid runaway)
      int guard=0; while(b.impedanceFastCycleActive && guard<1000){ b.emitOneSample(); guard++; }
      // 2 channels * 10 samples = 20 emitted
      CHECK(b.emitted.size()==20, "20 samples emitted (2 ch x dwell 10)");
      // first 10 samples channel 1, next 10 channel 3
      bool ch_ok=true;
      for(int i=0;i<10;i++) if(b.emitted[i].channel!=1) ch_ok=false;
      for(int i=10;i<20;i++) if(b.emitted[i].channel!=3) ch_ok=false;
      CHECK(ch_ok, "samples 0-9 -> ch1, 10-19 -> ch3");
      // every sample carries the magic
      bool magic_ok=true; for(auto&e:b.emitted) if(e.magic!=(short)OPENBCI_IMPEDANCE_MARKER_MAGIC) magic_ok=false;
      CHECK(magic_ok, "every sample tagged with marker magic");
      // settling: first 8 of each channel flagged settling, last 2 not
      bool settle_ok=true;
      for(int i=0;i<8;i++)  if(!(b.emitted[i].flags & OPENBCI_IMPEDANCE_FLAG_SETTLING)) settle_ok=false;
      for(int i=8;i<10;i++) if( (b.emitted[i].flags & OPENBCI_IMPEDANCE_FLAG_SETTLING)) settle_ok=false;
      for(int i=10;i<18;i++) if(!(b.emitted[i].flags & OPENBCI_IMPEDANCE_FLAG_SETTLING)) settle_ok=false;
      CHECK(settle_ok, "first 8 of each channel are settling, last 2 are not");
      // LAST flag on sample 9 (ch1) and 19 (ch3)
      CHECK((b.emitted[9].flags & OPENBCI_IMPEDANCE_FLAG_LAST_SAMPLE)!=0, "sample 9 is LAST of ch1");
      CHECK((b.emitted[19].flags & OPENBCI_IMPEDANCE_FLAG_LAST_SAMPLE)!=0, "sample 19 is LAST of ch3");
      // DONE flag only on final sample, and it keeps channel 3 (not relabeled to 0)
      CHECK((b.emitted[19].flags & OPENBCI_IMPEDANCE_FLAG_DONE)!=0, "final sample carries DONE");
      CHECK(b.emitted[19].channel==3, "final sample keeps ch3 (no lost sample)");
      bool done_unique=true; for(int i=0;i<19;i++) if(b.emitted[i].flags & OPENBCI_IMPEDANCE_FLAG_DONE) done_unique=false;
      CHECK(done_unique, "DONE appears only on the final sample");
      // register-write transitions: ch1 on, ch1 off, ch3 on, ch3 off  = 4 writes
      CHECK(b.regWrites.size()==4, "4 LOFF writes (on/off per channel)");
      CHECK(b.regWrites[0].channel==1 && b.regWrites[0].pOn==ON,  "w0: ch1 on");
      CHECK(b.regWrites[1].channel==1 && b.regWrites[1].pOn==OFF, "w1: ch1 off");
      CHECK(b.regWrites[2].channel==3 && b.regWrites[2].pOn==ON,  "w2: ch3 on");
      CHECK(b.regWrites[3].channel==3 && b.regWrites[3].pOn==OFF, "w3: ch3 off");
    }

    // F. Host-side window segmentation that PROMPT_02 will run:
    //    reconstruct per-channel USABLE (non-settling) sample counts from the marker stream.
    printf("[F] host segmentation of the emitted window\n");
    { FakeBoard b; b.daisyPresent=true; b.impedanceFastCycleStart(0x0005, 10);
      int guard=0; while(b.impedanceFastCycleActive && guard<1000){ b.emitOneSample(); guard++; }
      // segment: group by channel, count samples where SETTLING flag is clear
      int usableCh1=0, usableCh3=0; bool sawDone=false;
      for(auto&e: b.emitted){
        if(e.magic!=(short)OPENBCI_IMPEDANCE_MARKER_MAGIC) continue;
        bool settling = e.flags & OPENBCI_IMPEDANCE_FLAG_SETTLING;
        if(e.flags & OPENBCI_IMPEDANCE_FLAG_DONE) sawDone=true;
        if(!settling){ if(e.channel==1) usableCh1++; else if(e.channel==3) usableCh3++; }
      }
      CHECK(usableCh1==2, "ch1 yields 2 usable samples (dwell10 - settle8)");
      CHECK(usableCh3==2, "ch3 yields 2 usable samples");
      CHECK(sawDone, "host saw DONE terminator"); }

    printf("\n== RESULT: %d passed, %d failed ==\n", g_pass, g_fail);
    return g_fail==0 ? 0 : 1;
}
