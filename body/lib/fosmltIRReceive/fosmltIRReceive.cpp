#include "Arduino.h"
#include "fosmltIRReceive.h"
#include <driver/rmt.h>
#include <stdio.h>

//#define RMT_RX_ACTIVE_LEVEL 0 /*!< If we connect with a IR receiver, the data is active low */
#define IR_Receiver GPIO_NUM_14

fosmltIRReceive::fosmltIRReceive(int IRheaderRec, int IR1Rec, int IR0Rec, int IRgapRec, int IRfreqRec, uint8_t playerIDlengthRec, uint8_t teamIDlengthRec, uint8_t gunDMGlengthRec, uint8_t packettotallengthRec)
{//https://github.com/rocketstrong600/LaserTag/blob/master/lib/MilesTag/MilesTag.cpp
//http://www.buildlog.net/blog/2017/11/esp32-sending-short-pulses-with-the-rmt/
  configRx.rmt_mode = RMT_MODE_RX;
  configRx.channel = RMT_CHANNEL_1;
  configRx.gpio_num = IR_Receiver;
  configRx.mem_block_num = 1;
  configRx.rx_config.filter_en = true;
  configRx.rx_config.filter_ticks_thresh = 255;
  configRx.rx_config.idle_threshold = IRheaderRec + 500;
  configRx.clk_div = 80; //if we divide 80MHz by 80, we get 1MHz, = 1us per tick

  rmt_config(&configRx);
  rmt_driver_install(configRx.channel, 1000, 0);
  rmt_rx_start(RMT_CHANNEL_1, 1);

  IRheader = IRheaderRec;
  IR1 = IR1Rec;
  IR0 = IR0Rec;
  IRgap = IRgapRec;
  playerIDlength = playerIDlengthRec;
  teamIDlength = teamIDlengthRec;
  gunDMGlength = gunDMGlengthRec;
  packettotallength = packettotallengthRec;
  teamIDlengthMaxVal = ((1 << teamIDlength) - 1);
  gunDMGlengthMaxVal = ((1 << gunDMGlength) - 1);
  shotReceviedFlag = false;
}

void fosmltIRReceive::IRBuffer() // TODO: test robustness of this by sending varying length packets
{
  unsigned long data = 0;
  RingbufHandle_t rb = NULL;
  rmt_get_ringbuf_handle(RMT_CHANNEL_1, &rb);
  while(rb)
  {
    size_t rx_size = 0;
    rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
    if (item)
    {
      rmt_item32_t* itemproc = item;
      for(int i = 0; i <= packettotallength;i++) //debug loop
      {
        printf("%i: %i\n", i,itemproc[i].duration0);
      }
      for(size_t i=0; i < (rx_size / 4); i++)
      {
        if(itemproc->duration0 < (IRheader+100) && itemproc->duration0 > (IRheader-100))
        {
          for(int i=packettotallength; i >= 1; i--)
          {
            if (itemproc[i].duration0 < (IR1+100) && itemproc[i].duration0 > (IR1-100))
            {
              data = data | 1 << (packettotallength - i);
            }
             else if(itemproc[i].duration0 < (IR0+100) && itemproc[i].duration0 > (IR0-100))
            {
              data = data | 0 << (packettotallength - i);
            }
            else 
            {
              printf("Junk duration received, length of: %i\n", itemproc[i].duration0);
            }
          }
          receivedShot.gunDMG = data & gunDMGlengthMaxVal;  //strip gunDMG from data  ((1 << gunDMGlength) - 1)?
          receivedShot.teamID = (data >> gunDMGlength) & teamIDlengthMaxVal; //strip teamID from data
          receivedShot.playerID = (data >> (gunDMGlength + teamIDlength));

          printf("receivedShot.gunDMG: %i\n", receivedShot.gunDMG);
          printf("receivedShot.teamID: %i\n", receivedShot.teamID);
          printf("receivedShot.playerID: %i\n", receivedShot.playerID);

          data = 0;
          shotReceviedFlag = true;
        }
        ++itemproc;
      }
      vRingbufferReturnItem(rb, (void*) item);
    }
      else
    {
      break;
    }
  }
}

/*void shotParsedAcknowledgement()
{
  shotReceviedFlag = false;
}*/

void fosmltIRReceive::clearIRbuffer() //requires testing
{
  RingbufHandle_t rb = NULL;
  rmt_get_ringbuf_handle(RMT_CHANNEL_1, &rb);
  while(rb)
  {
    size_t rx_size = 0;
    rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
    vRingbufferReturnItem(rb, (void*) item);
  }
}