#include "Arduino.h"
#include "fosmltIRSend.h"
#include <driver/rmt.h>
//#include <stdio.h>

#define IR_LED GPIO_NUM_5

fosmltIRSend::fosmltIRSend()
{}

void fosmltIRSend::attach(Packet shotPacket, int IRheader, int IR1, int IR0, int IRgap, int IRfreq, uint8_t playerIDlength, uint8_t teamIDlength, uint8_t gunDMGlength, uint8_t packettotallength)
{//https://github.com/rocketstrong600/LaserTag/blob/master/lib/MilesTag/MilesTag.cpp
//http://www.buildlog.net/blog/2017/11/esp32-sending-short-pulses-with-the-rmt/
  configTx.rmt_mode = RMT_MODE_TX;
  configTx.channel = RMT_CHANNEL_0;
  configTx.gpio_num = IR_LED;
  configTx.mem_block_num = 1;
  configTx.tx_config.loop_en = 0;
  configTx.tx_config.carrier_duty_percent = 50;
  configTx.tx_config.carrier_freq_hz = IRfreq; //or 38000
  configTx.tx_config.carrier_en = 1;  //0 for using wire
  configTx.tx_config.idle_output_en = 1;
  configTx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  configTx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
  configTx.clk_div = 80; // 80MHz / 80 = 1MHz 0r 1uS per count

  rmt_config(&configTx);
  rmt_driver_install(configTx.channel, 0, 0);  //  rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int rmt_intr_num)

  shotPacketLength = packettotallength + 1; //+1 for header

  uint16_t shotPacket2Item = shotPacket.playerID;  //fill this long with struct info
  shotPacket2Item = shotPacket2Item << teamIDlength;
  shotPacket2Item = shotPacket2Item | shotPacket.teamID;
  shotPacket2Item = shotPacket2Item << gunDMGlength;
  shotPacket2Item = shotPacket2Item | shotPacket.gunDMG;

//thanks to https://github.com/rocketstrong600/LaserTag/blob/master/lib/MilesTag/MilesTag.cpp
//for a far neater way of creating this packet
  items[0].duration0 = IRheader;
  items[0].level0 = 1;
  items[0].duration1 = IRgap;
  items[0].level1 = 0;
  int i = 1;
	for (uint16_t  mask = 1U << (packettotallength - 1);  mask;  mask >>= 1) {
		if (shotPacket2Item & mask) {
      items[i].duration0 = IR1;
      items[i].level0 = 1;
      items[i].duration1 = IRgap;
      items[i].level1 = 0;
		}
    else
    {
      items[i].duration0 = IR0;
      items[i].level0 = 1;
      items[i].duration1 = IRgap;
      items[i].level1 = 0;
    }
    //printf("%i\n", items[i].duration0);
    i++;
  }
}

void fosmltIRSend::IRTransmit()
{
  rmt_write_items(configTx.channel, items, shotPacketLength+1, 0);
//@param wait_tx_done
//        - If set 1, it will block the task and wait for sending done.
//        - If set 0, it will not wait and return immediately.
}
