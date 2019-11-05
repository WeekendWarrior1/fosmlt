//#define lcd16x02
#define tft128x160
//#define dummyDisplay //define if no display

#include "Arduino.h"
#include "fosmltDisplay.h"

#if defined(lcd16x02)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2,8);  // set the LCD address to 0x27 for a 16 chars and 2 line display, and height of char
#endif

#if defined(tft128x160)
#include <TFT_eSPI.h>                 // Include the graphics library (this includes the sprite functions)
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();         // Create object "tft"
const uint16_t background = TFT_BLACK;
const uint8_t vert = 160;
const uint8_t hor = 128;
const uint8_t margin = 4;
const uint8_t barWidth = 6;
const uint8_t fullbarSize = (hor-(margin*2)); //120
//const uint8_t elementSpacing = 2; //spacing for Gs //make this 28 maybe
const uint8_t fontSize = 2;
//uint8_t barPos = 20;
//const uint8_t barSize = (hor-(margin*2)); //120
#define NUMATTRIBUTES 5
//const uint8_t NUMATTRIBUTES = 5;
const char* attributes[NUMATTRIBUTES] = {"Shield","Armour","Health","Ammo","Magazines"};
const uint16_t colours[NUMATTRIBUTES] = {TFT_WHITE,TFT_YELLOW,TFT_RED,TFT_BLUE,TFT_GREEN};
const uint8_t textPos[NUMATTRIBUTES] = {4,32,60,88,116}; //margin + 28
const uint8_t barPos[NUMATTRIBUTES] = {20,48,76,104,134}; //20 + 28
#endif

fosmltDisplay::fosmltDisplay()
{
  #if defined(tft128x160)
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(background);
  ammoLastValue = 0;
  magazinesLastValue = 0;
  armourLastValue = 0;
  shieldLastValue = 0;
  healthLastValue = 0;
  #endif
}

void fosmltDisplay::updateAmmo(uint16_t currentAmmo)
{
  #if defined(lcd16x02)
  if (currentAmmo > 99 && currentAmmo <= 999)                 //Ammo Formatting
  {
    lcd.setCursor(2,0);
    lcd.print(currentAmmo);
  }
  else if (currentAmmo > 9 && currentAmmo <= 99)
  {
    lcd.setCursor(2,0);
    lcd.print(' ');
    lcd.setCursor(3,0);
    lcd.print(currentAmmo);
  }
  else if (currentAmmo <= 9)
  {
    lcd.setCursor(2,0);
    lcd.print("  ");
    lcd.setCursor(4,0);
    lcd.print(currentAmmo);
  }
  #endif
  #if defined(tft128x160)
  updateBar(currentAmmo,ammoLastValue,maxAmmo,3);
  #endif
  ammoLastValue = currentAmmo;
}

void fosmltDisplay::updateMagazines(uint16_t currentMagazines)
{
  #if defined(lcd16x02)
  if (currentMagazines > 99 && currentMagazines <= 999)       //Ammo Formatting
  {
    lcd.setCursor(10,0);
    lcd.print(currentMagazines);
  }
  else if (currentMagazines > 9 && currentMagazines <= 99)
  {
    lcd.setCursor(10,0);
    lcd.print(' ');
    lcd.setCursor(11,0);
    lcd.print(currentMagazines);
  }
  else if (currentMagazines <= 9)
  {
    lcd.setCursor(10,0);
    lcd.print("  ");
    lcd.setCursor(12,0);
    lcd.print(currentMagazines);
  }
  #endif
  #if defined(tft128x160)
  updateBar(currentMagazines,magazinesLastValue,maxMagazines,4);
  #endif
  magazinesLastValue = currentMagazines;
}

void fosmltDisplay::reloadClear()
{
  #if defined(lcd16x02)
  lcd.setCursor(0,0);
  lcd.print("        ");
  lcd.setCursor(8,0);
  lcd.print("        ");
  #endif
  #if defined(tft128x160)
  #endif
}

void fosmltDisplay::reloadAnimation(uint8_t tick)
{
  #if defined(lcd16x02)
  tick--;
  lcd.setCursor(tick,0);
  lcd.print('#');
  #endif
  #if defined(tft128x160)
  #endif
}

void fosmltDisplay::updateShield(uint16_t currentShield)
{
  #if defined(lcd16x02)
  //std::string value = pRemote_BATT_Characteristic->readValue();
  //String batt_caracteristic = value.c_str();

  //std::string shieldsValue = pRemoteCharCurrentShields->readValue();
  //Serial.println(shieldsValue.c_str());
  //unsigned int shieldsInt = shieldsValue;
  //uint8_t shieldsValue = shieldsU
    
  if (shieldsInt > 99 && shieldsInt <= 999)                 //Ammo Formatting
  {
    lcd.setCursor(2,1);
    lcd.print(' ');
  }
  else if (shieldsInt > 9 && shieldsInt <= 99)
  {
    lcd.setCursor(2,1);
    lcd.print(' ');
    lcd.setCursor(3,1);
    //lcd.print(shieldsValue.c_str());
  }
  else if (shieldsInt <= 9)
  {
    lcd.setCursor(2,1);
    lcd.print("  ");
    lcd.setCursor(4,1);
    //lcd.print(shieldsValue.c_str());
  }
  
  #endif
  #if defined(tft128x160)
  updateBar(currentShield,shieldLastValue,maxShield,0);
  #endif
  shieldLastValue = currentShield;
}

void fosmltDisplay::updateArmour(uint16_t currentArmour)
{
  #if defined(tft128x160)
  updateBar(currentArmour,armourLastValue,maxArmour,1);
  #endif
}

