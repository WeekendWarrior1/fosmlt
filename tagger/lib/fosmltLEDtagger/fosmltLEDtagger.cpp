//#define fastled //any supported by the fastLED, https://github.com/FastLED/FastLED#supported-led-chipsets
//https://github.com/FastLED/FastLED/issues/875 skipping fastled for now
#define neopixel
//define LED //single colour LED
//#define 4legRGB
//#define dummyLED //define if no LEDs

/*things that can be done with RGB
On tagger:
  overheating status
  Firing animation
  muzzle flash
  breathing animation
  Empty mag
  Magazine capacity
  connected to body animation
On Body:
  breathing / aura
  Team colour for night games
  flash when shot
  death animation? (blink rapidly and fade out?)
  game Start animation
  Power up animation
  Health Status?
  Shield status? (strength of colour based on shield)

  LEDs on back of body, dead space style Health
  ALSO, consider a interactive mini game, 1dimensional pong style (also GearsofWar active reload mini game),
  support class to give power up
  minigames and skill testing
*/

#include "fosmltLEDtagger.h"
#include <Arduino.h>

//should seperate muzzle and rest of tagger
//aim to run light animations at 25fps -> so once every 40ms (how does this fit in to tagger timeTakenToFire?)

#define NUM_LEDS 4
#define LED_CHIP WS2812B
#define DATA_PIN 4
#define taggerAnimationDefine 500
#if defined(fastled)
//#include "FastLED.h"
//#include <SPI.h>
uint8_t max_bright = 255;
//#FASTLED_USING_NAMESPACE
CRGB leds[NUM_LEDS];
#endif
#if defined(neopixel)
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
#endif


fosmltLEDtagger::fosmltLEDtagger(uint8_t muzzleLEDs, uint8_t muzzleLEDpos[],uint8_t taggerLEDs, uint8_t taggerLEDpos[])
{
  this->muzzleLEDs = muzzleLEDs;
  for (int i = 0; i < (muzzleLEDs);i++)
  {
    this->muzzleLEDpos[i] = muzzleLEDpos[i];
  }
  this->taggerLEDs = taggerLEDs;
  for (int i = 0; i < (taggerLEDs);i++)
  {
    this->taggerLEDpos[i] = taggerLEDpos[i];
  }
  this->taggerAnimationTime = taggerAnimationDefine;
  #if defined(fastled)
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(max_bright);
    leds[0].setRGB(0,0,0);;
    FastLED.show();
  #endif
  #if defined(neopixel)
    pixels.begin();
    pixels.clear();
  #endif
}

void fosmltLEDtagger::muzzleFlash()
{
  this->lastFadeTick = 0;
  this->lastTaggerAnimationTick = 0;  //doesn't make much sense to put inside the muzzleflash function, perhaps a func rename is in order
  #if defined(fastled)
    for (int i = 0; i < (NUM_LEDS);i++)
    {
      leds[i].setRGB(255,0,0);
    }
    FastLED.show();
  #endif
  #if defined(neopixel)
    for (int i = 0; i < (muzzleLEDs);i++)
    {
      pixels.setPixelColor(muzzleLEDpos[i], pixels.Color(255, 255, 255));
    }
    pixels.setPixelColor(taggerLEDpos[0], pixels.Color(255, 255, 255)); //front
    pixels.setPixelColor(taggerLEDpos[1], pixels.Color(0, 0, 0));
    pixels.show();
  #endif
}

void fosmltLEDtagger::muzzleFade(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot)
{
  static uint8_t fadeDivisions = ((muzzleFlashTime*25)/1000); //25 frames per second
  static uint8_t fadeIncrements = muzzleFlashTime/fadeDivisions;
  uint8_t tick = (time-timeLastshot)/fadeIncrements;
  if (tick > lastFadeTick)
  {
    uint8_t fader = ((256*tick)/fadeDivisions)-1;
    lastFadeTick = tick;
    if (tick == fadeDivisions) {fader = 255;} //always 0 by last tick
    //printf("fadeDivisions: %i, fadeIncrements: %i, tick: %i, fader: %i \n",fadeDivisions,fadeIncrements,tick,fader);
    #if defined(fastled)
      leds[0].fadeToBlackBy(fader);;
      FastLED.show();
    #endif
    #if defined(neopixel)
      for (int i = 0; i < (muzzleLEDs);i++)
      {
        pixels.setPixelColor(muzzleLEDpos[i], pixels.Color((255-fader), (255-fader), (255-fader)));
      }  
      pixels.show();
    #endif
  }
}

