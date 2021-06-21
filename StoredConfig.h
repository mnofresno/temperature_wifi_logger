#ifndef __STOREDCONFIG_H__
#define __STOREDCONFIG_H__

#include <Arduino.h>

struct StoredConfig {
  String wifiSSID;
  String wifiKey ;
  String targetHost;
  String apiKey;
  String apiKeyFieldName;
  const char* wwwUsername;
  const char* wwwPassword;
  int reportingIntervalSeconds;
};

#endif // STOREDCONFIG_H
