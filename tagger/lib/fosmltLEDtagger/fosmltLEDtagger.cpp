//#define fastled //any supported by the fastLED, https://github.com/FastLED/FastLED#supported-led-chipsets
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

#define NUM_LEDS 1
#define LED_CHIP WS2812B
#define DATA_PIN 4
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


fosmltLEDtagger::fosmltLEDtagger()
{
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
  #if defined(fastled)
    leds[0].setRGB(255,0,0);;
    FastLED.show();
  #endif
  #if defined(neopixel)
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
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
    uint8_t fader = (256*tick)/fadeDivisions;
    lastFadeTick = tick;
    if (tick == fadeDivisions) {fader = 255;} //always 0 by last tick
    //printf("fadeDivisions: %i, fadeIncrements: %i, tick: %i, fader: %i \n",fadeDivisions,fadeIncrements,tick,fader);
    #if defined(fastled)
      leds[0].fadeToBlackBy(fader);;
      FastLED.show();
    #endif
    #if defined(neopixel)
      pixels.setPixelColor(0, pixels.Color((255-fader), 0, 0));
      pixels.show();
    #endif
  }
}