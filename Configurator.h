#ifndef __CONFIGURATOR_H__
#define __CONFIGURATOR_H__

#include "StoredConfig.h"

// typedef std::function<void(void)> THandlerFunction;

class Configurator {
  public:
    bool hasConfig();
    StoredConfig * getConfig();
  private:
    bool isValidConfig(StoredConfig * config);
  //   void setup(const char* username, const char* password, ESP8266WebServer &server);
  //   void handle();
  // private:
  //   void setup_root_path();
  //   void setup_update_path();
  //   void authenticate_and_handle(THandlerFunction handler);
  //   void handle_update();
  //   void finish_update();
    
  //   ESP8266WebServer * _server;
  //   const char* _username;
  //   const char* _password;
};

#endif //CONFIGURATOR_H
