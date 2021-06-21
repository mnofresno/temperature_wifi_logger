#ifndef __CONFIGURATOR_H__
#define __CONFIGURATOR_H__

#include "StoredConfig.h"
#include <ESPAsyncWebServer.h>

typedef std::function<void(StoredConfig)> TEventConfigHandler;

class Configurator {
  public:
    Configurator(AsyncWebServer &server, TEventConfigHandler onConfigChangeHandler);
    bool hasConfig();
    StoredConfig * getConfig();
  private:
    bool isValidConfig(StoredConfig *config);
    void setup_routes();
    void handleGet(const char contents[], AsyncWebServerRequest *request);
    void handlePost(AsyncWebServerRequest *request);
    std::function<String(const String&)> getProcessor(StoredConfig * config);
    AsyncWebServer * _server;
    TEventConfigHandler _onConfigChangeHandler;
};

#endif //CONFIGURATOR_H
