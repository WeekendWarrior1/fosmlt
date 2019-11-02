//HARDWARE

//#include <LiquidCrystal.h>    //save this for later, this library looks nice

#include <Arduino.h>

/********************************************************************GPIO PINS*/
//(anti-clockwise around chip, starting at top left corner)
//GPIO 36
//GPIO 39
//GPIO 34
//GPIO 35
//GPIO 32
//GPIO 33
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
//GPIO 4 
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

/********************************************************************IR_Packet*/
const uint8_t playerIDlength = 8; // Up to 256 players
const uint8_t teamIDlength = 2;  // Up to 4 teams
const uint8_t gunDMGlength = 4;  // Up to 16 different damage values that can be sent
const uint8_t packetTotalLength = playerIDlength + teamIDlength + gunDMGlength;

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
uint16_t muzzleFlashTime = 250;

unsigned long timeLastshot = 0;

bool reloading = false;
unsigned long timeStartedReloading;
const bool reloadInteruptable = true;

void EventBLEConnect();
void EventBLEDisconnect();
void EventBodyShot();
void EventFiredTagger();
void EventOutOfAmmo();
void EventReloading();
void EventReloaded();
void EventInteruptReload();


void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  pinMode(reload, INPUT_PULLUP);
  pinMode(trigger, INPUT_PULLUP);

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
            //tagger.IRTransmit();
            EventFiredTagger();
          } else {
            Serial.println("Firing");
            //tagger.IRTransmit();
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
  if (reloading)
  {
    if (millis() >= (reloadTime + timeStartedReloading))
    {
      EventReloaded();
    }
  }
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
  currentAmmo--;
  timeLastshot = millis();
  //DacAudio.Play(&shoot,true);
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
  if (!reloadInteruptable)
  {
    canIshoot = false;
  }
//display
}

void EventReloaded()
{
  Serial.println("EventReloaded");
  //sound
  currentMagazines--;
  currentAmmo = maxAmmo;
  canIshoot = true;
  reloading = 0;
  Serial.print("Tagger reloaded, Magazines: ");
  Serial.print(currentMagazines);
}

void EventReloadInterrupted()
{
  Serial.println("EventReloadInterrupted");
  reloading = false;
}