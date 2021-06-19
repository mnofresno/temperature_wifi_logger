#include "max6675.h"
#include "DHTesp.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirmwareUpdater.h"
#include "arduino_secrets.h"
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <TimedAction.h>
#include "Configurator.h"
#include "StoredConfig.h"

// CONNECTION PINS
// FOR THERMOCOUPLE
int SO_PIN = 12; // D6
int CS_PIN = 13; // D7
int SCK_PIN = 15; // D8
// FOR DHC
int DH_IN_PIN = 14;// D5
int DH_GND_PIN = 16; // D0 // FIXME: We use PIN D0 as GND
// FOR WIFI STATION
int RESET_WIFI_INPUT_PIN = 18; // D3
int RESET_WIFI_GND_PIN = 17; // D4 // FIXME: We use PIN D4 as GND


FirmwareUpdater firmwareUpdater;
ESP8266WebServer server;
MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
DHTesp dht;
WiFiClientSecure client;
Configurator configurator;

void handleWebServer() {
  server.handleClient();
}

void handleFirmwareUpdater() {
  firmwareUpdater.handle();
}

void loopMeasurements() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();
  makeHTTPRequest(h, t, tc);
}

TimedAction firmwareUpdaterTask = TimedAction(50, handleFirmwareUpdater);
TimedAction serverTask = TimedAction(50, handleWebServer);
TimedAction measurementTask = TimedAction(3000, loopMeasurements);

// Replace with your unique Thing Speak WRITE API KEY
const char* apiKey = SECRET_TS_API_KEY;

const char* resource = "/update?api_key=";

// Thing Speak API server 
const char* targetHost = "api.thingspeak.com";

const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASSWORD;
const char* www_username = SECRET_WWW_USERNAME;
const char* www_password = SECRET_WWW_PASSWORD;

// Temporary variables
static char temperatureTemp[7];
static char humidityTemp[7];
static char temperatureTempTC[7];

void setupGNDPin(int gndPin) {
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin, LOW);
}

void initializeWifi() {
  setupGNDPin(RESET_WIFI_GND_PIN);
  pinMode(RESET_WIFI_INPUT_PIN, INPUT_PULLUP);
  delay(300);
  if (!configurator.hasConfig() || digitalRead(RESET_WIFI_INPUT_PIN) == HIGH) {
    WiFi.mode(WIFI_AP); 
    String chipId = (String)ESP.getChipId();
    WiFi.softAP("ESPSensor_" + chipId);
  } else {
    WiFi.mode(WIFI_STA);
    StoredConfig * config = configurator.getConfig();  
    WiFi.begin(config->wifiSSID, config->wifiKey);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi Connect Failed! Rebooting...");
      delay(1000);
      ESP.restart();
    }
    client.setInsecure();
  }
}

void initializeDHT() {
  setupGNDPin(DH_GND_PIN);
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
  /*server.on("/status", [&]() {
    server.send(200, "application/json", getLastReadings());
  }); */
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

  delay(500);
}

// Make an HTTP request to Thing Speak
void makeHTTPRequest(float h, float t, float tc) {
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(tc)) {
    Serial.println("Failed to read from DHT sensor!");
    strcpy(humidityTemp, "Failed");
    strcpy(temperatureTemp,"Failed");
    strcpy(temperatureTempTC, "Failed");
    return;    
  }
  else {
    // Computes temperature values in Celsius + Fahrenheit and Humidity
    // float hic = dht.computeHeatIndex(t, h, false); 
    // Comment the next line, if you prefer to use Fahrenheit      
    
    dtostrf(h, 6, 2, humidityTemp);             
    dtostrf(t, 6, 2, temperatureTemp);
    dtostrf(tc, 6, 2, temperatureTempTC);

    
    // You can delete the following Serial.print's, it's just for debugging purposes
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(" %\t Temperature TC: ");
    Serial.print(tc);
    Serial.print(" *C ");
    
  }
  
  Serial.print("Connecting to "); 
  Serial.print(targetHost);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(targetHost, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);
  client.print(String("GET ") + resource + apiKey + "&field1=" + humidityTemp + "&field2=" + temperatureTemp + "&field3=" + temperatureTempTC +
                  " HTTP/1.1\r\n" +
                  "Host: " + targetHost + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
  client.stop();
}

void loop() {
  firmwareUpdaterTask.check();
  serverTask.check();
  measurementTask.check();
}
