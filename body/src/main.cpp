//HARDWARE

#include <Arduino.h>

#include <fosmltIRReceive.h>

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
//GPIO 15
//GPIO 2

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

/*************************************************************************GAME*/
uint8_t gameState = 2;  //1=development,2=idle,3=endofgame,4=demonstration,5=playing

/******************************************************************BODY+PLAYER*/
char playerName[] = "WeekendWarrior";
//player RBG Colour
uint8_t playerID = 170;  //10101010
uint8_t teamID = 2;  //10
uint8_t currentHealth = 100;
uint8_t maxHealth = 100;
uint8_t currentArmour = 0;
uint8_t maxArmour = 100; //set max armour to max health?
uint8_t currentShields = 0;
uint8_t maxShields = 100;
uint8_t testValue = 140;

bool canIShoot = false;

bool staggered = false;
unsigned int staggerDuration;
unsigned long timeLastStaggered;

/********************************************************************IR_Packet*/
const uint8_t playerIDlength = 8; // Up to 256 players
const uint8_t teamIDlength = 2;  // Up to 4 teams
const uint8_t gunDMGlength = 4;  // Up to 16 different damage values that can be sent
const uint8_t packettotallength = playerIDlength + teamIDlength + gunDMGlength;

/*******************************************************************IR_LENGTHS*/
int IRheader = 2400;   //high
int IR1 = 1200;      //high
int IR0 = 600;       //high
int IRgap = 600;     //low
int IRfreq = 56000;    //IR freq in Hz
//int IRheadermarginerror = 400;    //Margin of error of 400us when recieving IRheader
//int IR1marginerror =  130;        //Margin of error of 130us when recieving IR1
//int IR0marginerror = 50;          //Margin of error of 50us when recieving IR0
fosmltIRReceive irReceiver(IRheader, IR1, IR0, IRgap, IRfreq, playerIDlength, teamIDlength, gunDMGlength, packettotallength);

/***************************************************************GAME VARIABLES*/
//TODO turn into GameVariable Struct
bool GVfriendlyfire;
bool GVshieldactive;
bool GVarmouractive;
bool GVffa;              //ffa = free for all
bool GVpowerups;
bool GVstartingPeace;    //set to 1 if there will be some time at the start of the game where no-one can take damage
unsigned long peaceTime;      //how long the peace at start of game will last     //probably in ms
bool GVtimelimit;
unsigned long gameLength;
bool GVstagger;

unsigned long timeGameStarted;  //time game started

/*****************************************************************DAMAGE TABLE*/
int gunDMGtable[] = {1,2,3,4,5,6,7,8,9,10,20,12,13,14,5,10};
unsigned long staggerTable[] = {100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0}; //in ms, dmgtable specific stagger times

void EventStaggered(int receivedgunDMG);
void EventUnstaggered();
void EventGotShot(int receivedgunDMG);
void EventGotHealed();
void EventBLEConnect();
void EventBLEDisconnect();
void EventGameStart();
void EventGameEnd();
void EventPlayerDeath();
void EventFriendlyFireReceived();

void parseReceivedShot(int receivedplayerID, int receivedteamID, int receivedgunDMG);
//bool canIshootUpdate();

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");

  irReceiver.clearIRbuffer(); //when a game starts, make sure to clear the buffer of any shots received prior
}

void loop() {
  //DacAudio.FillBuffer();
  switch (gameState)
  {
    case 5:
      // statements
      break;
    case 4:
      // statements
      break;
    case 3:
      // statements
      break;
    case 2:
      // statements
      break;
    case 1:
      //Serial.println("gameState1");
      //check for new ble taggers (send data) + connected event
      irReceiver.IRBuffer();
      if (irReceiver.shotReceviedFlag)
      {
        Serial.println("Received Packet");
        Serial.println(irReceiver.receivedShot.playerID);
        Serial.println(irReceiver.receivedShot.teamID);
        Serial.println(irReceiver.receivedShot.gunDMG);
        //parse packet, then do something with it
        irReceiver.shotReceviedFlag = false;
        parseReceivedShot(irReceiver.receivedShot.playerID,irReceiver.receivedShot.teamID,irReceiver.receivedShot.gunDMG);
      }
      if (staggered)
      {
        if ((staggerDuration + timeLastStaggered) >= millis())
        {
          EventUnstaggered();
        }
      }
      if (GVtimelimit)
      {
        if(timeGameStarted + gameLength >= millis())
        {
          EventGameEnd();
        }
      }
      //currentHealth only required here if processing damage ticks from DamageoverTime, but then that could be managed within there
      break;
  }
}

void EventStaggered(int receivedgunDMG)
{
  Serial.println("EventStaggered");
  if ((millis() - timeLastStaggered) > staggerTable[receivedgunDMG])
  {
    timeLastStaggered = millis();
    staggerDuration = staggerTable[receivedgunDMG];
    staggered = 1;
    canIShoot = 0; //notify tagger
  }
}

void EventUnstaggered()
{
  Serial.println("EventUnstaggered");
  staggered = 0;
  canIShoot = 1; //notify tagger  //should be && with other reasons why I couldn't shoot
}

