#include "max6675.h"
#include "DHTesp.h"

int SO_PIN = 12; // D6
int CS_PIN = 13; // D7
int SCK_PIN = 15; // D8

int DH_IN_PIN = 14;// D5
int DH_GND_PIN = 16; // D0

MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
DHTesp dht;

  
void setup() {
  Serial.begin(115200);
  
  pinMode(DH_GND_PIN, OUTPUT);
  digitalWrite(DH_GND_PIN, LOW);
  dht.setup(D5, DHTesp::DHT11);

  Serial.println("MAX6675 test");
  // wait for MAX chip and DHT to stabilize
  delay(500);
}

void loop() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float tc = thermocouple.readCelsius();
      
  Serial.print("{\"humidity\": ");
  Serial.print(h);
  Serial.print(", \"temp\": ");
  Serial.print(t);
  Serial.print(", \"temp_tc\": ");
  Serial.print(tc);
  Serial.print("}\n");
   delay(1000);
}
