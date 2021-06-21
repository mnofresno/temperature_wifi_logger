#ifndef __CONFIGSTORAGE_H__
#define __CONFIGSTORAGE_H__

#include "StoredConfig.h"
#include <EEPROM.h>
#include <Arduino.h>

#define START_ADDRESS 0

class ConfigStorage {
  public:
    StoredConfig get() {
      StoredConfigPacket packet;
      int totalSize = sizeof(StoredConfig);
      Serial.println("Total size: " + String(totalSize));
      for ( int i = 0 ; i < totalSize; i++ ) {
        byte current = EEPROM.read(i); 
        Serial.println("Read data byte: " + char(current));
        packet.store[i] =  current;
      }
      return packet.structure;
    }

    void save(StoredConfig config) {
      EEPROM.begin(sizeof(StoredConfig));
      StoredConfigPacket packet;
      packet.structure = config;
      int totalSize = sizeof(StoredConfig);
      Serial.println("Total size: " + String(totalSize));
      for ( int i = 0 ; i < totalSize; i++ ) {
        byte current = packet.store[i]; 
        EEPROM.put(i, current);  // one byte at a time
      }
      Serial.println(EEPROM.commit() ? "[Done] Saved to FLASH" : "[Error] Failed saving FLASH Data");
    }
};

#endif //CONFIGSTORAGE_H
