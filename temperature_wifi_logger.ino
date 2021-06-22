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
WiFiClientSecure wifiClient;

void loopMeasurements();

TimedAction measurementTask = TimedAction(300000, loopMeasurements);

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

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.begin(115200);

  bool res;

  wm.setWebServerCallback(bindServerCallback);
  wm.setConfigPortalBlocking(false);

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

void loopMeasurements() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();
  client.report(h, t, tc);
}
