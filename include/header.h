#include <Arduino.h>
#include <Task.h>
TaskManager taskManager;

#include <Wire.h>
#include <EEPROM.h>

String version = "1.0.4";
#define DEBUG true

#define EEPROM_SIZE 1024
#define CHANNEL_NUMBER 4
#define VERSION 2.0
#define PROJECT "SMART-GROBOT"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define ledPin 2
#define co2Pin 4
#define ecPin 6
#define phPin 7
#define pumpPin 3
#define valvePin 5


HardwareSerial &mpuCom = Serial;
HardwareSerial &sensorCom = Serial2;