void EventGotShot(int receivedgunDMG)
{
  Serial.println("EventGotShot");
  //update display over BLE
  Serial.print(currentHealth);
  //delay(100);
  //play sound
  //hit LEDs flash    need a non blocking LED library for all flashes
  if (GVstagger && currentHealth > 0)
  {
    EventStaggered(receivedgunDMG);
  }
}

void EventGotHealed()
{
  Serial.println("EventGotHealed");
  //update display over BLE
  //play sound
  //hit LEDs flash    need a non blocking LED library for all flashes
}

void EventBLEConnect()
{
  Serial.println("EventBLEConnect");
  //play Sound
  //send BT variables

}

void EventBLEDisconnect()
{
  Serial.println("EventBLEDisconnect");
  //play sound
  // disconnecting
}

void EventGameStart()
{
  Serial.println("EventGameStart");
  //Should initialise game here,
  //if receive espnow packet that broadcasts game information,
  //single run function set up game
  //set these values from espnow game b

  // GAME VARIABLES
  GVfriendlyfire = 0;     // naming idea : GVfriendlyfire or GVff (Game Variable)
  GVshieldactive = 0;
  GVarmouractive = 0;
  GVffa = 0;              //ffa = free for all
  GVpowerups = 0;
  GVstartingPeace = 0;    //set to 1 if there will be some time at the start of the game where no-one can take damage
  peaceTime = 0;      //how long the peace at start of game will last     //probably in ms
  GVtimelimit = 0;
  gameLength = 0;
  GVstagger = 1;

  currentHealth = maxHealth;
  if (GVshieldactive)
  {
    currentShields = maxShields;
  }
  if (GVarmouractive)
  {
    currentArmour = maxArmour;
  }

  //also receive current time so every body is synced
  //and at what time the game will start

  //create IR reciever instance (have to be global? or else it's scope will just be this idle loop??)
  //fine to be global, can clear IR buffer when game starts

  //then change game state to 1
  //play Game starting sound
  timeGameStarted = millis();
  gameState = 1;
  canIShoot = 1;  //notify tagger
}

void EventGameEnd()
{
  Serial.println("EventGameEnd");
  //play sound gameOver
  //update display with gameStats
  gameState=3;
  canIShoot = 0;
}

void EventPlayerDeath()
{
  Serial.println("EventPlayerDeath");
  //play sound player death
  //update display with death stats
  gameState=3;
  canIShoot = 0;
}

void EventFriendlyFireReceived()
{
  Serial.println("EventFriendlyFireReceived");
  //Display Flash?
  //Play Gentle sound?
}

/*********************************************************************PARSE IR*/
//Logic is really "What do I do with the recieved shot?" since shot could be garbage, could be friendly fire, could be player shooting themselves, or could be a heal
void parseReceivedShot(int receivedplayerID, int receivedteamID, int receivedgunDMG)	//check for friendly fire, deal damage, etc  Thisfunc does shield,armour,health
{
  Serial.println("receivedgunDMG");
  Serial.println(receivedgunDMG);
  if ((GVffa == 0 && receivedteamID == teamID && GVfriendlyfire == 1)) //|| receivedplayerID == playerID)   //if not ffa, and received packet contains same teamID or playerID as player, it's friendlyfire  //can let player shoot themselves if we want
  {
    EventFriendlyFireReceived();
  }
  else if (receivedgunDMG >= 14 && receivedplayerID != playerID)  //lets say that 14 and 15 in gunDMG table are heals?
  {
    if ((currentHealth + gunDMGtable[receivedgunDMG]) > maxHealth)
    {
      currentHealth = maxHealth;
    }
    else
    {
      currentHealth = currentHealth + gunDMGtable[receivedgunDMG];
    }
    EventGotHealed();
  }
  else          //damage should flow shield > armour > health
  {
    if (gunDMGtable[receivedgunDMG] <= currentShields) //if the damage doesn't pierce shields, then minus from shields
    {
      currentShields = currentShields - gunDMGtable[receivedgunDMG];
      Serial.println("currentshields (doesn't pierce)");
      Serial.println(currentShields);
    }
    else    //if the damage does pierce shields, figure out by how much to take from armour
    {
      int shieldoverflow;
      shieldoverflow = -currentShields + gunDMGtable[receivedgunDMG];
      Serial.println("shieldoverflow (does pierce)");
      Serial.println(shieldoverflow);
      if (shieldoverflow <= currentArmour)  //if the damage pierces shields but doesn't pierce armour, drop shields to 0, and minus leftover from armour
      {
        currentShields = 0;
        currentArmour = currentArmour - shieldoverflow;
        Serial.println("currentArmour (doesn't pierce)");
        Serial.println(currentArmour);
      }
      else    //if the damage pierces shields, and armour, then find out by how much to take from health
      {
        int armouroverflow;
        armouroverflow = - currentArmour + shieldoverflow;
        Serial.println("armouroverflow (does pierce)");
        Serial.println(armouroverflow);
        if (armouroverflow >= currentHealth)    //if the overflow would bring health to, or below 0, set to 0
        {
          currentHealth = 0;
        }
        else
        {
          currentHealth = currentHealth - armouroverflow;
          Serial.println("currentHealth");
          Serial.println(currentHealth);
        }
      }
    }
    EventGotShot(receivedgunDMG);
    if (currentHealth == 0)
    {
      EventPlayerDeath();
    }
  }
}

/*bool canIshootUpdate();
{
  return !staggered && 
}*/
