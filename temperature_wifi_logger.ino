#include "max6675.h"
#include "DHTesp.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirmwareUpdater.h"
#include "arduino_secrets.h"
#include <ESP8266WebServer.h>
#include <TimedAction.h>
#include "pages/core_js.h"
#include <WiFiManager.h>
#include "ReportingClient.h"
#include <WiFiClientSecure.h>
#include "SettingsStorage.h"

int reportInterval = 300;

// CONNECTION PINS
// FOR THERMOCOUPLE
int SO_PIN = 12; // D6
int CS_PIN = 13; // D7
int SCK_PIN = 15; // D8
// FOR DHC
int DH_IN_PIN = 14;// D5
int DH_GND_PIN = 16; // D0 // FIXME: We use this pin as GND because the board only provides one GND, must create a bridge
// FOR WIFI STATION
int RESET_WIFI_INPUT_PIN = 20; // D1
int RESET_WIFI_GND_PIN = 19; // D2 // FIXME: We use this pin as GND because the board only provides one GND, must create a bridge

FirmwareUpdater firmwareUpdater;
MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
DHTesp dht;
ReportingClient client("api.thingspeak.com", SECRET_TS_API_KEY);
WiFiManager wm;
WiFiManagerParameter api_key_param("apiKey", "API Key", "please input your key", 20);
WiFiManagerParameter target_url_param("targetUrl", "Target URL", "introduce the target URL (apiKey is a placeholder)", 64);
WiFiManagerParameter report_interval_param("reportInterval", "Reporting Interval", "300", 32);

WiFiClientSecure wifiClient;
SettingsStorage settings;

void loopMeasurements();

TimedAction measurementTask = TimedAction(reportInterval * 1000, loopMeasurements);

void setupGNDPin(int gndPin) {
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin, LOW);
  delay(100);
}

void initializeDHT() {
  Serial.println("[Starting] Initialize DHT...");
  setupGNDPin(DH_GND_PIN);
  dht.setup(DH_IN_PIN, DHTesp::DHT11);
  Serial.println("[Done] DHT Running ok");
}

void initializeFirmwareUpdater() {
  Serial.println("[Starting] Initialize Firmware Updater...");
  auto server = wm.server.get();
  Serial.println("[Info] Obtained Server Pointer...");
  firmwareUpdater.setup(SECRET_WWW_USERNAME, SECRET_WWW_PASSWORD, server);
  Serial.println("[Info] / in your browser to see it working");
  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void bindServerCallback(){
  wm.server->on("/core.js", [&]() {
    wm.server->send(200, "application/javascript", CORE_JS);
  });
  initializeFirmwareUpdater();
}

void onParamsSave() {
  Serial.println("[Info] saveParamCallback callback fired");
  settings.save(
    String(report_interval_param.getValue()).toInt(),
    String(api_key_param.getValue()),
    String(target_url_param.getValue())
  );
  updateInterval();
  measurementTask.check();
  client.setApiKey(api_key_param.getValue());
  client.setTargetUrl(target_url_param.getValue());
}

void setupParams() {
  auto storedValues = settings.get(); 

  client.setApiKey(storedValues.apiKey);
  client.setTargetUrl(storedValues.targetUrl);
  doSetInterval(storedValues.reportInterval);
  api_key_param.setValue(storedValues.apiKey, 20);
  target_url_param.setValue(storedValues.targetUrl, 64);
  report_interval_param.setValue(String(storedValues.reportInterval).c_str(), sizeof(int));

  wm.setParamsPage(true);
  wm.setSaveParamsCallback(onParamsSave);
  wm.addParameter(&api_key_param);
  wm.addParameter(&report_interval_param);
  wm.addParameter(&target_url_param);
}

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.begin(115200);

  bool res;

  wm.setWebServerCallback(bindServerCallback);
  wm.setConfigPortalBlocking(false);
  setupParams();

  res = wm.autoConnect(); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
  
  initializeDHT();
  enableAllTasks();
  wifiClient.setInsecure();
  delay(5000);
  loopMeasurements();
  wm.startConfigPortal();
}

void enableAllTasks() {
  measurementTask.enable();
}

void disableAllTasks() {
  measurementTask.disable();
}

void loop() {
  wm.process();
  measurementTask.check();
  firmwareUpdater.handle();
}

void doSetInterval(int interval) {
  measurementTask.setInterval(interval * 1000);
}

void updateInterval() {
  int newReportInterval = settings.get().reportInterval;
  if (reportInterval != newReportInterval) {
    Serial.println("[Info] New report interval: " + String(newReportInterval));
    reportInterval = newReportInterval;
    doSetInterval(reportInterval);
  }
}

void loopMeasurements() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();
  client.report(h, t, tc);
}
