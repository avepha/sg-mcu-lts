#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#define VERSION "1.0.0"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1
#define EEPROM_SIZE 4096

#include "util/util.h"
#include "combineContext.h"
#include "combineResolvers.h"

#include "validationError.h"
#include "endpoint.h"

HardwareSerial &entryPort = Serial;
HardwareSerial &sensorCom = Serial2;

EndPoint *endpoint;
CombineResolvers *resolvers;
CombineContext *context;

void loop1(void *pvParameters);

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  entryPort.begin(115200);


  endpoint = new EndPoint(&entryPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);
}

void loop1(void *pvParameters) {
  while (true) {
    String jsonString = endpoint->embrace();
    if (jsonString == "NULL") {
      endpoint->unleash((new InvalidJsonFormatError())->toJsonString());
      continue;
    }

    StaticJsonDocument<1024> json;
    DeserializationError error = deserializeJson(json, jsonString);
    if (error) {
      endpoint->unleash((new InvalidJsonFormatError())->toJsonString());
      continue;
    }

    if (json["topic"].isNull() || json["method"].isNull()) {
      endpoint->unleash((new InvalidRequestFormatError())->toJsonString());
      continue;
    }

    endpoint->unleash(resolvers->execute(json.as<JsonObject>()));
  }
}

void loop() {

}