void fosmltDisplay::updateHealth(uint16_t currentHealth)
{
  #if defined(lcd16x02)
  /*
  if (pRemoteCharCurrentHealth->getValue() > 99 && pRemoteCharCurrentHealth->getValue() <= 999)                 //Ammo Formatting
  {
    lcd.setCursor(10,1);
    lcd.print(pRemoteCharCurrentHealth->getValue());
  }
  else if (pRemoteCharCurrentHealth->getValue() > 9 && pRemoteCharCurrentHealth->getValue() <= 99)
  {
    lcd.setCursor(10,1);
    lcd.print(' ');
    lcd.setCursor(11,1);
    lcd.print(pRemoteCharCurrentHealth->getValue());
  }
  else if (pRemoteCharCurrentHealth->getValue() <= 9)
  {
    lcd.setCursor(10,1);
    lcd.print("  ");
    lcd.setCursor(12,1);
    lcd.print(pRemoteCharCurrentHealth->getValue());
  }*/
  #endif
  #if defined(tft128x160)
  updateBar(currentHealth,healthLastValue,maxHealth,2);
  #endif
  healthLastValue = currentHealth;
}

void fosmltDisplay::updateBar(uint16_t current, uint16_t last, uint16_t max, uint8_t pos)
{
  printf("current: %i, last: %i, max: %i, pos: %i \n", current,last,max,pos);
  if (current < last)
  {
    tft.fillRect((current*fullbarSize)/max, barPos[pos], (hor-margin), barWidth, background);
  }
    else if (current > last)
  {
    tft.fillRect(margin, barPos[pos], (current*fullbarSize)/max, barWidth, colours[pos]);
  } 
    else if (current == last)
  {
  }
}


void fosmltDisplay::buildTaggerUI(uint16_t currentAmmo,uint16_t maxAmmoRec,uint16_t currentMagazines,uint16_t maxMagazinesRec)
{
  printf("currentAmmo: %i, maxAmmo: %i, currentMagazines: %i, maxMagazines: %i \n", currentAmmo,maxAmmoRec,currentMagazines,maxMagazinesRec);
  uint8_t arrayPos;
  maxAmmo = maxAmmoRec;
  maxMagazines = maxMagazinesRec;
  
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  arrayPos = 3;
  tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  updateAmmo(currentAmmo);
  arrayPos = 4;
  tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  updateMagazines(currentMagazines);
}

void fosmltDisplay::buildPlayerUI(uint16_t currentShield,uint16_t maxShieldRec,uint16_t currentArmour,uint16_t maxArmourRec,uint16_t currentHealth,uint16_t maxHealthRec)
{
  uint8_t arrayPos;
  maxShield = maxShieldRec;
  maxArmour = maxArmourRec;
  maxHealth = maxHealthRec;
  
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  arrayPos = 0;
  tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  updateShield(currentShield);
  arrayPos = 1;
  tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  updateArmour(currentArmour);
  arrayPos = 2;
  tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  updateHealth(currentHealth);
}
/*
void fosmltDisplay::displayHUD(uint16_t currentAmmo,uint16_t currentMagazines,uint16_t currentShield,uint16_t currentHealth)
{
  #if defined(lcd16x02)
  lcd.clear();
  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print("A:");
  lcd.setCursor(8,0); // set the cursor to column 8, line 0
  lcd.print("M:");
  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print("S:");
  lcd.setCursor(8,1); // set the cursor to column 8, line 1
  lcd.print("H:");
  updateAmmo(currentAmmo);
  updateMagazines(currentMagazines);
  //updateShield(currentShield);
  //updateHealth(currentHealth);
  #endif
  #if defined(tft128x160)
    //uint8_t textPos = margin;
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    for(uint8_t i =0; i < NUMATTRIBUTES;i++) {
        tft.drawCentreString(attributes[i],(hor/2),textPos,fontSize);
        //tft.fillRect (margin, barPos, barSize, barWidth, colours[i]);
        //barPos = barPos + 28;
        //textPos = textPos + 28;
        if (attributes[i] == "Magazines") { //add spacing for magazine (because it has a g)
            barPos = barPos + elementSpacing;
        }
    }

    tft.drawCentreString("Shield",(hor/2),textPos,fontSize);
    tft.fillRect (margin, barPos, (hor-margin), barWidth, TFT_WHITE);
    barPos = barPos + 28;
    textPos = textPos + 28;

    //tft.setCursor(2, 28, 2);
    //tft.println("Armour");
    tft.drawCentreString("Armour",(hor/2),textPos,fontSize);
    tft.fillRect (margin, barPos, (hor-margin), barWidth, TFT_YELLOW);
    barPos = barPos + 28;
    textPos = textPos + 28;

    //tft.setCursor(2, 56, 2);
    //tft.println("Health");
    tft.drawCentreString("Health",(hor/2),textPos,fontSize);
    tft.fillRect (margin, barPos, (hor-margin), barWidth, TFT_RED);
    barPos = barPos + 28;
    textPos = textPos + 28;

    //tft.setCursor(2, 84, 2);
    //tft.println("Ammo");
    tft.drawCentreString("Ammo",(hor/2),textPos,fontSize);
    tft.fillRect (margin, barPos, (hor-margin), barWidth, TFT_BLUE);
    barPos = barPos + 28;
    textPos = textPos + 28;

    //tft.setCursor(2, 110, 2);
    //tft.println("Magazines");
    tft.drawCentreString("Magazines",(hor/2),textPos,fontSize);
    tft.fillRect (margin, (barPos+elementSpacing), (hor-margin), barWidth, TFT_GREEN);
    
  #endif
}
*/