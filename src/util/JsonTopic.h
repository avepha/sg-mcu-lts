//
// Created by Alfarie-MBP on 2019-08-11.
//
#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_TOPIC_H
#define SG_MCU_JSON_TOPIC_H


class JsonTopic {
public:
  JsonTopic(String topic, String method);

  JsonTopic(String topic, String method, String data);

  JsonTopic(String topic, String method, JsonObject data);

  String toString();

  StaticJsonDocument<1024> toStaticJsonObject();

private:
  String topic;
  String method;
  JsonObject dataJson;
  String dataString;
};

JsonTopic::JsonTopic(String topic, String method) :
    topic(topic),
    method(method) {};

JsonTopic::JsonTopic(String topic, String method, String data) :
    topic(topic),
    method(method),
    dataString(data) {};

JsonTopic::JsonTopic(String topic, String method, JsonObject data) :
    topic(topic),
    method(method),
    dataJson(data) {};

StaticJsonDocument<1024> JsonTopic::toStaticJsonObject() {
  StaticJsonDocument<1024> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!dataJson.isNull())
    json["data"] = dataJson;

  return json;
}

String JsonTopic::toString() {
  StaticJsonDocument<1024> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!dataJson.isNull())
    json["data"] = dataJson;
  else
    json["data"] = dataString;

  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}


#endif //SG_MCU_JSON_H
