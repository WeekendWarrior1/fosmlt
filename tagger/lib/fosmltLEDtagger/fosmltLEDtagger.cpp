#define addressableLED //any supported by the fastLED, https://github.com/FastLED/FastLED#supported-led-chipsets
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

//#if defined(addressableLED)
#include "FastLED.h"
#define NUM_LEDS 1
#define LED_CHIP WS2812B
#define DATA_PIN 4
uint8_t max_bright = 255;                                      // Overall brightness definition. It can be changed on the fly, i.e. with a potentiometer.
//#FASTLED_USING_NAMESPACE
CRGB leds[NUM_LEDS];
//#endif

fosmltLEDtagger::fosmltLEDtagger()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  //FastLED.addLeds<LED_CHIP, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(max_bright);                          // You can change the overall brightness on the fly, i.e. with a potentiometer.
}

void fosmltLEDtagger::blinkRed()
{
  for (int i=0; i< NUM_LEDS;i++)
  {
    leds[i].setRGB(255,0,0);
  }
  FastLED.show();
  delay(100);
  printf("End of delay\n");
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
// Now turn the LED off, then pause
  for (int i=0; i< NUM_LEDS;i++)
  {
    leds[i].setRGB(255,0,0);
  }
  FastLED.show();
}

void fosmltLEDtagger::muzzleFlash()
{
  this->lastFadeTick = 0;
  leds[0].setRGB(255,0,0);
  FastLED.show();
}

void fosmltLEDtagger::muzzleFade(uint32_t time, uint16_t muzzleFlashTime, uint32_t timeLastshot)
{
  static uint8_t fadeDivisions = ((muzzleFlashTime*25)/1000);//(((muzzleFlashTime*25)/1000)*256)/((time-timeLastshot)); //25 frames per second == 18 atm
  static uint8_t fadeIncrements = muzzleFlashTime/fadeDivisions;
  uint8_t tick = (time-timeLastshot)/fadeIncrements;
  if (tick > lastFadeTick)
  {
    uint8_t fader = (256*tick)/fadeDivisions;
    //printf("fadeDivisions: %i, fadeIncrements: %i, tick: %i, fader: %i \n",fadeDivisions,fadeIncrements,tick,fader);
    leds[0].fadeToBlackBy(fader);
    lastFadeTick = tick;
    FastLED.show();
  }
}