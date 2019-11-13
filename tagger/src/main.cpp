//HARDWARE

//#include <LiquidCrystal.h>    //save this for later, this library looks nice
//8000Hz sampling rate gives you 125 seconds of audio per MB

#include <Arduino.h>

#include <fosmltDisplay.h>

//#define FASTLED_ESP32_I2S true
//#define FASTLED_RMT_BUILTIN_DRIVER
//#define FASTLED_RMT_BUILTIN_DRIVER true
//#define FASTLED_RMT_MAX_CHANNELS 7
#include <fosmltLEDtagger.h>

#include <fosmltIRSend.h>
/********************************************************************GPIO PINS*/
//(anti-clockwise around chip, starting at top left corner)
//GPIO 36
//GPIO 39
//GPIO 34
//GPIO 35
//GPIO 32
//GPIO 33 IR_LED
//GPIO 25
//GPIO 26
//GPIO 27
//GPIO 14
//GPIO 12

//GPIO 13 
//GPIO 9
//GPIO 10
//GPIO 11
//GPIO 6
//GPIO 7
//GPIO 8
//GPIO 15 reload
//GPIO 2 trigger

//GPIO 0 
//GPIO 4 WS2812B RGB LED
//GPIO 16
//GPIO 17 
//GPIO 5 
//GPIO 18 
//GPIO 19
//GPIO 21
//GPIO 3
//GPIO 1
//GPIO 22
//GPIO 23 

#define reload 15
#define trigger 2
//#define IR_LED GPIO_NUM_33 //defining in fosmltIRSend

/********************************************************************IR_Packet*/
const uint8_t playerIDlength = 8; // Up to 256 players
const uint8_t teamIDlength = 2;  // Up to 4 teams
const uint8_t gunDMGlength = 4;  // Up to 16 different damage values that can be sent
const uint8_t packetTotalLength = playerIDlength + teamIDlength + gunDMGlength;

/****************************************************************Magazine Type*/
#define variableMagazine //eg Magazines stored in a variable on the tagger, resets on power reset (not recommended if player can access tagger power)
//#define eepromMagazine //store and read ammo and Magazines in eeprom, meaning it remains after a power reset
//define physicalMagazine //ammo is stored in the memory of an external magazine

/******************************************************************Reload Type*/
#define buttonReload
//#define rechargingBattery
//define physicalReload //external magazine is removed and replaced

//#define boltActionButton //could also include any weapon that requires cocking,
// etc. Can be defined alongside other reload types
//bool cocked is required to fire
//bool cocked is required after reload

/******************************************************************Firing Type*/
#define fullAutomatic

//#define semiAutomatic
//requires removal of fire rate, requires debounce

//#define rampingFireRate
//automatic with decreasing time between shots

//#define chargingFire
//fire and release below a certain time = standard shot
//else charge to a maximum amount over a set time
//increasing damage is hard with our table
//fires on trigger release

//define overHeating
//could be used alongside other firerates, requires logic for  gradually cooling down weapon

/***********************************************************************Tagger*/
bool canIshoot = false;

uint16_t gunDMG = 10;
uint16_t currentAmmo = 20;
uint16_t maxAmmo = 20;
uint16_t currentMagazines = 10;
uint16_t maxMagazines = 10;
uint16_t reloadTime = 1600;
uint16_t RPM = 120;
unsigned long IRShotDelay = 60000/RPM; //convert roundsPerMinute into a millisecond delay between shots
                                          //change into genROF func later on
uint16_t muzzleFlashTime = 250; //TODO doesn't work nicely with muzzleAnimation below 250

unsigned long timeLastshot = 0;

bool reloading = false;
uint32_t timeStartedReloading;
const bool reloadInterruptable = true;

void EventBLEConnect();
void EventBLEDisconnect();
void EventBodyShot();
void EventFiredTagger();
void EventOutOfAmmo();
void EventReloading();
void EventReloaded();
void EventReloadInterrupted();
void scheduler(uint32_t time);

//To be replace with bluetooth received variables
  int IRheader = 2400; //high
  int IR1 = 1200;      //high
  int IR0 = 600;       //high
  int IRgap = 600;     //low
  int IRfreq = 38000;  //IR freq in Hz

  int playerID = 170;
  int teamID = 2;
  //int gunDMG = gunDMG;

  fosmltIRSend tagger;

/**********************************************************************Display*/
  fosmltDisplay display;    //definitions done in fosmltDisplay.cpp && if tft display, platformio.ini buildflags

/**************************************************************************LED*/
  fosmltLEDtagger LED;    //definitions done in fosmltLEDtagger.cpp, platformio.ini buildflags
