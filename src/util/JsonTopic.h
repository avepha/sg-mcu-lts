#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef SG_MCU_JSON_TOPIC_H
#define SG_MCU_JSON_TOPIC_H


class JsonTopic {
public:
  JsonTopic(String topic, String method);

  JsonTopic(String topic, String method, String data);

  JsonTopic(String topic, String method, JsonObject data);

  JsonTopic(String topic, String method, JsonArray data);

  String toString();

  StaticJsonDocument<1024> toStaticJsonObject();

private:
  String topic;
  String method;
  JsonObject dataJson;
  JsonArray dataJsonArray;
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

JsonTopic::JsonTopic(String topic, String method, JsonArray data) :
    topic(topic),
    method(method),
    dataJsonArray(data) {
//  serializeJsonPretty(dataJsonArray, Serial);
};

StaticJsonDocument<1024> JsonTopic::toStaticJsonObject() {
  StaticJsonDocument<1024> json;
  json["topic"] = topic;
  json["method"] = method;

  if (!dataJson.isNull())
    json["data"] = dataJson;

  return json;
}

String JsonTopic::toString() {
  StaticJsonDocument<2048> json;
  json["topic"] = topic;
  json["method"] = method;

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
