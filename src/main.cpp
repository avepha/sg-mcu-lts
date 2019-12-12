#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_TIMEOUT

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "config.h"
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#define VERSION "1.3.1"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1
#define EEPROM_SIZE 4096

Scheduler controlScheduler, gpioScheduler;

#include "util/util.h"
#include "combineContext.h"
#include "combineResolvers.h"

#include "validationError.h"
#include "deviceEndpoint.h"
#include "sensorEndpoint.h"

HardwareSerial &entryPort = Serial1;
HardwareSerial &sensorPort = Serial2;

DeviceEndpoint *deviceEndpoint, *serialEndpoint;
SensorEndpoint *sensorEndpoint;
CombineResolvers *resolvers;
CombineContext *context;

// simulate node sensor
#include "./util/simulateNodeSensors.h"
Scheduler simulateScheduler;
SimulateNodeSensorsTask *simulateNodeSensorsTask;

void loop1(void *pvParameters);

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  pinMode(SG_DIR_PIN, OUTPUT);
  digitalWrite(SG_DIR_PIN, LOW);

  Debug::update();
  Serial.begin(345600);
  entryPort.begin(345600, SERIAL_8N1, SG_MPU_RX, SG_MPU_TX);
  sensorPort.begin(9600, SERIAL_8N1, SG_SENSOR_RX, SG_SENSOR_TX);
  
  Serial.println();
  Serial.println("VERSION: " + String(VERSION));
  Serial.println("PROJECT: " + String(PROJECT));
  Serial.println("UPLOAD_DATE: " + String(UPLOADDATE));
  Serial.println("SG_MODEL: " + String(SG_MODEL));
  Serial.println("DEBUG: " + String(Debug::isDebuggingMode()));
#ifdef SG_TEST
  Serial.println("SG_MODE: TEST");
#endif

  controlScheduler.setHighPriorityScheduler(&gpioScheduler);

  serialEndpoint = new DeviceEndpoint(&Serial); // for laptop
  deviceEndpoint = new DeviceEndpoint(&entryPort);
  sensorEndpoint = new SensorEndpoint(&sensorPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);

  // for test
#ifdef SG_TEST
  simulateNodeSensorsTask = new SimulateNodeSensorsTask(&simulateScheduler);
#endif
}

void loop1(void *pvParameters) {
  while (true) {
    String requestString;;
    bool isDeviceDataComing = deviceEndpoint->embrace(&requestString);
    bool isEndpointDataComing = serialEndpoint->embrace(&requestString);

    if (isEndpointDataComing || isDeviceDataComing) {
      DynamicJsonDocument requestJson(2048);
      DeserializationError error = deserializeJson(requestJson, requestString);

      DynamicJsonDocument responseJson(2048);

      if (error) {

        InvalidJsonFormatError err;
        Debug::Print(error.c_str());
        responseJson = err.toJson();
      }
      else if (requestJson["topic"].isNull()) {
        InvalidRequestFormatError err("Topic must not be null");
        responseJson = err.toJson();
      }
      else if (requestJson["method"].isNull()) {
        InvalidRequestFormatError err("Method must not be null");
        responseJson = err.toJson();
      }
      else if (requestJson["reqId"].isNull()) {
        InvalidRequestFormatError err("reqId must not be null");
        responseJson = err.toJson();
      }
      else {
        responseJson = resolvers->execute(requestJson);
      }

      responseJson["reqId"] = requestJson["reqId"];

      String responseString;
      serializeJson(responseJson, responseString);

      if (isDeviceDataComing) {
        deviceEndpoint->unleash(responseString);
      }
      if (isEndpointDataComing) {
        serialEndpoint->unleash(responseString);
      }

      if (Debug::isDebuggingMode()) {
        DynamicJsonDocument responseMemory(128);
        responseMemory["topic"] = "memory";
        responseMemory["method"] = "event";
        responseMemory["data"]["freeHeap"] = String(xPortGetFreeHeapSize());
        String heapString;
        serializeJson(responseMemory, heapString);
        Serial.println(heapString);
      }
      continue;
      // for memory profiling
    }

    byte bSensors[64];
    int bSize = sensorEndpoint->embrace(bSensors);

    if (bSize > 0) {
      context->nsensors->core->updateNSensor(bSensors, bSize);
      continue;
    }
    else if(bSize == -2) { //checksum error
      NSensorInvalidCheckSumError err;
      Serial.println(err.toJsonString());
      deviceEndpoint->unleash(err.toJsonString());
      continue;
    }
    else if(bSize == -3) { //timeout
      NSensorTimeoutError err;
      Serial.println(err.toJsonString());
      deviceEndpoint->unleash(err.toJsonString());
      continue;
    }

    delay(1);
  }
}

void loop() {
#ifdef SG_TEST
  simulateScheduler.execute();
#endif
  controlScheduler.execute();
}


