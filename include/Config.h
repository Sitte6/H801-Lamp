#ifndef H_CONFIG
#define H_CONFIG


#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>



  struct configData
  {
    unsigned int checksum;
    int valid;                          // 0=no configuration, 1=valid configuration
    char SSID[32];                      // SSID of WiFi
    char password[32];                  // Password of WiFi

    int mode;                           // Current Lamp mode: White / RGB / Fade
    int colortemperature;               // Current color temperature
    float brightness;                   // Current brightness: 0.1 - 1
    int color;                          // Current RGB color
    int fadespeed;                          // Current Fadespeed (Duration per Color in ms)

    bool configchanged;                 // States if config has been changed
  };

  typedef struct configData configData;


void eraseConfig(configData cfg);

void saveConfig(configData cfg);

void loadConfig(configData* cfg);

unsigned int CalculateChecksum(configData configdata);

#endif