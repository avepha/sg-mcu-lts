//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "util/util.h"

#include "../resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_SENSOR_RESOLVERS_H
#define SG_MCU_SENSOR_RESOLVERS_H

// @mutation: date_save
//class mutation_date_save : public Resolvers {
//public:
//  explicit mutation_date_save(CombineContext *context) : Resolvers("date_save", context) {};
//
//  String resolve(JsonObject json) override {
//    if (json["data"]["date"].isNull()) {
//      return (new InvalidInputError("Date is not specified"))->toJsonString();
//    }
//
//    DateTime newDate(IsoStringToDateTime(json["data"]["date"]));
//
//    context->rtcContext->core->setDate(newDate);
//    DateTime dateTime = context->rtcContext->core->getDate();
//
//    StaticJsonDocument<1024> result;
//    String jsonString;
//    result["topic"] = "date_save";
//    result["method"] = "mutation";
//    result["data"] = DateTimeToIsoString(dateTime);
//
//    serializeJson(json, jsonString);
//    return jsonString;
//  };
//};

// @query: date
class query_sensor : public Resolvers {
public:
  explicit query_sensor(CombineContext *context) : Resolvers("sensor", context) {};

  String resolve(JsonObject reqJson) override {
    SensorSchema sensorNames = context->sensorContext->model->get();
    float *sensors = context->sensorContext->core->getSensors();

    StaticJsonDocument<256> data;
    for(int i = 0; i < sensorNames.name->length(); i++) {
      data[sensorNames.name[i]] = sensors[i];
    }

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

class query_sensor_order : public Resolvers {
public:
  explicit query_sensor_order(CombineContext *context) : Resolvers("sensor_order", context) {};

  String resolve(JsonObject reqJson) override {
    SensorSchema sensorNames = context->sensorContext->model->get();

    StaticJsonDocument<256> data;
    JsonArray names = data.createNestedArray("names");
    for(int i = 0; i < sensorNames.name->length(); i++) {
      names.add(sensorNames.name[i]);
    }

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
