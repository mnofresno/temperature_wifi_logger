#ifndef __STOREDCONFIG_H__
#define __STOREDCONFIG_H__

#include <Arduino.h>
#define STRING_FIELD_SIZE 64
struct StoredConfig {
  char wifiSSID[STRING_FIELD_SIZE];
  char wifiKey[STRING_FIELD_SIZE];
  char targetHost[STRING_FIELD_SIZE];
  char apiKey[STRING_FIELD_SIZE];
  char apiKeyFieldName[STRING_FIELD_SIZE];
  char wwwUsername[STRING_FIELD_SIZE];
  char wwwPassword[STRING_FIELD_SIZE];
  int reportingIntervalSeconds;
};

const int union_size = sizeof(StoredConfig);
typedef union StoredConfigPacket{
  StoredConfig structure;
  byte store[union_size];
};
#endif // STOREDCONFIG_H
