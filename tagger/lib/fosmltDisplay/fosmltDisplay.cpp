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
const uint8_t charW = 5;
const uint8_t charH = 8;
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
  uint8_t digits = digitLength(currentAmmo);
  lcd.setCursor(2,0); //we have room for 3 digits, starting at (2,0)
  lcd.print("  ");  //wipe any potential spaces our string might not be long enough to write over
  lcd.setCursor((5-digits),0);  //digits can be 1,2 or 3, meaning cursor is either 4,3 or 2 (perfect!)
  lcd.print(currentAmmo);
  #endif
  #if defined(tft128x160)
  uint8_t attr = 3; //shield's position in all of the UI's arrays
  uint8_t digits = digitLength(currentAmmo);
  uint8_t digitsLast = digitLength(ammoLastValue);  //get the length of the last ammo value, for writing over with a rect later
  updateBar(currentAmmo,ammoLastValue,maxAmmo,attr);  //supports loss or gain of an attribute
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[attr], fontSize); //set the cursor far right padded eg. digit(1)*charW(5)*fontsize(2)=10 padding from margin
  tft.fillRect((hor-margin-(digitsLast*charW*fontSize)), textPos[attr], (hor-margin-(digitsLast*charW*fontSize)), (fontSize*charH), background);  //write over old value using length of last value
  tft.print(currentAmmo);
  #endif
  ammoLastValue = currentAmmo;
}

void fosmltDisplay::updateMagazines(uint16_t currentMagazines)
{
  #if defined(lcd16x02)
  uint8_t digits = digitLength(currentMagazines);
  lcd.setCursor(10,0);
  lcd.print("  ");
  lcd.setCursor((13-digits),0);
  lcd.print(currentMagazines);
  #endif
  #if defined(tft128x160)
  uint8_t attr = 4;
  uint8_t digits = digitLength(currentMagazines);
  uint8_t digitsLast = digitLength(magazinesLastValue);
  updateBar(currentMagazines,magazinesLastValue,maxMagazines,attr);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[attr], fontSize);
  tft.fillRect((hor-margin-(digitsLast*charW*fontSize)), textPos[attr], (hor-margin-(digitsLast*charW*fontSize)), (fontSize*charH), background);
  tft.print(currentMagazines);
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
  uint8_t digits = digitLength(currentShield);
  lcd.setCursor(2,1);
  lcd.print("  ");
  lcd.setCursor((5-digits),1);
  lcd.print(currentShield);
  #endif
  #if defined(tft128x160)
  uint8_t attr = 0;
  uint8_t digits = digitLength(currentShield);
  uint8_t digitsLast = digitLength(shieldLastValue);
  updateBar(currentShield,shieldLastValue,maxShield,attr);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[attr], fontSize);
  tft.fillRect((hor-margin-(digitsLast*charW*fontSize)), textPos[attr], (hor-margin-(digitsLast*charW*fontSize)), (fontSize*charH), background);
  tft.print(currentShield);
  #endif
  shieldLastValue = currentShield;
}

void fosmltDisplay::updateArmour(uint16_t currentArmour)
{
  #if defined(tft128x160)
  uint8_t attr = 1;
  uint8_t digits = digitLength(currentArmour);
  uint8_t digitsLast = digitLength(ammoLastValue);
  updateBar(currentArmour,armourLastValue,maxArmour,attr);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[attr], fontSize);
  tft.fillRect((hor-margin-(digitsLast*charW*fontSize)), textPos[attr], (hor-margin-(digitsLast*charW*fontSize)), (fontSize*charH), background);
  tft.print(currentArmour);
  #endif
  armourLastValue = currentArmour;
}

void fosmltDisplay::updateHealth(uint16_t currentHealth)
{
  #if defined(lcd16x02)
  uint8_t digits = digitLength(currentHealth);
  lcd.setCursor(10,1);
  lcd.print("  ");
  lcd.setCursor((13-digits),1);
  lcd.print(currentHealth);
  #endif
  #if defined(tft128x160)
  uint8_t attr = 2;
  uint8_t digits = digitLength(currentHealth);
  uint8_t digitsLast = digitLength(healthLastValue);
  updateBar(currentHealth,healthLastValue,maxHealth,attr);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[attr], fontSize);
  tft.fillRect((hor-margin-(digitsLast*charW*fontSize)), textPos[attr], (hor-margin-(digitsLast*charW*fontSize)), (fontSize*charH), background);
  tft.print(currentHealth);
  #endif
  healthLastValue = currentHealth;
}

