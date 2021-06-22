#ifndef __SETTINGSSTORAGE_H__
#define __SETTINGSSTORAGE_H__

#include <Arduino.h>
#include <EEPROM.h>

class SettingsStorage {
  public:
    struct Config {
      // char wwwUsername[20];
      // char wwwPassword[20];
      char targetUrl[64];
      char apiKey[20];
      int reportInterval;
      Config() {
        strncpy(targetUrl, "api.thingspeak.com/update?api_key={apiKey}", sizeof(targetUrl));
      }
    } settings;
    
    SettingsStorage::Config get() {
      EEPROM.begin(512);
      EEPROM.get(0, settings);
      return settings;
    }

    void save(int reportInterval, String apiKey, String targetUrl) {
      settings.reportInterval = reportInterval;
      strncpy(settings.apiKey, apiKey.c_str(), 20);
      strncpy(settings.targetUrl, targetUrl.c_str(), 64);
      EEPROM.put(0, settings);
      if (EEPROM.commit()) {
          Serial.println("[Info] Settings saved");
      } else {
          Serial.println("[Error] EEPROM error");
      }
    }
};

#endif // SETTINGSSTORAGE_H