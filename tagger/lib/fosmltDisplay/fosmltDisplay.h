#ifndef fosmltDisplay_h
#define fosmltDisplay_h

#include "Arduino.h"

class fosmltDisplay
{
  public:
    fosmltDisplay();
    void updateAmmo(uint16_t currentAmmo);
    void updateMagazines(uint16_t currentMagazines);
    void updateShield(uint16_t currentShield);
    void updateArmour(uint16_t currentArmour);
    void updateHealth(uint16_t currentHealth);

    void updateTaggerBattery(uint8_t taggerBattery);
    void updateBodyBattery(uint8_t bodyBattery);
    void updateTime();
    void updateConnectionStatus();

    void updateGameTime();
    void showGameSettings(); //what am I doing with this? When Game Starts, game settings should be sent to all taggers?

    void reloadStart();
    void reloadAnimation(uint32_t time, uint16_t reloadTime, uint32_t timeStartedReloading);
    void reloadInterrupted();
    void reloadFinish(uint16_t currentMagazines);
    //void displayHUD(uint16_t currentAmmo,uint16_t currentMagazines,uint16_t currentShield,uint16_t currentHealth);

    void noMagazineInserted();

    void failedToFire();

    void buildTopBarUI(); //battery, time, connected to body or not, body battery
    void buildTaggerUI(uint16_t currentAmmo,uint16_t maxAmmo,uint16_t currentMagazines,uint16_t maxMagazines);
    void buildPlayerUI(uint16_t currentShield,uint16_t maxShield,uint16_t currentArmour,uint16_t maxArmour,uint16_t currentHealth,uint16_t maxHealth);


//do I intend to use these privately 
    void buildAmmoUI(uint16_t currentAmmo,uint16_t maxAmmo);
    void buildMagazineUI(uint16_t currentMagazines,uint16_t maxMagazines);
    void buildShieldUI(uint16_t currentShield,uint16_t maxShield);
    void buildArmourUI(uint16_t currentArmour,uint16_t maxArmour);
    void buildHealthUI(uint16_t currentHealth,uint16_t maxHealth);

    void updateBar(uint16_t current, uint16_t last, uint16_t max, uint8_t pos);
    uint8_t digitLength(uint16_t num);
  private:
    uint16_t ammoLastValue;
    uint16_t magazinesLastValue;
    uint16_t shieldLastValue;
    uint16_t armourLastValue;
    uint16_t healthLastValue;

    uint8_t lastReloadTick;

    uint16_t maxAmmo;
    uint16_t maxMagazines;
    uint16_t maxShield;
    uint16_t maxArmour;
    uint16_t maxHealth;
    //TODO: stat screen with tagger stats, type, silhouette etc
};

#endif
