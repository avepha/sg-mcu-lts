using namespace std;

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "./config.h"
#include "./bootstrap.h"
#include "./logger/log.h"
#include "./logger/event.h"

#include "./domain/notification/notificationManager.h"

#include "./dev/devSimplexStationTask.h"

LoggerTray *loggerTray;
NotificationTray *notificationTray;

void loop1(void *pvParameters);

void setup() {
  bootstrap(); // init function
  controlScheduler.setHighPriorityScheduler(&gpioScheduler);

  EventManager::init();
  DynamicJsonDocument eventData(256);
  eventData["bc"] = bootCount;
  EventManager::create("init", eventData.as<JsonObject>());


  loggerTray = new LoggerTray;
  Log::updateLogLevel();
  Log::setLoggerTray(loggerTray);

  notificationTray = new NotificationTray;
  NotificationManager::setNotificationTray(notificationTray);

  serialEndpoint = new DeviceEndpoint(&Serial); // for laptop
  rpiEndpoint = new DeviceEndpoint(&entryPort);
  loraEndpoint = new LoraEndpoint(&stationPort);
  context = new CombineContext();
  resolvers = new CombineResolvers(context);
  initModel(context); // init model for each mode

  xTaskCreatePinnedToCore(loop1, "loop1", 4096 * 8, NULL, 1, NULL, COMCORE);
}

void loop1(void *pvParameters) {
  delay(1000);
  //  notification
  uint32_t notiTimestamp = 0;
  uint16_t notificationFrame = 2;
  uint16_t currentNotificationIndex = 0;

  //simplex_dev
  uint32_t simplexTimestamp = 0;

  while (true) {
    String requestString;
    bool isDeviceDataComing = rpiEndpoint->embrace(&requestString);
    bool isEndpointDataComing = serialEndpoint->embrace(&requestString);

    if (isEndpointDataComing || isDeviceDataComing) {
      Log::trace("device-endpoint", "got request " + requestString);
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

    if (!EventManager::isEmptyTray()) {
      EventManager::Event event = EventManager::popFromTray();

      DynamicJsonDocument eventJson(1024);
      eventJson["method"] = "event";
      eventJson["data"]["topic"] = event.getTopic();
      eventJson["data"]["data"] = event.getData();

      String eventString;
      serializeJson(eventJson, eventString);
      rpiEndpoint->unleash(eventString);
      serialEndpoint->unleash(eventString);
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

    if (!NotificationManager::getNotificationTray()->isEmpty()) {
      if (millis() - notiTimestamp <= 1000) {
        continue;// do nothing
      }
      notiTimestamp = millis();

      DynamicJsonDocument notiJson(1024);
      notiJson["method"] = "notification";
      JsonArray notiArray = notiJson.createNestedArray("data");
      std::vector<Notification *> lNotifications = NotificationManager::getNotificationTray()->getList();
      for (int i = 0;
           i < notificationFrame && currentNotificationIndex < lNotifications.size(); i++, currentNotificationIndex++) {
        JsonObject notiObj = notiArray.createNestedObject();
        notiObj["id"] = lNotifications[currentNotificationIndex]->getNotificationId();
        notiObj["type"] = NotificationTypeToString(lNotifications[currentNotificationIndex]->getNotificationType());
        NotificationType type = lNotifications[currentNotificationIndex]->getNotificationType();
        switch (type) {
          case NOTI_GPIO: {
            auto *gpioNotification = (GpioNotification *) lNotifications[currentNotificationIndex];
            notiObj["data"]["ts"] = gpioNotification->getDateTime().unixtime();
            notiObj["data"]["ch"] = gpioNotification->getChannel();
            notiObj["data"]["state"] = gpioNotification->getStatus() == HIGH;
          }
          default: { ;
          }
        }
      }

      if (currentNotificationIndex >= lNotifications.size()) {
        currentNotificationIndex = 0;
      }

      String notiString;
      serializeJson(notiJson, notiString);
      rpiEndpoint->unleash(notiString);
      serialEndpoint->unleash(notiString);
    }

#ifdef SG_COMMUNICATION_SIMPLEX
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
#ifdef SG_MODE_DEVELOPMENT
    if (millis() - simplexTimestamp >= 2000) {
      DevSimplexStationTask::instance()->runningTask(context);
      simplexTimestamp = millis();
    }
#endif

#endif
    delay(1);
  }
}

void loop() {
  controlScheduler.execute();
  backgroundScheduler.execute();
}
