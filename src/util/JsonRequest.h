#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_TOPIC_H
#define SG_MCU_JSON_TOPIC_H


class JsonRequest {
public:
  JsonRequest(String topic, String method, int reqId = 0);

  JsonRequest(String topic, String method, String data, int reqId = 0);

  JsonRequest(String topic, String method, JsonObject data, int reqId = 0);

  JsonRequest(String topic, String method, JsonArray data, int reqId = 0);

  String toString();

  StaticJsonDocument<2048> toStaticJsonObject();

private:
  String topic;
  String method;
  JsonObject dataJson;
  JsonArray dataJsonArray;
  String dataString;
  int reqId;
};

JsonRequest::JsonRequest(String topic, String method, int reqId) :
    topic(topic),
    method(method),
    reqId(reqId)
    {};

JsonRequest::JsonRequest(String topic, String method, String data, int reqId) :
    topic(topic),
    method(method),
    dataString(data),
    reqId(reqId)
    {};

JsonRequest::JsonRequest(String topic, String method, JsonObject data, int reqId) :
    topic(topic),
    method(method),
    dataJson(data),
    reqId(reqId)
    {};

JsonRequest::JsonRequest(String topic, String method, JsonArray data, int reqId) :
    topic(topic),
    method(method),
    dataJsonArray(data),
    reqId(reqId)
    {};

StaticJsonDocument<2048> JsonRequest::toStaticJsonObject() {
  StaticJsonDocument<2048> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!dataJson.isNull())
    json["data"] = dataJson;

  return json;
}

String JsonRequest::toString() {
  StaticJsonDocument<2048> json;
  json["topic"] = topic;
  json["method"] = method;
  json["reqId"] = reqId;

  if (!dataJson.isNull())
    json["data"] = dataJson;
  else if(!dataJsonArray.isNull())
    json["data"] = dataJsonArray;
  else
    json["data"] = dataString;

  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}

#endif //SG_MCU_JSON_H