void fosmltLEDtagger::taggerAnimation(uint32_t time, uint32_t timeLastshot)
{
  //this animation works in 3 segments
  //1. frontLED lights up
  //2. frontLED starts fading whilst backLED starts lighting up
  //3 backLED fades
  static uint8_t animationDivisions = ((taggerAnimationTime*25)/1000); //25 frames per second
  static uint8_t animationIncrements = taggerAnimationTime/animationDivisions;
  static uint32_t segementTime = taggerAnimationTime/3;
  static uint8_t segmentTicks = animationDivisions/3;

  uint8_t tick = (time-timeLastshot)/animationIncrements;
  if (tick > lastTaggerAnimationTick)
  {
    uint8_t segment = ((time-timeLastshot)/segementTime)+1;
    //printf("animationDivisions: %i, animationIncrements: %i, segementTime: %i \n",animationDivisions,animationIncrements,segementTime);
    //printf("segmentTicks: %i, segment: %i, tick: %i \n",segmentTicks,segment,tick);
    if (segment == 1)
    {
      #if defined(fastled)
        leds[0].fadeToBlackBy(fader);;
        FastLED.show();
      #endif
      #if defined(neopixel)
        //already set in muzzleflash animation (should be renamed to startFiringAnimation) 
        //pixels.setPixelColor(taggerLEDpos[0], pixels.Color(255, 255, 255)); //front
        //pixels.show();
      #endif
    }
    if (segment == 2)
    {
      uint8_t fader = ((256*tick)/segmentTicks)-1;
      #if defined(fastled)
        leds[0].fadeToBlackBy(fader);;
        FastLED.show();
      #endif
      #if defined(neopixel)
        pixels.setPixelColor(taggerLEDpos[0], pixels.Color(255-fader, 255-fader, 255-fader)); //front
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(fader, fader, fader)); //back
        pixels.show();
      #endif
    }
    if (segment == 3)
    {
      uint8_t fader = ((128*tick)/segmentTicks)-1;
      if (tick == animationDivisions) {fader = 128;} //always 0 by last tick
      #if defined(fastled)
        leds[0].fadeToBlackBy(fader);;
        FastLED.show();
      #endif
      #if defined(neopixel)
        //pixels.setPixelColor(taggerLEDpos[0], pixels.Color(0, 0, 0)); //front
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(128-fader, 128-fader, 128-fader)); //back
        pixels.show();
      #endif
    }
    lastTaggerAnimationTick = tick;
  }
}

void fosmltLEDtagger::startReloadAnimation()
{
  lastReloadAnimationTick = 0;
}

void fosmltLEDtagger::reloadingAnimation(uint32_t time, uint16_t reloadTime, uint32_t timeStartedReloading)
{
  //grow,pulse back and forth between front and back tagger leds, then fade (1,2,3,4,5,6)
  //grow from 0 - 255 as it reloads
  static uint8_t animationDivisions = ((reloadTime*25)/1000); //25 frames per second
  static uint8_t animationIncrements = reloadTime/animationDivisions;
  //static uint32_t segementTime = reloadTime/8;
  //static uint8_t segmentTicks = animationDivisions/8;

  uint8_t tick = (time-timeStartedReloading)/animationIncrements;
  if (tick > lastReloadAnimationTick)
  {
    /*uint8_t segment = ((time-timeStartedReloading)/segementTime)+1;
    #if defined(neopixel)
      if (segment == 1)
      {
        uint8_t fader = ((256*tick)/segmentTicks)-1;
        pixels.setPixelColor(taggerLEDpos[0], pixels.Color(fader, fader, fader)); //back
        pixels.show();
      }
      if ((segment == 2) || (segment == 3))
      {
        uint8_t fader = ((256*tick)/(segmentTicks*2))-1;
        pixels.setPixelColor(taggerLEDpos[0], pixels.Color(255-fader, 255-fader, 255-fader)); //front
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(fader, fader, fader)); //back
        pixels.show();

      }
      if ((segment == 4) || (segment == 5))
      {
        uint8_t fader = ((256*tick)/(segmentTicks*2))-1;
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(255-fader, 255-fader, 255-fader)); //front
        pixels.setPixelColor(taggerLEDpos[0], pixels.Color(fader, fader, fader)); //back
        pixels.show();

      }
      if ((segment == 6) || (segment == 7))
      {
        uint8_t fader = ((256*tick)/(segmentTicks*2))-1;
        pixels.setPixelColor(taggerLEDpos[0], pixels.Color(255-fader, 255-fader, 255-fader)); //front
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(fader, fader, fader)); //back
        pixels.show();

      }
      if (segment == 8)
      {
        uint8_t fader = ((256*tick)/segmentTicks)-1;
        if (tick == animationDivisions) {fader = 255;} //always 0 by last tick
        pixels.setPixelColor(taggerLEDpos[1], pixels.Color(255-fader, 255-fader, 255-fader)); //front
        pixels.show();
      }
    #endif
    lastReloadAnimationTick = tick;*/

    //animation2
    uint8_t fader = ((256*tick)/animationDivisions)-1;
    lastReloadAnimationTick = tick;
    if (tick == animationDivisions) {fader = 0;} //back to 0 by last tick
    printf("animationDivisions: %i, animationIncrements: %i, tick: %i, fader: %i \n",animationDivisions,animationIncrements,tick,fader);
    #if defined(fastled)
      leds[0].fadeToBlackBy(fader);;
      FastLED.show();
    #endif
    #if defined(neopixel)
      for (int i = 0; i < (taggerLEDs);i++)
      {
        pixels.setPixelColor(taggerLEDpos[i], pixels.Color((fader), (fader), (fader)));
      }
      if (tick == animationDivisions) {pixels.setPixelColor(taggerLEDpos[1], pixels.Color(0, 0, 0));}
      pixels.show();
    #endif
  }
}

void fosmltLEDtagger::reloadInterrupted()
{
    #if defined(fastled)
      leds[0].fadeToBlackBy(fader);;
      FastLED.show();
    #endif
    #if defined(neopixel)
      for (int i = 0; i < (taggerLEDs);i++)
      {
        pixels.setPixelColor(taggerLEDpos[i], pixels.Color(0,0,0));
      }  
      pixels.show();
    #endif
}