void fosmltDisplay::buildAmmoUI(uint16_t currentAmmo,uint16_t maxAmmo)
{
  uint8_t arrayPos = 3;
  tft.setTextColor(TFT_WHITE,background);
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
  //tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  tft.setCursor(margin, textPos[arrayPos], fontSize);
  tft.print(attributes[arrayPos]);

  uint8_t digits = digitLength(currentAmmo);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[arrayPos], fontSize);
  tft.print(currentAmmo);
  //could build currentAmmo/maxAmmo
}

void fosmltDisplay::buildMagazineUI(uint16_t currentMagazines,uint16_t maxMagazines)
{
  uint8_t arrayPos = 4;
  tft.setTextColor(TFT_WHITE,background);
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
  //tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  tft.setCursor(margin, textPos[arrayPos], fontSize);
  tft.print(attributes[arrayPos]);

  uint8_t digits = digitLength(currentMagazines);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[arrayPos], fontSize);
  tft.print(currentMagazines);
}

void fosmltDisplay::buildShieldUI(uint16_t currentShield,uint16_t maxShield)
{
  uint8_t arrayPos = 0;
  tft.setTextColor(TFT_WHITE,background);
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
  //tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  tft.setCursor(margin, textPos[arrayPos], fontSize);
  tft.print(attributes[arrayPos]);

  uint8_t digits = digitLength(currentShield);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[arrayPos], fontSize);
  tft.print(currentShield);
}

void fosmltDisplay::buildArmourUI(uint16_t currentArmour,uint16_t maxArmour)
{
  uint8_t arrayPos = 1;
  tft.setTextColor(TFT_WHITE,background);
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
  //tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  tft.setCursor(margin, textPos[arrayPos], fontSize);
  tft.print(attributes[arrayPos]);

  uint8_t digits = digitLength(currentArmour);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[arrayPos], fontSize);
  tft.print(currentArmour);
}

void fosmltDisplay::buildHealthUI(uint16_t currentHealth,uint16_t maxHealth)
{
  uint8_t arrayPos = 2;
  tft.setTextColor(TFT_WHITE,background);
  //printf("attributes[arrayPos]: %s, (hor/2): %i, textPos[arrayPos]: %i, fontSize: %i \n", attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize);
  //tft.drawCentreString(attributes[arrayPos],(hor/2),textPos[arrayPos],fontSize); //modify to left soon
  tft.setCursor(margin, textPos[arrayPos], fontSize);
  tft.print(attributes[arrayPos]);

  uint8_t digits = digitLength(currentHealth);
  tft.setCursor((hor-margin-(digits*charW*fontSize)), textPos[arrayPos], fontSize);
  tft.print(currentHealth);
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
uint8_t fosmltDisplay::digitLength(uint16_t num)
{
  uint8_t length = 0;
  if (num == 0) {return 1;} //0 is still one digit
  while (num > 0)
  {
    num = num/10;
    length++;
  }
  return length;
}


void fosmltDisplay::buildTaggerUI(uint16_t currentAmmo,uint16_t maxAmmo,uint16_t currentMagazines,uint16_t maxMagazines)
{
  printf("currentAmmo: %i, maxAmmo: %i, currentMagazines: %i, maxMagazines: %i \n", currentAmmo,maxAmmo,currentMagazines,maxMagazines);
  this->maxAmmo = maxAmmo;
  this->maxMagazines = maxMagazines;
  
  buildAmmoUI(currentAmmo,maxAmmo);
  updateAmmo(currentAmmo);

  buildMagazineUI(currentMagazines,maxMagazines);
  updateMagazines(currentMagazines);
}

void fosmltDisplay::buildPlayerUI(uint16_t currentShield,uint16_t maxShield,uint16_t currentArmour,uint16_t maxArmour,uint16_t currentHealth,uint16_t maxHealth)
{
  this->maxShield = maxShield;
  this->maxArmour = maxArmour;
  this->maxHealth = maxHealth;
  
  buildShieldUI(currentShield,maxShield);
  updateShield(currentShield);

  buildArmourUI(currentArmour,maxArmour);
  updateArmour(currentArmour);

  buildHealthUI(currentHealth,maxHealth);
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