void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  pinMode(reload, INPUT_PULLUP);
  pinMode(trigger, INPUT_PULLUP);

  //To be replace with bluetooth received variables
  /*IRheader = 2400; //high
  IR1 = 1200;      //high
  IR0 = 600;       //high
  IRgap = 600;     //low
  IRfreq = 56000;  //IR freq in Hz

  playerID = 170;
  teamID = 2;
  //gunDMG = gunDMG; */
  canIshoot = true;

  //ble connection assumed below this point
  Packet shotPacket;
  shotPacket.playerID = playerID;
  shotPacket.teamID = teamID;
  shotPacket.gunDMG = gunDMG;
  tagger.attach(shotPacket,IRheader,IR1,IR0,IRgap,IRfreq,playerIDlength,teamIDlength,gunDMGlength,packetTotalLength);

  display.buildTaggerUI(currentAmmo,maxAmmo,currentMagazines,maxMagazines);
  display.buildPlayerUI(100,100,100,100,100,100);

  delay(muzzleFlashTime); //super ugly fix for our animation scheduler, figure out how to do this nicely
              //would break in the first muzzleFlash time because of the check
              //using fadetoblack solves this, but is that okay?
              //definelty not okay to leave this problem, was breaking whole scheduler
}

void loop() {
  //DacAudio.FillBuffer();
  if(digitalRead(trigger) == LOW)
  {
    if(canIshoot)// && currentAmmo > 0 && (millis() - timeLastshot) >= IRShotDelay)//&& BLEConnected)
    {
      if (currentAmmo > 0)
      {
        if ((millis() - timeLastshot) >= IRShotDelay)
        {
          if (reloading)
          {
            EventReloadInterrupted();
            Serial.println("Firing");
            tagger.IRTransmit();
            EventFiredTagger();
          } else {
            Serial.println("Firing");
            tagger.IRTransmit();
            EventFiredTagger();
          }
        }
      }
    }
    else if (currentAmmo == 0)
    {
      EventOutOfAmmo();
    }
  }
  if(digitalRead(reload) == LOW)
  {
    if(currentAmmo < maxAmmo && currentMagazines > 0 && !reloading)
    {
      EventReloading();
    }
  }
  scheduler(millis());
}

/*************************************************************************EVENTS
*******************************************************************************/
void EventBLEConnect()
{
  Serial.println("EventBLEConnect");
  //DacAudio.Play(&connected,true);
  //receive BT variables

}

void EventBLEDisconnect()
{
  Serial.println("EventBLEDisconnect");
  //play sound
}

void EventBodyShot()
{
  Serial.println("EventBodyShot");
  //update display health and shield
}

void EventFiredTagger()
{
  Serial.println("EventFiredTagger");
  // set alarm for totalTimeToFire ms from now to start muzzle flash
  currentAmmo--;
  timeLastshot = millis();
  //DacAudio.Play(&shoot,true);
  display.updateAmmo(currentAmmo);
  LED.muzzleFlash();
  Serial.print("Shot fired, Ammo: ");
  Serial.print(currentAmmo);
  Serial.print('\n');
}

void EventOutOfAmmo()
{
  Serial.println("EventOutOfAmmo");
  //DacAudio.Play(&emptyMag,true);
  timeLastshot = millis();
  //display flash
}

void EventReloading()
{
  Serial.println("EventReloading");
  //DacAudio.Play(&reload,true);
  timeStartedReloading = millis();
  reloading = true;
  if (!reloadInterruptable)
  {
    canIshoot = false;
  }
  display.reloadStart();
}

void EventReloaded()
{
  Serial.println("EventReloaded");
  //sound
  currentMagazines--;
  currentAmmo = maxAmmo;
  canIshoot = true;
  reloading = 0;
  display.updateAmmo(currentAmmo);
  display.reloadFinish(currentMagazines);
  Serial.print("Tagger reloaded, Magazines: ");
  Serial.print(currentMagazines);
  Serial.print('\n');
}

void EventReloadInterrupted()
{
  Serial.println("EventReloadInterrupted");
  reloading = false;
  display.reloadInterrupted();
}

void scheduler(uint32_t time)  //to be replaced with esp alarms and interupts
{
  if (reloading)
  {
    display.reloadAnimation(time,reloadTime,timeStartedReloading);
    if (time >= (reloadTime + timeStartedReloading))
    {
      EventReloaded();
    }
  }
  if (time < (timeLastshot+muzzleFlashTime))
  {
    LED.muzzleFade(time, muzzleFlashTime, timeLastshot);
  }
}