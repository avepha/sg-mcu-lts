#include "util/util.h"

#include "../resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_SENSOR_RESOLVERS_H
#define SG_MCU_SENSOR_RESOLVERS_H

// @mutation: date_save
class mutation_sensor_order_save : public Mutation {
public:
  explicit mutation_sensor_order_save() : Mutation("sensor_order_save") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["names"].isNull()) {
      InvalidInputError err;
      throw err;
    }
    JsonArray names = reqData["names"].as<JsonArray>();

    SensorSchema sensorSchema = context->sensor->model->get();
    sensorSchema.numberOfSensor = names.size();
    for (int i = 0 ; i < names.size(); i++) {
      String name = names[i].as<String>();
      name.toCharArray(sensorSchema.names[i], name.length() + 1);
    }

    int writeOps = context->sensor->model->save(sensorSchema);

    delay(10);
    SensorSchema newSchema = context->sensor->model->get();
    DynamicJsonDocument data(300);
    data["writeOps"] = writeOps;
    JsonArray newNames = data.createNestedArray("names");
    for (int i = 0 ; i < newSchema.numberOfSensor; i++) {
      newNames.add(String(newSchema.names[i]));
    }

    return data;
  };
};

// @query: date
class query_sensor : public Query {
public:
  explicit query_sensor() : Query("sensor") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SensorSchema sensorNames = context->sensor->model->get();
    float *sensors = context->sensor->core->getSensors();
    DynamicJsonDocument data(256);
    for(int i = 0; i < sensorNames.numberOfSensor; i++) {
      data[sensorNames.names[i]] = sensors[i];
    }
    return data;
  };
};

class query_sensor_order : public Query {
public:
  explicit query_sensor_order() : Query("sensor_order") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SensorSchema sensorNames = context->sensor->model->get();

    DynamicJsonDocument data(256);
    JsonArray names = data.createNestedArray("names");
    for(int i = 0; i < sensorNames.numberOfSensor; i++) {
      names.add(sensorNames.names[i]);
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
