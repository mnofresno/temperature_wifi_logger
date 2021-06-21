#include "FirmwareUpdater.h"

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "pages/uploader_html.h"
#include "pages/core_js.h"

#define UPDATE_SIZE_UNKNOWN 0XFFFFFFFF

void FirmwareUpdater::setup(String username, String password, AsyncWebServer &server) {
  _username = username;
  _password = password;
  _server = &server;
  setup_root_path();
  setup_update_path();
  ArduinoOTA.begin();
}

void FirmwareUpdater::handle() {
  ArduinoOTA.handle();
}

void FirmwareUpdater::setup_root_path() {
  _server->on("/", [&](AsyncWebServerRequest *request) {
    authenticate_and_handle(request, [&](){
      request->send(200, "text/html", UPLOADER_HTML);
    });
  });
}

void FirmwareUpdater::authenticate_and_handle(AsyncWebServerRequest *request, THandlerFunction handler) {
  if (!request->authenticate(_username.c_str(), _password.c_str())) {
    // FIXME: Maybe we can add a flag: return this or redirect?
    // _server->send(200, "Auth error");
    return request->requestAuthentication();
  }
  handler();
}

void FirmwareUpdater::setup_update_path() {
  // _server->on("/update", HTTP_POST, [&](AsyncWebServerRequest *request) {
  //   finish_update(request);
  // }, [&](AsyncWebServerRequest *request) {
  //   handle_update(request);
  // });
}

void FirmwareUpdater::handle_update(AsyncWebServerRequest *request) {//, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // HTTPUpload& upload = request->upload();
  // if (upload.status == UPLOAD_FILE_START) {
  //   Serial.printf("Update: %s\n", upload.filename.c_str());
  //   if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
  //     Update.printError(Serial);
  //   }
  // } else if (upload.status == UPLOAD_FILE_WRITE) {
  //   if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
  //     Update.printError(Serial);
  //   }
  // } else if (upload.status == UPLOAD_FILE_END) {
  //   if (Update.end(true)) { //true to set the size to the current progress
  //     Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
  //   } else {
  //     Update.printError(Serial);
  //   }
  // }
}

void FirmwareUpdater::finish_update(AsyncWebServerRequest *request) {
  authenticate_and_handle(request, [&](){
    AsyncWebServerResponse * response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    request->send(response);
    delay(500);
    ESP.restart();
  });
}
