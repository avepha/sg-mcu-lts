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
    SensorSchema sensorSchema = context->sensorContext->model->get();
    NSensor *nodes = context->nSensorContext->core->getNSensor();

    DynamicJsonDocument data(2048);
    for (int i = 0; i < 8; i++) {
      if( millis() - nodes[i].lastSeen < 10000 && nodes[i].lastSeen != 0) {
        JsonObject dataObj = data.createNestedObject();
        dataObj["index"] = i;
        dataObj["lastSeen"] = nodes[i].lastSeen;
        dataObj["isAlive"] = millis() - nodes[i].lastSeen < 10000 && nodes[i].lastSeen != 0;

        JsonObject sensorObj = dataObj.createNestedObject("sensors");
        for(int j = 0; j < sensorSchema.numberOfSensor; j++) {
          sensorObj[sensorSchema.names[j]] = nodes[i].sensors[j];
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
      InvalidInputError err("index or timers field is not specified.");
      throw err;
    }

    if (reqData["index"].as<int>() < 0 || reqData["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    SensorSchema sensorSchema = context->sensorContext->model->get();
    NSensor *nodes = context->nSensorContext->core->getNSensor();

    DynamicJsonDocument data(512);
    data["index"] = index;
    data["lastSeen"] = nodes[index].lastSeen;
    data["isAlive"] = millis() - nodes[index].lastSeen < 10000 && nodes[index].lastSeen != 0;

    JsonObject sensorObj = data.createNestedObject("sensors");
    for(int i = 0; i < sensorSchema.numberOfSensor; i++) {
      sensorObj[sensorSchema.names[i]] = nodes[index].sensors[i];
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
