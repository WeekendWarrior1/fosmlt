#ifndef fosmltIRReceive_h
#define fosmltIRReceive_h

#include "Arduino.h"
#include <driver/rmt.h>

typedef struct Packet{
  unsigned int playerID; //size of 8 bits
  unsigned int teamID;
  unsigned int gunDMG;
} Packet;

class fosmltIRReceive
{
  public:
    fosmltIRReceive(int IRheaderRec, int IR1Rec, int IR0Rec, int IRgapRec, int IRfreqRec, uint8_t playerIDlengthRec, uint8_t teamIDlengthRec, uint8_t gunDMGlengthRec, uint8_t packettotallengthRec);
    void IRBuffer();
    void clearIRbuffer();
    //void shotParsedAcknowledgement();
    rmt_config_t configRx;
    //ir vars
    int IRheader;
    int IR1;
    int IR0;
    int IRgap;
    uint8_t packettotallength;
    uint8_t playerIDlength;
    uint8_t teamIDlength;
    uint8_t gunDMGlength;
    int teamIDlengthMaxVal;
    int gunDMGlengthMaxVal;
    bool shotReceviedFlag;
    Packet receivedShot;
  private:

};

#endif