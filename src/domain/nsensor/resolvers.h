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

  String resolve(JsonObject reqJson) override {
    SensorSchema sensorSchema = context->sensorContext->model->get();
    NSensor *nodes = context->nSensorContext->core->getNSensor();

    StaticJsonDocument<2048> data;
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

    JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonArray>());
    return response.toString();
  };
};

class query_nsensor : public Resolvers {
public:
  explicit query_nsensor(CombineContext *context) : Resolvers("nsensor", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull()) {
      InvalidInputError err("index or timers field is not specified.");
      return err.toJsonString();
    }

    if (reqJson["data"]["index"].as<int>() < 0 || reqJson["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = reqJson["data"]["index"];
    SensorSchema sensorSchema = context->sensorContext->model->get();
    NSensor *nodes = context->nSensorContext->core->getNSensor();

    StaticJsonDocument<512> data;
    data["index"] = index;
    data["lastSeen"] = nodes[index].lastSeen;
    data["isAlive"] = millis() - nodes[index].lastSeen < 10000 && nodes[index].lastSeen != 0;

    JsonObject sensorObj = data.createNestedObject("sensors");
    for(int i = 0; i < sensorSchema.numberOfSensor; i++) {
      sensorObj[sensorSchema.names[i]] = nodes[index].sensors[i];
    }

    JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return response.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
