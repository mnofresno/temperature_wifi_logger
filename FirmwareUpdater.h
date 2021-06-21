#ifndef __FIRMWARE_UPDATER_H__
#define __FIRMWARE_UPDATER_H__

#include <ESPAsyncWebServer.h>

typedef std::function<void(void)> THandlerFunction;

class FirmwareUpdater {
  public:
    void setup(String username, String password, AsyncWebServer &server);
    void handle();
  private:
    void setup_root_path();
    void setup_update_path();
    void authenticate_and_handle(AsyncWebServerRequest *request, THandlerFunction handler);
    void handle_update(AsyncWebServerRequest *request);
    void finish_update(AsyncWebServerRequest *request);
    
    AsyncWebServer * _server;
    String _username;
    String _password;
};

#endif //FIRMWAREUPDATER_H
