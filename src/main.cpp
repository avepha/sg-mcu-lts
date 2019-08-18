#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#define VERSION "0.0.1"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1
#define EEPROM_SIZE 4096

#include "util/util.h"
#include "combineContext.h"
#include "combineResolvers.h"

#include "validationError.h"
#include "deviceEndpoint.h"
#include "sensorEndpoint.h"

HardwareSerial &entryPort = Serial;
HardwareSerial &sensorPort = Serial2;

DeviceEndpoint *deviceEndpoint;
SensorEndpoint *sensorEndpoint;
CombineResolvers *resolvers;
CombineContext *context;

void loop1(void *pvParameters);

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  entryPort.begin(115200);
  sensorPort.begin(9600);

  deviceEndpoint = new DeviceEndpoint(&entryPort);
  sensorEndpoint = new SensorEndpoint(&sensorPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);
}

void loop1(void *pvParameters) {
  while (true) {
    String jsonString;
    bool isDataComing = deviceEndpoint->embrace(&jsonString);
    if (isDataComing) {
      StaticJsonDocument<1024> json;
      DeserializationError error = deserializeJson(json, jsonString);
      if (error) {
        InvalidJsonFormatError err;
        deviceEndpoint->unleash(err.toJsonString());
        continue;
      }

      if (json["topic"].isNull() || json["method"].isNull()) {
        InvalidRequestFormatError err;
        deviceEndpoint->unleash(err.toJsonString());
        continue;
      }

      int _f = millis();
      deviceEndpoint->unleash(resolvers->execute(json.as<JsonObject>()));
      Serial.println("execution time: " + String(millis() - _f));
    }
    

    byte bSensors[64];
    int bSize = sensorEndpoint->embrace(bSensors);

    if (bSize > 0) {
      context->nSensorContext->core->updateNSensor(bSensors, bSize);
    }
    else if(bSize == -2) { //checksum error
      NSensorInvalidCheckSumError err;
      Serial.println(err.toJsonString());
      deviceEndpoint->unleash(err.toJsonString());
    }
    else if(bSize == -3) { //timeout
      NSensorTimeoutError err;
      Serial.println(err.toJsonString());
      deviceEndpoint->unleash(err.toJsonString());
    }
  }
}

void loop() {
//  Serial.println("free heap: " + String(xPortGetFreeHeapSize()));
  delay(1000);
}