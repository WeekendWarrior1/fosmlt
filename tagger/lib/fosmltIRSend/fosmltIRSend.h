#ifndef fosmltIRSend_h
#define fosmltIRSend_h

#include "Arduino.h"
#include <driver/rmt.h>

struct Packet{
  unsigned int playerID: 8; //size of 8 bits
  unsigned int teamID: 2;
  unsigned int gunDMG: 4;
}; //creates shotpacket var as Packet

class fosmltIRSend
{
  public:
    fosmltIRSend();
    void attach(Packet shotPacket, int IRheader, int IR1, int IR0, int IRgap, int IRfreq, uint8_t playerIDlength, uint8_t teamIDlength, uint8_t gunDMGlength, uint8_t packettotallength); //convert packet to RMT item here: fosmltIRSend(struct packet);
    void IRTransmit();
    rmt_config_t configTx;
    rmt_item32_t items[15]; //this can't be static forever
    uint8_t shotPacketLength;
    uint32_t totalTimeToFire;
  private:

};

#endif
