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

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["names"].isNull()) {
      InvalidInputError err;
      throw err;
    }
    JsonArray names = reqData["names"].as<JsonArray>();

    SensorSchema sensorSchema = context->sensorContext->model->get();
    sensorSchema.numberOfSensor = names.size();
    for (int i = 0 ; i < names.size(); i++) {
      String name = names[i].as<String>();
      name.toCharArray(sensorSchema.names[i], name.length() + 1);
    }

    int writeOps = context->sensorContext->model->save(sensorSchema);

    delay(10);
    SensorSchema newSchema = context->sensorContext->model->get();
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
class query_sensor : public Resolvers {
public:
  explicit query_sensor(CombineContext *context) : Resolvers("sensor", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    SensorSchema sensorNames = context->sensorContext->model->get();
    float *sensors = context->sensorContext->core->getSensors();

    DynamicJsonDocument data(256);
    for(int i = 0; i < sensorNames.numberOfSensor; i++) {
      data[sensorNames.names[i]] = sensors[i];
    }

    return data;
  };
};

class query_sensor_order : public Resolvers {
public:
  explicit query_sensor_order(CombineContext *context) : Resolvers("sensor_order", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    SensorSchema sensorNames = context->sensorContext->model->get();
    delay(10);

    DynamicJsonDocument data(256);
    JsonArray names = data.createNestedArray("names");
    for(int i = 0; i < sensorNames.numberOfSensor; i++) {
      names.add(sensorNames.names[i]);
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
