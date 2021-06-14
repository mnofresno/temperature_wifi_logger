#include "max6675.h"
#include "DHTesp.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirmwareUpdater.h"
#include "arduino_secrets.h"
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

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
WiFiClientSecure client;

void initializeWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  client.setInsecure();
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

String getLastReadings() {
  String last_string = "";
  serializeJson(last_readings, last_string);
  return last_string;
}

void initializeWebServer() {
  server.begin(80);
  server.on("/status", [&]() {
    server.send(200, "application/json", getLastReadings());
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
  client.setInsecure();

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
  // firmwareUpdater.handle();
  server.handleClient();
  
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();

  makeHTTPRequest(h, t, tc);
  delay(2000);
}
