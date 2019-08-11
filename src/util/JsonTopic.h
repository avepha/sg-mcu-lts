//
// Created by Alfarie-MBP on 2019-08-11.
//
#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_H
#define SG_MCU_JSON_H


class JsonTopic {
public:
  JsonTopic(String topic, String method, JsonObject data);
  JsonTopic(String topic, String method);

  String toString();
  StaticJsonDocument<256> toStaticJsonObject();
private:
  String topic;
  String method;
  JsonObject data;
};
JsonTopic::JsonTopic(String topic, String method):
    topic(topic),
    method(method)
    {};

JsonTopic::JsonTopic(String topic, String method, JsonObject data):
  topic(topic),
  method(method),
  data(data) {};

StaticJsonDocument<256> JsonTopic::toStaticJsonObject() {
  StaticJsonDocument<256> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!data.isNull())
    json["data"] = data;

  return json;
}

String JsonTopic::toString() {
  StaticJsonDocument<1024> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!data.isNull())
    json["data"] = data;

  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}


#endif //SG_MCU_JSON_H
