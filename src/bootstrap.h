
#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_TIMEOUT

#include <Arduino.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include "./config.h"

#ifndef SG_MCU_BOOTSTRAP_H
#define SG_MCU_BOOTSTRAP_H

#define VERSION "1.8.1"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1
#define EEPROM_SIZE 4096

Scheduler controlScheduler, gpioScheduler, backgroundScheduler;
HardwareSerial &entryPort = Serial1;
HardwareSerial &stationPort = Serial2;

#include "util/util.h"
#include "combineContext.h"
#include "combineResolvers.h"

#include "validationError.h"
#include "deviceEndpoint.h"
#include "loraEndpoint.h"

DeviceEndpoint *rpiEndpoint, *serialEndpoint;
LoraEndpoint *loraEndpoint;
CombineResolvers *resolvers;
CombineContext *context;


void bootstrap() {
  EEPROM.begin(EEPROM_SIZE);
  pinMode(RS485_DIR_PIN, OUTPUT);
  digitalWrite(RS485_DIR_PIN, RS485_RECV_MODE);

  pinMode(SG_STATION_TX, OUTPUT);
#ifdef SG_MCU_V2_LORA
  digitalWrite(SG_STATION_TX, LOW);
#else
  digitalWrite(SG_STATION_TX, HIGH);
#endif

  pinMode(SG_STATION_RX, INPUT);

  Serial.begin(serialBaudRate);
  entryPort.begin(rpiBaudRate, SERIAL_8N1, SG_MPU_RX, SG_MPU_TX);
  stationPort.begin(9600, SERIAL_8N1, SG_STATION_RX, SG_STATION_TX);

  Serial.println();
  Serial.println("VERSION: " + String(VERSION));
  Serial.println("PROJECT: " + String(PROJECT));
  Serial.println("UPLOAD_DATE: " + String(UPLOADDATE));
  Serial.println("SG_MODEL: " + String(SG_MODEL));
#ifdef SG_TEST
  Serial.println("SG_MODE: TEST");
#endif

}

#endif
