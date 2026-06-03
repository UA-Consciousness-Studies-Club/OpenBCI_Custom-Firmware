// extended_commands_logic_test.cpp
//
// Host-runnable (g++) validation of the CUSTOM extended-command LOGIC added to
// OpenBCI_32bit_Library (VSC_OpenBCI_Custom-Firmware) — features 2..5:
//   2. acquisition profiles      (n p / n a)
//   3. contact/railing telemetry (n t  + buildContactTelemetryFrame)
//   4. channel power mask         (n c)
//   5. sample-locked event codes  (n e)
//
// Faithful PORT of the parser + action logic (no hardware). The register writes the
// firmware would issue are represented by mutations of the channelSettings array, which
// is exactly what the firmware's writeChannelSettings() consumes — so asserting the array
// after a command asserts the same state the ADS1299 would receive.
//
// Build & run:
//   g++ -std=c++11 -Wall -o /tmp/ext_test extended_commands_logic_test.cpp && /tmp/ext_test

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>

// ---- constants mirrored from OpenBCI_32bit_Library_Definitions.h ----
static const char OPENBCI_CHANNEL_IMPEDANCE_LATCH = 'Z';
static const char OPENBCI_EXTENDED_CMD_SET = 'n';
static const char SUB_PROFILE_DEFINE='p', SUB_PROFILE_APPLY='a', SUB_CHANNEL_MASK='c', SUB_TELEMETRY='t', SUB_EVENT='e';
static const int  LEN_PROFILE_DEFINE=10, LEN_PROFILE_APPLY=3, LEN_CHANNEL_MASK=6, LEN_TELEMETRY=4, LEN_EVENT=6;
static const int  N_SLOTS=4, N_CH_DAISY=16, N_CH_DEFAULT=8, ADS_CHANS_PER_BOARD=8;
static const int  POWER_DOWN=0, GAIN_SET=1, INPUT_TYPE_SET=2, BIAS_SET=3, SRB2_SET=4, SRB1_SET=5;
static const int  YES=0x01, NO=0x00, ON=1, OFF=0;
static const int  ADS_GAIN24=0x60, ADS_GAIN08=0x40, ADSINPUT_NORMAL=0;
static const uint16_t TELEMETRY_MAGIC=0x0FC6, EVENT_MAGIC=0x0FC7;
static const long RAILING_THRESHOLD=8323072L; // 0x7F0000

static int g_pass=0, g_fail=0;
#define CHECK(c,m) do{ if(c){g_pass++;} else {g_fail++; printf("  FAIL: %s (line %d)\n",m,__LINE__);} }while(0)

static uint8_t hx(char c){ if(c>='0'&&c<='9')return c-'0'; if(c>='A'&&c<='F')return c-'A'+10; if(c>='a'&&c<='f')return c-'a'+10; return 0; }

struct AcqProfile { bool valid; uint16_t mask; uint8_t gainReg, inputReg; bool bias, srb2; };

struct FakeBoard {
    bool daisyPresent=false, streaming=true, multiCharActive=false;
    int  channelSettings[16][6];
    long boardChannelDataInt[8]={0}, daisyChannelDataInt[8]={0};
    long boardStat=0, daisyStat=0;
    short auxData[3]={0,0,0};
    AcqProfile extProfiles[4];
    int  extArgCounter=0; char extArgBuffer[12];
    uint8_t extTelemetryCadence=0;
    bool extEventCodePending=false; uint16_t extEventCode=0;

    FakeBoard(){
        for(int i=0;i<16;i++){ channelSettings[i][POWER_DOWN]=NO; channelSettings[i][GAIN_SET]=ADS_GAIN24;
            channelSettings[i][INPUT_TYPE_SET]=ADSINPUT_NORMAL; channelSettings[i][BIAS_SET]=YES;
            channelSettings[i][SRB2_SET]=YES; channelSettings[i][SRB1_SET]=NO; }
        for(int i=0;i<4;i++){ extProfiles[i]={false,0,(uint8_t)ADS_GAIN24,(uint8_t)ADSINPUT_NORMAL,true,true}; }
    }

