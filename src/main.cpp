//#define SG_TEST
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

HardwareSerial &entryPort = Serial1;
HardwareSerial &sensorPort = Serial2;

DeviceEndpoint *deviceEndpoint, *serialEndpoint;
SensorEndpoint *sensorEndpoint;
CombineResolvers *resolvers;
CombineContext *context;

void loop1(void *pvParameters);

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  entryPort.begin(115200, SERIAL_8N1, 18, 19);
  sensorPort.begin(9600);

  serialEndpoint = new DeviceEndpoint(&Serial); // for laptop
  deviceEndpoint = new DeviceEndpoint(&entryPort);
  sensorEndpoint = new SensorEndpoint(&sensorPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);
}

void loop1(void *pvParameters) {
  while (true) {
    String requestString;;
    bool isDeviceDataComing = deviceEndpoint->embrace(&requestString);
    bool isEndpointDataComing = serialEndpoint->embrace(&requestString);

    if (isEndpointDataComing || isDeviceDataComing) {
      DynamicJsonDocument requestJson(1024);
      DeserializationError error = deserializeJson(requestJson, requestString);

      DynamicJsonDocument responseJson(2048);

      if (error) {
        InvalidJsonFormatError err;
        responseJson = err.toJson();
      }
      else if (requestJson["topic"].isNull() || requestJson["method"].isNull()) {
        InvalidRequestFormatError err;
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
#ifdef SG_TEST
  for (int i = 1 ; i < 3; i++) { // mock 3 stations
    int bucketSize = 7;
    uint16_t sensorBucket[bucketSize];
    sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
    sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
    sensorBucket[2] = Float16Compressor::compress((float) random(290, 298) / 10);
    sensorBucket[3] = Float16Compressor::compress((float) random(510, 520) / 10);
    sensorBucket[4] = Float16Compressor::compress((float) random(10, 15) / 10);
    sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);
    sensorBucket[6] = Float16Compressor::compress((float) random(10, 15) / 10);

    byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
    byte payload_sta_1[payloadSize];
    craftBytesArrayOfSensorPayload(i, sensorBucket, bucketSize, payload_sta_1);

    int plain_payload_size = payloadSize - 3;
    byte plain_payload[plain_payload_size];
    memcpy(plain_payload, payload_sta_1 + 1, plain_payload_size);

    context->nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);
  }
  for (int i = 1 ; i < 3; i++) { // mock 3 stations
    int bucketSize = 7;
    uint16_t sensorBucket[bucketSize];
    sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
    sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
    sensorBucket[2] = Float16Compressor::compress((float) random(31000, 31500) / 10);
    sensorBucket[3] = Float16Compressor::compress((float) random(290, 298) / 10);
    sensorBucket[4] = Float16Compressor::compress((float) random(510, 520) / 10);
    sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);
    sensorBucket[6] = Float16Compressor::compress((float) random(10, 15) / 10);

    byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
    byte payload_sta_1[payloadSize];
    craftBytesArrayOfSensorPayload(i, sensorBucket, bucketSize, payload_sta_1);

    int plain_payload_size = payloadSize - 3;
    byte plain_payload[plain_payload_size];
    memcpy(plain_payload, payload_sta_1 + 1, plain_payload_size);

    context->nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);
  }
#endif

  delay(1000);
}
