#ifndef __SETTINGSSTORAGE_H__
#define __SETTINGSSTORAGE_H__

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFiManager.h>

class IntParameter : public WiFiManagerParameter {
  public:
    IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
    }

    long getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }
};

class SettingsStorage {
  public:
    struct Config {
      // char wwwUsername[20];
      // char wwwPassword[20];
      // char targetUrl[64];
      char apiKey[20];
      int reportInterval;
    } settings;
    
    SettingsStorage::Config get() {
      EEPROM.begin(512);
      EEPROM.get(0, settings);
      return settings;
    }

    void save(int reportInterval, String apiKey) {
      settings.reportInterval = reportInterval;
      strncpy(settings.apiKey, apiKey.c_str(), 20);
      EEPROM.put(0, settings);
      if (EEPROM.commit()) {
          Serial.println("[Info] Settings saved");
      } else {
          Serial.println("[Error] EEPROM error");
      }
    }

  //   void configureParameters() {
  //     _wm->setSaveParamsCallback([&]() {
  //       onParamsSave();
  //     });
  //     auto setting = get();
  //     // setting.wwwUsername[19] = '\0';
  //     // setting.wwwPassword[19] = '\0';
  //     // setting.targetUrl[63] = '\0';
  //     setting.apiKey[19] = '\0';
  //     IntParameter reportInterval("reportInterval", "300", setting.reportInterval);
  //     WiFiManagerParameter apiKeyParam("apiKey", "not set", setting.apiKey, 20);
  //     _reportIntervalParam =& reportInterval;
  //     _apiKeyParam =& apiKeyParam;
  //     _wm->addParameter(&reportInterval);
  //     _wm->addParameter(&apiKeyParam);
  //   };
  // private:
  //   WiFiManager *_wm;
  //   IntParameter * _reportIntervalParam;
  //   WiFiManagerParameter * _apiKeyParam;
};

#endif // SETTINGSSTORAGE_H