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
class mutation_sensor_order_save : public Resolvers {
public:
  explicit mutation_sensor_order_save(CombineContext *context) : Resolvers("sensor_order_save", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"]["names"].isNull()) {
      InvalidInputError err;
      return err.toJsonString();
    }
    JsonArray names = reqJson["data"]["names"].as<JsonArray>();

    SensorSchema sensorSchema = context->sensorContext->model->get();
    for (int i = 0 ; i < names.size(); i++) {
      String name = names[i].as<String>();
      name.toCharArray(sensorSchema.names[i], name.length() + 1);
    }

    int writeOps = context->sensorContext->model->save(sensorSchema);

    delay(10);
    SensorSchema newSchema = context->sensorContext->model->get();
    StaticJsonDocument<300> data;
    data["writeOps"] = writeOps;
    JsonArray newNames = data.createNestedArray("names");
    for (int i = 0 ; i < sizeof(newSchema.names) / sizeof(newSchema.names[0]); i++) {
      newNames.add(String(newSchema.names[i]));
    }

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

// @query: date
class query_sensor : public Resolvers {
public:
  explicit query_sensor(CombineContext *context) : Resolvers("sensor", context) {};

  String resolve(JsonObject reqJson) override {
    SensorSchema sensorNames = context->sensorContext->model->get();
    float *sensors = context->sensorContext->core->getSensors();

    StaticJsonDocument<256> data;
    for(int i = 0; i < sizeof(sensorNames.names) / sizeof(sensorNames.names[0]); i++) {
      data[sensorNames.names[i]] = sensors[i];
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
    delay(10);

    StaticJsonDocument<256> data;
    JsonArray names = data.createNestedArray("names");
    for(int i = 0; i < sizeof(sensorNames.names) / sizeof(sensorNames.names[0]); i++) {
      names.add(sensorNames.names[i]);
    }

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
