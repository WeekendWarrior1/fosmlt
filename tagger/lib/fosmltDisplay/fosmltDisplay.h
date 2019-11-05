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

    void reloadClear();
    void reloadAnimation(uint8_t tick);
    //void displayHUD(uint16_t currentAmmo,uint16_t currentMagazines,uint16_t currentShield,uint16_t currentHealth);

    void noMagazine();

    void failedToFire();

    void buildTopBarUI(); //battery, time, connected to body or not, body battery
    void buildTaggerUI(uint16_t currentAmmo,uint16_t maxAmmoRec,uint16_t currentMagazines,uint16_t maxMagazinesRec);
    void buildPlayerUI(uint16_t currentShield,uint16_t maxShieldRec,uint16_t currentArmour,uint16_t maxArmourRec,uint16_t currentHealth,uint16_t maxHealthRec);

    void buildAmmoUI(uint16_t currentAmmo,uint16_t maxAmmoRec);
    void buildMagazineUI(uint16_t currentMagazines,uint16_t maxMagazinesRec);
    void buildShieldUI(uint16_t currentShield,uint16_t maxShieldRec);
    void buildArmourUI(uint16_t currentArmour,uint16_t maxArmourRec);
    void buildHealthUI(uint16_t currentHealth,uint16_t maxHealthRec);

    void updateBar(uint16_t current, uint16_t last, uint16_t max, uint8_t pos);
  private:
    uint16_t ammoLastValue;
    uint16_t magazinesLastValue;
    uint16_t shieldLastValue;
    uint16_t armourLastValue;
    uint16_t healthLastValue;

    uint16_t maxAmmo;
    uint16_t maxMagazines;
    uint16_t maxShield;
    uint16_t maxArmour;
    uint16_t maxHealth;
    //TODO: stat screen with tagger stats, type, silhouette etc
};

#endif