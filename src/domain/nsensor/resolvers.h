#include "util/util.h"

#include "domain/resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_NSENSOR_RESOLVERS_H
#define SG_MCU_NSENSOR_RESOLVERS_H

// @query: date
class query_nsensors : public Resolvers {
public:
  explicit query_nsensors(CombineContext *context) : Resolvers("nsensors", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    std::array<NSensor, 8> nodes = context->nsensors->core->getNSensor();
    SensorSchema sensorSchema = context->sensor->model->get();

    DynamicJsonDocument data(2048);

    if (reqData["withSensor"].isNull() || reqData["withSensor"]) {
      for (int i = 0; i < 8; i++) {
        if (millis() - nodes[i].timeStamp < EXPECTED_ACTIVE_DIFF_TIME && nodes[i].timeStamp != 0) {
          JsonObject dataObj = data.createNestedObject();
          dataObj["index"] = i;
          dataObj["lastSeen"] = millis() - nodes[i].timeStamp;
          dataObj["isAlive"] = millis() - nodes[i].timeStamp < EXPECTED_ACTIVE_DIFF_TIME && nodes[i].timeStamp != 0;

          JsonObject sensorObj = dataObj.createNestedObject("sensors");
          for (int j = 0; j < sensorSchema.numberOfSensor; j++) {
            sensorObj[sensorSchema.names[j]] = nodes[i].sensors[j];
          }
        }
      }
    } else if (reqData["withSensor"] == false) {
      for (int i = 0; i < 8; i++) {
        if (millis() - nodes[i].timeStamp < EXPECTED_ACTIVE_DIFF_TIME && nodes[i].timeStamp != 0) {
          JsonObject dataObj = data.createNestedObject();
          dataObj["index"] = i;
          dataObj["lastSeen"] = millis() - nodes[i].timeStamp;
          dataObj["isAlive"] = millis() - nodes[i].timeStamp < EXPECTED_ACTIVE_DIFF_TIME && nodes[i].timeStamp != 0;

          JsonArray sensorObj = dataObj.createNestedArray("sensors");
          for (int j = 0; j < sensorSchema.numberOfSensor; j++) {
            sensorObj.add(nodes[i].sensors[j]);
          }
        }
      }
    }
    return data;
  };
};

class query_nsensor : public Resolvers {
public:
  explicit query_nsensor(CombineContext *context) : Resolvers("nsensor", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index field is not specified.");
      throw err;
    }

    if (reqData["index"].as<int>() < 0 || reqData["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    SensorSchema sensorSchema = context->sensor->model->get();
    std::array<NSensor, 8> nodes = context->nsensors->core->getNSensor();

    DynamicJsonDocument data(512);
    data["index"] = index;
    data["lastSeen"] = millis() - nodes[index].timeStamp;
    data["isAlive"] = millis() - nodes[index].timeStamp < EXPECTED_ACTIVE_DIFF_TIME && nodes[index].timeStamp != 0;

    JsonObject sensorObj = data.createNestedObject("sensors");
    for (int i = 0; i < sensorSchema.numberOfSensor; i++) {
      sensorObj[sensorSchema.names[i]] = nodes[index].sensors[i];
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
