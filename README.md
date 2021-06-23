# Temperature Wifi Logger

## Project Hardware BOM

* Wemos D1 R1 Mini ESP8266
* DHT11 Humidity & Temp Sensor
* MAX66775 Thermocouple Amplifier + Thermocouple

## Introduction

The aim of this project is to build a multi-purpose pseudo-meteorological station.

Currently it measures humidity and temperature from the air with a DHT11 sensor, it also has attached a thermocouple sensor MAX6675

The first problem this project is trying to solve is the meassurement and traceability of a centralized calefaction system. And the second one will be an identical instance of this IOT device (but maybe without the thermocouple) suited about 1 meter away from the inner of the apartment.

In the inner instance of the project we are using the TC to measure the calefaction pipelines temperature, at the same time we measure ambient temperature. This will help (by analyzing the data) to measure the correlation between the working hours of the central boiler and the thermal inerthia of the building at different climate conditions.

# Over the air programming

This project inherits some files from [this repo](https://github.com/mnofresno/esp32-over-the-air-programming) but adapted to the ESP8266 microprocessor of this project. You can see that part's README file [here](OTA.md).