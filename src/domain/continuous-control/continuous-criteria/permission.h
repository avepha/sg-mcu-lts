#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_CONTINUOUS_CRITERIA_PERMISSION_H
#define SG_MCU_CONTINUOUS_CRITERIA_PERMISSION_H

class permission_continuous_criteria_save : public Permission {
public:
  explicit permission_continuous_criteria_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {

    if (reqData["criteria"].isNull()) {
      InvalidInputError err("criteria is not specified.");
      throw err;
    }

    if (!reqData["criteria"].is<JsonObject>()) {
      InvalidInputError err("criteria must be an object.");
      throw err;
    }

    JsonObject criteria = reqData["criteria"];
    if (criteria["sensor"].isNull()) {
      InvalidInputError err("sensor is not specified");
      throw err;
    }

    if (!criteria["sensor"].is<int>()) {
      InvalidInputError err("sensor must be type of integer");
      throw err;
    }

    SensorSchema sensorSchema = context->sensor->model->get();
    if (criteria["sensor"] < 0 || criteria["sensor"] > sensorSchema.numberOfSensor) {
      InvalidInputError err("sensor index out of bound");
      throw err;
    }

    if (criteria["criteria"].isNull()) {
      InvalidInputError err("criteria is not specified");
      throw err;
    }

    if (!criteria["criteria"].is<float>()) {
      InvalidInputError err("sensor must be type of double");
      throw err;
    }

    if (!criteria["greater"].is<bool>()) {
      InvalidInputError err("Greater field must be boolean");
      throw err;
    }

    if (!criteria["timing"].isNull()) {
      if (criteria["timing"]["working"].isNull()) {
        InvalidInputError err("timing.working is not specified");
        throw err;
      }
      if (criteria["timing"]["waiting"].isNull()) {
        InvalidInputError err("timing.waiting is not specified");
        throw err;
      }
    }
  }
};

#endif //SG_MCU_CONTINUOUS_CRITERIA_PERMISSION_H
