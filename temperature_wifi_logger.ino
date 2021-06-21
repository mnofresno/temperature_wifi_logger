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
#include "pages/core_js.h"

// SoftAP config
IPAddress ip(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 0, 0, 0);

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

StoredConfig * _config;
FirmwareUpdater firmwareUpdater;
ESP8266WebServer server;
MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
DHTesp dht;
WiFiClientSecure client;

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

void onConfigChange(StoredConfig config) {
  Serial.println("New reporting interval received: " + (String)config.reportingIntervalSeconds);
  measurementTask.disable();
  measurementTask.setInterval(config.reportingIntervalSeconds * 1000);
  measurementTask.enable();
}

Configurator configurator(server, onConfigChange);

const char* resource = "/update?api_key=";

// Temporary variables
static char temperatureTemp[7];
static char humidityTemp[7];
static char temperatureTempTC[7];

void setupGNDPin(int gndPin) {
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin, LOW);
}

bool initializeWifiIsOk() {
  Serial.println("Initialize Wifi...");
  setupGNDPin(RESET_WIFI_GND_PIN);
  pinMode(RESET_WIFI_INPUT_PIN, INPUT_PULLUP);
  delay(300);
  bool resetPinIsHigh = digitalRead(RESET_WIFI_INPUT_PIN) == HIGH;
  String resetPinMessage = resetPinIsHigh
    ? "High"
    : "Low";
  Serial.println("Reset INPUT PIN is: " + resetPinMessage);
  if (!configurator.hasConfig() || resetPinIsHigh) {
    Serial.println("Initialize Wifi SoftAP...");
    WiFi.mode(WIFI_AP); 
    String chipId = (String)ESP.getChipId();
    String status = WiFi.softAPConfig(ip, gateway, subnet) ? "Ready" : "Failed!";
    Serial.println("Config: " + status);
    status = WiFi.softAPConfig(ip, gateway, subnet) ? "Ready" : "Failed!";
    Serial.println("Standup: " + status);
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    Serial.println("Generated SoftAP OK...");
    return false;
  }
  Serial.println("Initialize Wifi STA...");
  WiFi.mode(WIFI_STA);
  _config = configurator.getConfig();  
  WiFi.begin(_config->wifiSSID, _config->wifiKey);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
    return false;
  }
  client.setInsecure();
  Serial.println("Connected Wifi OK...");
  return true;
}

void initializeDHT() {
  Serial.println("Initialize DHT...");
  setupGNDPin(DH_GND_PIN);
  dht.setup(DH_IN_PIN, DHTesp::DHT11);
}

void initializeFirmwareUpdater() {
  Serial.println("Initialzie Firmware Updater...");
  firmwareUpdater.setup(_config->wwwUsername, _config->wwwPassword, server);
  Serial.println("/ in your browser to see it working");
  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void initializeWebServer() {
  Serial.println("Initialize Webserver...");
  server.begin(80);
  server.on("/core.js", [&]() {
    server.send(200, "application/javascript", CORE_JS);
  });
  Serial.println("Webserver init OK...");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  if (initializeWifiIsOk()) {
    initializeDHT();
    delay(250);
    initializeFirmwareUpdater();
    delay(250);
    enableAllTasks();
  } else {
    disableAllTasks();
  }
  initializeWebServer();
}

void enableAllTasks() {
  measurementTask.enable();
}

void disableAllTasks() {
  measurementTask.disable();
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
  Serial.print(_config->targetHost);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(_config->targetHost, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);
  client.print(String("GET ") + resource + _config->apiKey + "&field1=" + humidityTemp + "&field2=" + temperatureTemp + "&field3=" + temperatureTempTC +
                  " HTTP/1.1\r\n" +
                  "Host: " + _config->targetHost + "\r\n" + 
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
