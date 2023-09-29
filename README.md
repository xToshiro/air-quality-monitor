# air-quality-monitor
# Air Quality Monitor with ESP32

![License](https://img.shields.io/badge/license-GPL--3.0-blue)

This repository contains the source code and documentation for an air quality monitor based on a 30-pin ESP32. The project uses a variety of sensors to collect data on air quality, temperature, pressure, and humidity, and sends this data to Google Firebase Firestore. 

## Overview

The air quality monitor is designed to collect data from the following sensors:

- CCS811 Sensor: Measures eCO2 (equivalent carbon dioxide) and TVOCs (total volatile organic compounds).
- ADS1115 Converter with MICS 6814 Sensor: Collects data on CO (carbon monoxide), NH3 (ammonia), and NO2 (nitrogen dioxide).
- Particulate Sensor PMS 3003: Measures particulate matter in the PM1, PM2.5, and PM10 ranges.
- BME280 Temperature, Pressure, and Humidity Sensor: Collects weather data, including temperature, atmospheric pressure, and relative humidity.
- RTC DS1307 Clock: Used to timestamp data collection.
- DS18B20 Temperature Sensor: Measures ambient temperature.

Data collected by the sensors is processed, and averages are calculated over a data collection period (typically 30 seconds, but customizable). Subsequently, the data is sent to Google Firebase Firestore via a Wi-Fi connection.

## Used Libraries

The following libraries are used in this project:

- `WiFi.h`: For Wi-Fi connection management.
- `SoftwareSerial.h`, `Wire.h`, `OneWire.h`: For I/O management and sensor communication.
- `time.h`, `RTClib.h`: For clock and time management.
- `FS.h`, `SD.h`, `SPI.h`: For memory card management.
- `DFRobot_CCS811.h`, `Adafruit_ADS1X15.h`, `PMserial.h`, `DallasTemperature.h`, `Adafruit_BME280.h`: For sensor management.
- `Adafruit_Sensor.h`: Dependency required for Adafruit sensors.
- `Firebase_ESP_Client.h`: Dependency for sending data to Google Firebase Firestore.

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

## How to Use

1. Clone this repository to your development environment.
2. Connect all sensors according to the pinout defined in the source code.
3. Configure your Wi-Fi and Firebase credentials in the source code.
4. Upload the code to the ESP32.
5. Collected data will be sent to Firebase Firestore according to the configured collection interval.

## Notes

Ensure your sensors are correctly connected and calibrated for accurate results. For more information on setting up and calibrating specific sensors, refer to the manufacturers' documentation.

This project is a useful tool for monitoring air quality in different environments and can be customized to meet your specific needs. Feel free to contribute to this repository and improve the project.
