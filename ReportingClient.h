#ifndef __REPORTINGCLIENT_H__
#define __REPORTINGCLIENT_H__

#include <Arduino.h>
#include <WiFiClientSecure.h> 

class ReportingClient {
  public:
    ReportingClient(String targetHost, String apiKey) {
      setTargetHost(targetHost);
      setApiKey(apiKey);
    }
    
    void setApiKey(String apiKey) {
      _apiKey = apiKey;
    }

    void setTargetHost(String targetHost) {
      _targetHost = targetHost;
    }

    void report(float h, float t, float tc) {
      // Temporary variables
      static char temperatureTemp[7];
      static char humidityTemp[7];
      static char temperatureTempTC[7];
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
      Serial.print(_targetHost);
      WiFiClient client;
      int retries = 5;
      while(!!!client.connect(_targetHost, 80) && (retries-- > 0)) {
        Serial.print(".");
      }
      Serial.println();
      if(!!!client.connected()) {
        LogError("Failed to connect, going back to sleep");
      }
      
      LogInfo("Request resource: " + resource); 
      client.print(String("GET ") + resource + _apiKey + "&field1=" + humidityTemp + "&field2=" + temperatureTemp + "&field3=" + temperatureTempTC +
                      " HTTP/1.1\r\n" +
                      "Host: " + _targetHost + "\r\n" + 
                      "Connection: close\r\n\r\n");
                      
      int timeout = 5 * 10; // 5 seconds             
      while(!!!client.available() && (timeout-- > 0)){
        delay(100);
      }
      if(!!!client.available()) {
        LogError("No response, going back to sleep");
      }
      while(client.available()){
        Serial.write(client.read());
      }
      
      LogInfo("closing connection");
      client.stop();
    }

  private:
    String resource = "/update?api_key=";
    String _targetHost;
    String _apiKey;
    void LogInfo(String payload) {
      Serial.println("[Info] " + payload);
    }
    void LogError(String payload) {
      Serial.println("[Error] " + payload);
    }
};

#endif // REPORTINGCLIENT_H
