#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_TOPIC_H
#define SG_MCU_JSON_TOPIC_H


class JsonRequest {
public:
  JsonRequest(String topic, String method, String reqId);

  JsonRequest(String topic, String method, JsonDocument data, String reqId);

  String toString();

  DynamicJsonDocument toJson();

private:
  String topic;
  String method;
  StaticJsonDocument<2048> data;
  String reqId = "";
};

JsonRequest::JsonRequest(String topic, String method, String reqId = "") :
    topic(topic),
    method(method),
    reqId(reqId)
    {};

JsonRequest::JsonRequest(String topic, String method, JsonDocument data, String reqId = "") :
    topic(topic),
    method(method),
    data(data),
    reqId(reqId)
    {};

DynamicJsonDocument JsonRequest::toJson() {
  DynamicJsonDocument json(2048);
  json["topic"] = topic;
  json["method"] = method;
  json["reqId"] = reqId == "" ? (char*)0 : reqId;

  if (!data.isNull())
    json["data"] = data;

  return json;
}

String JsonRequest::toString() {
  DynamicJsonDocument json(2048);
  json["topic"] = topic;
  json["method"] = method;
  json["reqId"] = reqId;

  if (!data.isNull())
    json["data"] = data;

  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}

#endif //SG_MCU_JSON_H
