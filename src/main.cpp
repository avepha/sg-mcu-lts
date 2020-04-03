#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "./config.h"
#include "./bootstrap.h"
#include "logger/log.h"

LoggerTray *tray;

void loop1(void *pvParameters);

void setup() {
  bootstrap(); // init function
  controlScheduler.setHighPriorityScheduler(&gpioScheduler);

  serialEndpoint = new DeviceEndpoint(&Serial); // for laptop
  rpiEndpoint = new DeviceEndpoint(&entryPort);
  loraEndpoint = new LoraEndpoint(&stationPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);
  tray = new LoggerTray;
  Log::updateLogLevel();
  Log::setLoggerTray(tray);

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
        Log::error("json", error.c_str());
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

      if (Log::getLogLevel() >= DEBUG) {
        DynamicJsonDocument logJson(1024);
        logJson["method"] = "log";
        logJson["data"]["topic"] = "heap";
        logJson["data"]["level"] = "debug";
        logJson["data"]["message"] = "heap size: " + String(xPortGetFreeHeapSize());
        String logString;
        serializeJson(logJson, logString);
        rpiEndpoint->unleash(logString);
        serialEndpoint->unleash(logString);
      }
      continue;
      // for memory profiling
    }

    if (!Log::getLoggerTray()->isEmpty()) {
      DynamicJsonDocument logJson(1024);
      Logger log = Log::getLoggerTray()->pop();
      logJson["method"] = "log";
      logJson["data"]["topic"] = log.getTopic();
      logJson["data"]["level"] = log.getLevelString();
      logJson["data"]["message"] = log.getMessage();

      String logString;
      serializeJson(logJson, logString);
      rpiEndpoint->unleash(logString);
      serialEndpoint->unleash(logString);
    }

#ifdef SG_MCU_V2_LORA
    std::vector<byte> vByte;
    if (loraEndpoint->embrace(vByte)) {
      std::vector<Station *> stations = context->station->core->getStations();
      uint8_t st = vByte[0];
      bool isValid = false;
      for (int i = 0; i < stations.size(); i++) {
        if (stations[i]->getAddress() == st) {
          stations[i]->onPacketReceived(vByte);
          Log::trace("lora", "got packet sta: " + String(st));
          isValid = true;
        }
      }

      if (!isValid) {
        Log::error("lora", "no-station sta: " + String(st));
      }
    }
#endif

    delay(1);
  }
}

void loop() {
  controlScheduler.execute();
  backgroundScheduler.execute();
}


