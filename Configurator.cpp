#include "Configurator.h"
#include "pages/config_html.h"
#include "pages/core_js.h"
#include <EEPROM.h>

bool Configurator::hasConfig() {
  return Configurator::isValidConfig(getConfig());
}

StoredConfig * Configurator::getConfig() {
  StoredConfig config;
  EEPROM.get(0, config);
  return &config;
}

bool Configurator::isValidConfig(StoredConfig * config) {
  return config->wifiSSID != NULL
    && config->wifiKey != NULL;
}

// #define UPDATE_SIZE_UNKNOWN 0XFFFFFFFF

// void Configurator::setup(const char* username, const char* password, ESP8266WebServer &server) {
//   _username = username;
//   _password = password;
//   _server = &server;
//   setup_root_path();
//   setup_update_path();
//   ArduinoOTA.begin();
// }

// void FirmwareUpdater::handle() {
//   ArduinoOTA.handle();
// }

// void FirmwareUpdater::setup_root_path() {
//   _server->on("/", [&]() {
//     authenticate_and_handle([&](){
//       _server->send(200, "text/html", UPLOADER_HTML);
//     });
//   });
//   _server->on("/core.js", [&]() {
//     authenticate_and_handle([&](){
//       _server->send(200, "application/javascript", CORE_JS);
//     });
//   });
// }

// void FirmwareUpdater::authenticate_and_handle(THandlerFunction handler) {
//   if (!_server->authenticate(_username, _password)) {
//     // FIXME: Maybe we can add a flag: return this or redirect?
//     // _server->send(200, "Auth error");
//     return _server->requestAuthentication();
//   }
//   handler();
// }

// void FirmwareUpdater::setup_update_path() {
//   _server->on("/update", HTTP_POST, [&]() {
//     finish_update();
//   }, [&]() {
//     handle_update();
//   });
// }

// void FirmwareUpdater::handle_update() {
//   HTTPUpload& upload = _server->upload();
//   if (upload.status == UPLOAD_FILE_START) {
//     Serial.printf("Update: %s\n", upload.filename.c_str());
//     if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//       Update.printError(Serial);
//     }
//   } else if (upload.status == UPLOAD_FILE_WRITE) {
//     if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//       Update.printError(Serial);
//     }
//   } else if (upload.status == UPLOAD_FILE_END) {
//     if (Update.end(true)) { //true to set the size to the current progress
//       Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//     } else {
//       Update.printError(Serial);
//     }
//   }
// }

// void FirmwareUpdater::finish_update() {
//   authenticate_and_handle([&](){
//     _server->sendHeader("Connection", "close");
//     _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//     delay(500);
//     ESP.restart();
//   });
// }
