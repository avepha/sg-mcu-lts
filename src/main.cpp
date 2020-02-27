#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_TIMEOUT

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "config.h"
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#define VERSION "1.4.3"
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

void loop1(void *pvParameters);

void setup() {
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

  Debug::update();
  Serial.begin(serialBaudRate);
  entryPort.begin(rpiBaudRate, SERIAL_8N1, SG_MPU_RX, SG_MPU_TX);
  stationPort.begin(9600, SERIAL_8N1, SG_STATION_RX, SG_STATION_TX);

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
  rpiEndpoint = new DeviceEndpoint(&entryPort);
  loraEndpoint = new LoraEndpoint(&stationPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);
}

void loop1(void *pvParameters) {
  delay(1000);
  while (true) {
    String requestString;;
    bool isDeviceDataComing = rpiEndpoint->embrace(&requestString);
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
        rpiEndpoint->unleash(responseString);
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

#ifdef SG_MCU_V2_LORA
    String loraString;
    loraEndpoint->embrace(&loraString);
#endif

    delay(1);
  }
}

void loop() {
  controlScheduler.execute();
  backgroundScheduler.execute();
}


