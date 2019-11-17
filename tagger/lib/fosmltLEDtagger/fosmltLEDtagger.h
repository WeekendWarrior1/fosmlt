#ifndef fosmltLEDtagger_h
#define fosmltLEDtagger_h

#include "Arduino.h"

class fosmltLEDtagger
{
  public:
    fosmltLEDtagger(uint8_t muzzleLEDs, uint8_t muzzleLEDpos[],uint8_t taggerLEDs, uint8_t taggerLEDpos[]);
    void muzzleFlash();
    void muzzleFade(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot);
    void muzzleAnimation(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot);
    void taggerAnimation(uint32_t time, uint32_t timeLastshot);
    void startReloadAnimation();
    void reloadingAnimation(uint32_t time, uint16_t reloadTime, uint32_t timeStartedReloading);
    void reloadInterrupted();

    uint16_t taggerAnimationTime;
  private:
    uint8_t lastFadeTick;
    uint8_t lastTaggerAnimationTick;
    uint8_t lastReloadAnimationTick;


    uint8_t muzzleLEDs;
    uint8_t muzzleLEDpos[2];
    uint8_t taggerLEDs;
    uint8_t taggerLEDpos[2];
};

#endif