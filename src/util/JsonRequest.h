#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_TOPIC_H
#define SG_MCU_JSON_TOPIC_H


class JsonRequest {
public:
  JsonRequest(String topic, String method, int reqId = 0);

  JsonRequest(String topic, String method, JsonDocument data, int reqId = 0);

  String toString();

  StaticJsonDocument<2048> toStaticJsonObject();

private:
  String topic;
  String method;
  StaticJsonDocument<2048> data;
  int reqId;
};

JsonRequest::JsonRequest(String topic, String method, int reqId) :
    topic(topic),
    method(method),
    reqId(reqId)
    {};

JsonRequest::JsonRequest(String topic, String method, JsonDocument data, int reqId) :
    topic(topic),
    method(method),
    data(data),
    reqId(reqId)
    {};

StaticJsonDocument<2048> JsonRequest::toStaticJsonObject() {
  StaticJsonDocument<2048> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!data.isNull())
    json["data"] = data;

  return json;
}

String JsonRequest::toString() {
  StaticJsonDocument<2048> json;
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
