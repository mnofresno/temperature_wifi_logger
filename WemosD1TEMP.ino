#include "max6675.h"
#include "DHTesp.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirmwareUpdater.h"
#include "arduino_secrets.h"
#include <ESP8266WebServer.h>


const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASSWORD;
const char* www_username = SECRET_WWW_USERNAME;
const char* www_password = SECRET_WWW_PASSWORD;

int SO_PIN = 12; // D6
int CS_PIN = 13; // D7
int SCK_PIN = 15; // D8

int DH_IN_PIN = 14;// D5
int DH_GND_PIN = 16; // D0

//char* last_readings = "";
DynamicJsonDocument last_readings(1024);

MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
DHTesp dht;
FirmwareUpdater firmwareUpdater;
ESP8266WebServer server;

void initializeWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
}

void initializeDHT() {
  pinMode(DH_GND_PIN, OUTPUT);
  digitalWrite(DH_GND_PIN, LOW);
  dht.setup(DH_IN_PIN, DHTesp::DHT11);
}

void initializeFirmwareUpdater() {
  firmwareUpdater.setup(www_username, www_password, server);
  Serial.println("/ in your browser to see it working");
  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void initializeWebServer() {
  server.begin(80);
  server.on("/status", [&]() {
    String last_string = "";
    serializeJson(last_readings, last_string);
    server.send(200, "application/json", last_string);
  });
}

void setup() {
  Serial.begin(115200);
  delay(500);
  initializeWifi();
  initializeDHT();
  delay(250);
  initializeFirmwareUpdater();
  delay(250);
  initializeWebServer();

  Serial.println("MAX6675 test");
  // wait for MAX chip and DHT to stabilize
  delay(500);
}

void loop() {
  firmwareUpdater.handle();
  server.handleClient();
  
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();

  // sprintf(last_readings, "{\"humidity\": %f, \"temp\": %f, \"temp_tc\": %f }", h, t, tc);
  last_readings["humidity"] = h;
  last_readings["temp"] = t;
  last_readings["temp_tc"] = tc;
}
