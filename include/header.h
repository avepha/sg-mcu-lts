#include <Arduino.h>

#define VERSION "2.0.0"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1

HardwareSerial &mpuCom = Serial1;
HardwareSerial &sensorCom = Serial2;