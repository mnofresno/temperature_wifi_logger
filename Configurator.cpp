#include "Configurator.h"
#include "pages/config_html.h"
#include <Arduino.h>
#include "ESPROM.h"

Configurator::Configurator(AsyncWebServer &server, TEventConfigHandler onConfigChangeHandler) {
  _server = &server;
  _onConfigChangeHandler = onConfigChangeHandler;
  setup_routes();
}

bool Configurator::hasConfig() {
  return isValidConfig(getConfig());
}

StoredConfig * Configurator::getConfig() {
  ESPROM::Memory memory = ESPROM::read();
  StoredConfig config;
  strcpy(config.wifiSSID, memory.credentials.ssid);
  strcpy(config.wifiKey, memory.credentials.psk);
  return &config;
}

bool Configurator::isValidConfig(StoredConfig * config) {
  return config->wifiSSID != NULL
    && config->wifiKey != NULL;
}

void Configurator::setup_routes() {
  _server->on("/config", HTTP_GET, [&](AsyncWebServerRequest *request) {
    handleGet(CONFIG_HTML, request);
  });
  _server->on("/config", HTTP_POST, [&](AsyncWebServerRequest *request) {
    handlePost(request);
  });
}

std::function<String(const String&)> Configurator::getProcessor(StoredConfig * config) {
  return [&](const String& configField) -> String {
    Serial.println("[Info] Processing field: " + configField);
    if (configField == "wifiSSID")
      return String(config->wifiSSID);
    if (configField == "wifiKey")
      return String(config->wifiKey);
    if (configField == "wwwUsername")
      return String(config->wwwUsername);
    if (configField == "wwwPassword")
      return String(config->wwwPassword);
    if (configField == "targetHost")
      return String(config->targetHost);
    // if (configField == "apiKeyFieldName")
    //   return String(config->apiKeyFieldName);
    // if (configField == "apiKey")
    //   return String(config->apiKey);
    // if (configField == "reportingIntervalSeconds")
    //   return String(config->reportingIntervalSeconds);
    return String();
  };
}

void Configurator::handleGet(const char contents[], AsyncWebServerRequest *request) {
  Serial.println("[Info] Received Config GET...");
  StoredConfig *config = getConfig();
  Serial.println("[Info] Config Retrieved");
  request->send_P(200, "text/html", contents, getProcessor(config));
}

void Configurator::handlePost(AsyncWebServerRequest *request) {
  Serial.println("[Info] Received Config Post...");
  String missingFields = "";
  String fields[] = {
    "wifiSSID",
    "wifiKey",
    "wwwUsername",
    "wwwPassword",
    "targetHost",
    "apiKeyFieldName",
    "apiKey",
    "reportingIntervalSeconds"
  };
  for (int i = 0; i < sizeof(fields) / sizeof(String); i++) {
    String field = fields[i];
    Serial.println("Analyzing field: " + field);
    if (!request->hasArg(field.c_str()) || request->arg(field.c_str()) == "") {
      missingFields += field + ", ";
    }      
  }
  if (missingFields != "") {
    Serial.println("[Error] Missing Fields...");
    request->send(422, "application/json", "{\"missing_fields\": \"" + missingFields + "\"}");
  }

  String ssid = request->arg("wifiSSID");
  String psk = request->arg("wifiKey");
  
  ESPROM::Credentials credentials;
  strcpy(credentials.ssid, ssid.c_str());
  strcpy(credentials.psk, psk.c_str());
  
  ESPROM::setCredentials(credentials);
  Serial.println("[Info] Starting save...");
  delay(500);
  Serial.println("[Done] Saved ssid and psk...");
  // StoredConfig config;
  // strcpy(config.wifiSSID, request->arg("wifiSSID").c_str());
  // strcpy(config.wifiSSID, request->arg("wifiSSID").c_str());
  // strcpy(config.wifiKey, request->arg("wifiKey").c_str());
  // strcpy(config.wwwUsername, request->arg("wwwUsername").c_str());
  // strcpy(config.wwwPassword, request->arg("wwwPassword").c_str());
  // strcpy(config.targetHost, request->arg("targetHost").c_str());
  // strcpy(config.apiKeyFieldName, request->arg("apiKeyFieldName").c_str());
  // strcpy(config.apiKey, request->arg("apiKey").c_str());
  // config.reportingIntervalSeconds = request->arg("reportingIntervalSeconds").toInt();
  // _onConfigChangeHandler(config);
  Serial.println("[Info] Saving Fields...");
  
  
  request->send(201, "application/json", "{\"status\": \"ok\"}");
  delay(500);
  Serial.println("[Done] Saved Fields OK and response sent");
}
