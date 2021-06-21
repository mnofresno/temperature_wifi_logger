#ifndef __CONFIGURATOR_H__
#define __CONFIGURATOR_H__

#include "StoredConfig.h"
#include <ESP8266WebServer.h>

typedef std::function<void(StoredConfig)> TEventConfigHandler;

class Configurator {
  public:
    Configurator(ESP8266WebServer &server, TEventConfigHandler onConfigChangeHandler);
    bool hasConfig();
    StoredConfig * getConfig();
  private:
    bool isValidConfig(StoredConfig *config);
    void setup_routes();
    void handleGet(const char contents[]);
    void handlePost();
    std::function<String(const String&)> getProcessor(StoredConfig * config);
    ESP8266WebServer * _server;
    TEventConfigHandler _onConfigChangeHandler;
};

#endif //CONFIGURATOR_H
