#ifndef __ESPROM_H__
#define __ESPROM_H__

#include <Arduino.h>
#include <EEPROM.h>

class ESPROM {
  public:
    struct Credentials {
      char ssid[24];
      char psk[16];
    };

    struct Memory {
      Credentials credentials;
      
      int timestamp;
      byte lastState;
    };

    static ESPROM::Memory read() {
      Memory memory;

      EEPROM.begin(1024);
      EEPROM.get(0, memory);
      EEPROM.end();
      delay(500);
      
      return memory;
    }

    static void write(ESPROM::Memory credentials) {
      EEPROM.begin(1024);
      EEPROM.put(0, credentials);
      EEPROM.end();
    }

    static void setCredentials(ESPROM::Credentials credentials) {
      Memory memory = ESPROM::read();
      memory.credentials = credentials;
      ESPROM::write(memory);
      delay(1000);
    }
};
#endif // ESPROM_H
