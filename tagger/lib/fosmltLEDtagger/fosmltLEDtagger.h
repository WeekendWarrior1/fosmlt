#ifndef fosmltLEDtagger_h
#define fosmltLEDtagger_h

#include "Arduino.h"

class fosmltLEDtagger
{
  public:
    fosmltLEDtagger();
    void blinkRed();
    void muzzleFlash();
    void muzzleFade(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot);
    void muzzleAnimation(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot);
    void taggerAnimation();
    void reloadingAnimation(uint8_t tick);
  private:
    uint8_t lastFadeTick;
};

#endif