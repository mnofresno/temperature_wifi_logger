#include "Configurator.h"
#include "pages/config_html.h"
#include <EEPROM.h>
#include <Arduino.h>

Configurator::Configurator(ESP8266WebServer &server, TEventConfigHandler onConfigChangeHandler) {
  SPIFFS.begin();
  _server = &server;
  _onConfigChangeHandler = onConfigChangeHandler;
  setup_routes();
}

bool Configurator::hasConfig() {
  return isValidConfig(getConfig());
}

StoredConfig * Configurator::getConfig() {
  StoredConfig config;
  EEPROM.get(0, config);
  return &config;
}

bool Configurator::isValidConfig(StoredConfig * config) {
  return config->wifiSSID != NULL
    && config->wifiKey != NULL
    && config->wifiSSID != ""
    && config->wifiKey != "";
}

void Configurator::setup_routes() {
  _server->on("/config", HTTP_GET, [&]() {
    handleGet(CONFIG_HTML);
  });
  _server->on("/config", HTTP_POST, [&]() {
    handlePost();
  });
}

std::function<String(const String&)> Configurator::getProcessor(StoredConfig * config) {
  return [=](const String& configField) -> String {
    if (configField == "wifiSSID")
      return config->wifiSSID;
    if (configField == "wifiKey")
      return config->wifiKey;
    if (configField == "wwwUsername")
      return config->wwwUsername;
    if (configField == "wwwPassword")
      return config->wwwPassword;
    if (configField == "targetHost")
      return config->targetHost;
    if (configField == "apiKeyFieldName")
      return config->apiKeyFieldName;
    if (configField == "apiKey")
      return config->apiKey;
    if (configField == "reportingIntervalSeconds")
      return (String)config->reportingIntervalSeconds;
    return String();
  };
}

void Configurator::handleGet(const char contents[]) {
  Serial.println("Received Config GET...");
  _server->send_P(200, "text/html", contents); //, getProcessor(getConfig()));
}

void Configurator::handlePost() {
  Serial.println("Received Config Post...");
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
    if (!_server->hasArg(field)) {
      missingFields += field + ", ";
    }      
  }
  if (missingFields != "") {
    Serial.println("Missing Fields...");
    _server->send(422, "application/json", "{\"missing_fields\": \"" + missingFields + "\"}");
  }
  StoredConfig config = StoredConfig {
    _server->arg("wifiSSID"),
    _server->arg("wifiKey"),
    _server->arg("wwwUsername"),
    _server->arg("wwwPassword"),
    _server->arg("targetHost"),
    _server->arg("apiKeyFieldName").c_str(),
    _server->arg("apiKey").c_str(),
    _server->arg("reportingIntervalSeconds").toInt()
  };
  _onConfigChangeHandler(config);
  Serial.println("Saving Fields...");
  EEPROM.put(0, config);
  Serial.println("Saved Fields OK");
  _server->send(201, "application/json", "{\"status\": \"ok\"}");
}
