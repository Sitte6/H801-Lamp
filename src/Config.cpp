
#include "Config.h"


int cfgStart = 10;

//EEPROM Config
void eraseConfig(configData cfg) {
  
  // Reset EEPROM bytes to '0' for the length of the data structure
  Serial.println("Erasing Config from Flash...");
  EEPROM.begin(4095);
  for (unsigned int i = cfgStart ; i < sizeof(cfg) ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
  Serial.println("Flash storage erased!");
}

void saveConfig(configData cfg) {
  
  //Calculate Checksum of config
  unsigned int checksum = CalculateChecksum(cfg);
  cfg.checksum=checksum;
  // Save configuration from RAM into EEPROM
  Serial.println("Writing Data to Flash...");
  EEPROM.begin(4095);
  EEPROM.put( cfgStart, cfg );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
  Serial.println("Config stored in Flash!");
}




void loadConfig(configData* cfg) {
  
  // Loads configuration from EEPROM into RAM
  Serial.println("Loading Config from Flash...");
  EEPROM.begin(4095);
  EEPROM.get( cfgStart, *cfg );
  EEPROM.end();
  Serial.println("Config loaded!");
}

unsigned int CalculateChecksum(configData configdata)
{
  unsigned int sum = 0;
  unsigned char *p = (unsigned char *)&configdata.SSID;
  for (unsigned int i = 0; i < sizeof(configdata.SSID); i++) {
    sum += p[i];
  }
  return sum;
}