    void extProfileDefine(uint8_t slot,uint16_t mask,uint8_t g,uint8_t in,bool bias,bool srb2){
        if(slot>=N_SLOTS) return;
        extProfiles[slot]={true,mask,g,in,bias,srb2};
    }
    bool extProfileApply(uint8_t slot){
        if(slot>=N_SLOTS||!extProfiles[slot].valid)return false;
        AcqProfile*p=&extProfiles[slot]; int upper=daisyPresent?N_CH_DAISY:N_CH_DEFAULT;
        for(int i=0;i<16;i++){ bool a=(i<upper)&&((p->mask>>i)&1);
            if(a){ channelSettings[i][POWER_DOWN]=NO; channelSettings[i][GAIN_SET]=p->gainReg;
                   channelSettings[i][INPUT_TYPE_SET]=p->inputReg; channelSettings[i][BIAS_SET]=p->bias?YES:NO;
                   channelSettings[i][SRB2_SET]=p->srb2?YES:NO; channelSettings[i][SRB1_SET]=NO; }
            else { channelSettings[i][POWER_DOWN]=YES; channelSettings[i][BIAS_SET]=NO;
                   channelSettings[i][SRB2_SET]=NO; channelSettings[i][SRB1_SET]=NO; } }
        return true;
    }
    void extChannelPowerMask(uint16_t mask){
        int upper=daisyPresent?N_CH_DAISY:N_CH_DEFAULT;
        for(int i=0;i<16;i++){ bool a=(i<upper)&&((mask>>i)&1);
            channelSettings[i][POWER_DOWN]=a?NO:YES; channelSettings[i][BIAS_SET]=a?YES:NO; }
    }
    void buildContactTelemetryFrame(){
        uint16_t railing=0, contact=0;
        for(int i=0;i<ADS_CHANS_PER_BOARD;i++){ long v=boardChannelDataInt[i];
            if(v>=RAILING_THRESHOLD||v<=-RAILING_THRESHOLD) railing|=(1<<i); }
        uint8_t pB=(boardStat>>12)&0xFF, nB=(boardStat>>4)&0xFF; contact|=(uint16_t)(pB|nB);
        if(daisyPresent){ for(int i=0;i<ADS_CHANS_PER_BOARD;i++){ long v=daisyChannelDataInt[i];
            if(v>=RAILING_THRESHOLD||v<=-RAILING_THRESHOLD) railing|=(1<<(8+i)); }
            uint8_t pD=(daisyStat>>12)&0xFF, nD=(daisyStat>>4)&0xFF; contact|=(uint16_t)((pD|nD)<<8); }
        auxData[0]=(short)TELEMETRY_MAGIC; auxData[1]=(short)railing; auxData[2]=(short)contact;
    }
    // mirror the sendChannelData event/telemetry hook (fast-cycle omitted here)
    void emitSample(int sampleCounter){
        if(extEventCodePending){ auxData[0]=(short)EVENT_MAGIC; auxData[1]=(short)extEventCode; auxData[2]=0; extEventCodePending=false; }
        else if(extTelemetryCadence>0 && (sampleCounter%extTelemetryCadence==0)){ buildContactTelemetryFrame(); }
    }
    void processIncomingExtendedSettings(char c){
        if(extArgCounter<(int)sizeof(extArgBuffer)) extArgBuffer[extArgCounter]=c;
        extArgCounter++;
        int expected;
        switch(extArgBuffer[0]){
            case SUB_PROFILE_DEFINE: expected=LEN_PROFILE_DEFINE; break;
            case SUB_PROFILE_APPLY:  expected=LEN_PROFILE_APPLY;  break;
            case SUB_CHANNEL_MASK:   expected=LEN_CHANNEL_MASK;   break;
            case SUB_TELEMETRY:      expected=LEN_TELEMETRY;      break;
            case SUB_EVENT:          expected=LEN_EVENT;          break;
            default: extArgCounter=0; multiCharActive=false; return;
        }
        if(extArgCounter<expected) return;
        if(extArgBuffer[expected-1]!=OPENBCI_CHANNEL_IMPEDANCE_LATCH){ extArgCounter=0; multiCharActive=false; return; }
        switch(extArgBuffer[0]){
            case SUB_PROFILE_DEFINE:{ uint8_t slot=hx(extArgBuffer[1]);
                uint16_t mask=(hx(extArgBuffer[2])<<12)|(hx(extArgBuffer[3])<<8)|(hx(extArgBuffer[4])<<4)|hx(extArgBuffer[5]);
                uint8_t g=(uint8_t)(hx(extArgBuffer[6])<<4), in=hx(extArgBuffer[7]), fl=hx(extArgBuffer[8]);
                extProfileDefine(slot,mask,g,in,(fl&1),((fl>>1)&1)); break; }
            case SUB_PROFILE_APPLY:{ extProfileApply(hx(extArgBuffer[1])); break; }
            case SUB_CHANNEL_MASK:{ uint16_t mask=(hx(extArgBuffer[1])<<12)|(hx(extArgBuffer[2])<<8)|(hx(extArgBuffer[3])<<4)|hx(extArgBuffer[4]);
                extChannelPowerMask(mask); break; }
            case SUB_TELEMETRY:{ extTelemetryCadence=(uint8_t)((hx(extArgBuffer[1])<<4)|hx(extArgBuffer[2])); break; }
            case SUB_EVENT:{ extEventCode=(hx(extArgBuffer[1])<<12)|(hx(extArgBuffer[2])<<8)|(hx(extArgBuffer[3])<<4)|hx(extArgBuffer[4]);
                extEventCodePending=true; break; }
        }
        extArgCounter=0; multiCharActive=false;
    }
    void processChar(char c){
        if(multiCharActive){ processIncomingExtendedSettings(c); return; }
        if(c==OPENBCI_EXTENDED_CMD_SET){ extArgCounter=0; multiCharActive=true; }
    }
    void feed(const std::string&s){ for(char c:s) processChar(c); }
};

int main(){
    printf("== extended-command LOGIC test (features 2-5) ==\n");

    // [G] profile define + apply (Feature 2): np0 00FF 6 0 3 Z -> ch1-8 gain x24 normal bias+srb2
    printf("[G] profile define+apply 'np000FF603Z' (daisy)\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("np000FF603Z");
      CHECK(b.extProfiles[0].valid && b.extProfiles[0].mask==0x00FF, "slot0 defined mask 0x00FF");
      CHECK(b.extProfiles[0].gainReg==ADS_GAIN24, "gain code 6 -> 0x60");
      b.feed("na0Z");
      bool act=true; for(int i=0;i<8;i++) if(b.channelSettings[i][POWER_DOWN]!=NO||b.channelSettings[i][GAIN_SET]!=ADS_GAIN24
          ||b.channelSettings[i][BIAS_SET]!=YES||b.channelSettings[i][SRB2_SET]!=YES) act=false;
      CHECK(act,"ch1-8 active, gain x24, bias+srb2 on");
      bool down=true; for(int i=8;i<16;i++) if(b.channelSettings[i][POWER_DOWN]!=YES||b.channelSettings[i][BIAS_SET]!=NO) down=false;
      CHECK(down,"ch9-16 powered down, removed from bias"); }

    // [G2] EMG-style profile: lower gain x8 on just channel 1: n p 1 | 0001 | 4 | 0 | 3 | Z
    printf("[G2] low-gain single-channel profile 'np10001403Z'\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("np10001403Z"); b.feed("na1Z");
      CHECK(b.channelSettings[0][GAIN_SET]==ADS_GAIN08, "ch1 gain code 4 -> x8 (0x40)");
      CHECK(b.channelSettings[0][POWER_DOWN]==NO, "ch1 active");
      CHECK(b.channelSettings[1][POWER_DOWN]==YES, "ch2 down"); }

    // [G3] apply empty slot -> no-op (valid flag false)
    printf("[G3] apply empty slot 'na3Z'\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("na3Z");
      CHECK(b.extProfiles[3].valid==false, "slot3 still empty -> apply was a no-op"); }

    // [H] channel power mask (Feature 4): nc 000F Z -> ch1-4 on, rest down
    printf("[H] channel power mask 'nc000FZ' (daisy)\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("nc000FZ");
      bool ok=true; for(int i=0;i<4;i++) if(b.channelSettings[i][POWER_DOWN]!=NO) ok=false;
      for(int i=4;i<16;i++) if(b.channelSettings[i][POWER_DOWN]!=YES) ok=false;
      CHECK(ok,"ch1-4 on, ch5-16 down");
      CHECK(b.channelSettings[0][GAIN_SET]==ADS_GAIN24, "gain preserved (mask doesn't touch gain)"); }

    // [H2] no-daisy: high channels in mask are ignored (only 1-8 exist)
    printf("[H2] channel mask no-daisy 'ncFFFFZ' -> only ch1-8\n");
    { FakeBoard b; b.daisyPresent=false; b.feed("ncFFFFZ");
      bool ok=true; for(int i=0;i<8;i++) if(b.channelSettings[i][POWER_DOWN]!=NO) ok=false;
      for(int i=8;i<16;i++) if(b.channelSettings[i][POWER_DOWN]!=YES) ok=false;
      CHECK(ok,"only ch1-8 powered (no daisy)"); }

    // [I] telemetry cadence (Feature 3): nt 04 Z -> every 4 samples
    printf("[I] telemetry cadence 'nt04Z' + frame contents\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("nt04Z");
      CHECK(b.extTelemetryCadence==4, "cadence set to 4");
      // railing on board ch3, contact (lead-off) on board ch1 (LOFF_STATP bit0)
      b.boardChannelDataInt[2]=RAILING_THRESHOLD+10;     // ch3 railing
      b.boardStat=( (0x01)<<12 );                         // LOFF_STATP bit0 -> ch1 lead-off
      b.daisyChannelDataInt[0]=-(RAILING_THRESHOLD+10);  // ch9 railing (negative)
      b.buildContactTelemetryFrame();
      CHECK((uint16_t)b.auxData[0]==TELEMETRY_MAGIC, "telemetry magic 0x0FC6");
      uint16_t rail=(uint16_t)b.auxData[1], cont=(uint16_t)b.auxData[2];
      CHECK((rail&(1<<2))!=0, "ch3 flagged railing");
      CHECK((rail&(1<<8))!=0, "ch9 flagged railing (negative full-scale)");
      CHECK((rail&(1<<0))==0, "ch1 not railing");
      CHECK((cont&(1<<0))!=0, "ch1 flagged lead-off/contact"); }

    // [I2] telemetry cadence emit timing
    printf("[I2] telemetry fires only on multiples of cadence\n");
    { FakeBoard b; b.daisyPresent=false; b.feed("nt03Z");
      b.auxData[0]=0; b.emitSample(1); CHECK((uint16_t)b.auxData[0]!=TELEMETRY_MAGIC, "no frame at sample 1");
      b.auxData[0]=0; b.emitSample(3); CHECK((uint16_t)b.auxData[0]==TELEMETRY_MAGIC, "frame at sample 3"); }

    // [J] event code (Feature 5): ne 1234 Z -> pending, emitted on next sample
    printf("[J] event code 'ne1234Z'\n");
    { FakeBoard b; b.feed("ne1234Z");
      CHECK(b.extEventCodePending==true, "event pending after command");
      CHECK(b.extEventCode==0x1234, "event code 0x1234 decoded");
      b.emitSample(7);
      CHECK((uint16_t)b.auxData[0]==EVENT_MAGIC, "next sample carries event magic 0x0FC7");
      CHECK((uint16_t)b.auxData[1]==0x1234, "next sample carries the code");
      CHECK(b.extEventCodePending==false, "pending cleared after one sample"); }

    // [K] parser robustness: bad latch + unknown sub-op abort cleanly
    printf("[K] parser abort on bad latch / unknown sub-op\n");
    { FakeBoard b; b.daisyPresent=true; b.feed("np000FF603X"); // X not Z
      CHECK(b.extProfiles[0].valid==false, "bad latch -> profile not stored");
      CHECK(b.multiCharActive==false, "parser reset after bad latch"); }
    { FakeBoard b; b.feed("nz9999Z");  // unknown sub-op 'z'
      CHECK(b.multiCharActive==false, "unknown sub-op aborts"); }

    printf("\n== RESULT: %d passed, %d failed ==\n", g_pass, g_fail);
    return g_fail==0?0:1;
}
