#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_RANGE_PERMISSION_H
#define SG_MCU_RANGE_PERMISSION_H

class permission_range_save : public Permission {
public:
  explicit permission_range_save(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    if (reqData["range"].isNull()) {
      InvalidInputError err("range is not specified.");
      throw err;
    }

    if (!reqData["range"].is<JsonObject>()) {
      InvalidInputError err("range must be an object.");
      throw err;
    }

    JsonObject range = reqData["range"];
    if (range["sensor"].isNull()) {
      InvalidInputError err("sensor is not specified");
      throw err;
    }

    if (!range["sensor"].is<int>()) {
      InvalidInputError err("sensor must be type of integer");
      throw err;
    }

    SensorSchema sensorSchema = context->sensor->model->get();
    if (range["sensor"] < 0 || range["sensor"] > sensorSchema.numberOfSensor) {
      InvalidInputError err("sensor index out of bound");
      throw err;
    }

    if (range["upper"].isNull()) {
      InvalidInputError err("upper is not specified");
      throw err;
    }

    if (!range["upper"].is<float>()) {
      InvalidInputError err("upper must be type of double");
      throw err;
    }

    if (range["lower"].isNull()) {
      InvalidInputError err("lower is not specified");
      throw err;
    }

    if (!range["lower"].is<float>()) {
      InvalidInputError err("lower must be type of double");
      throw err;
    }

    if (!range["greater"].is<bool>()) {
      InvalidInputError err("Greater field must be boolean");
      throw err;
    }

    if (!range["timing"].isNull() && range["timing"].is<JsonObject>()) {
      if (range["timing"]["working"].isNull()) {
        InvalidInputError err("timing.working is not specified");
        throw err;
      }
      if (range["timing"]["waiting"].isNull()) {
        InvalidInputError err("timing.waiting is not specified");
        throw err;
      }
    }
  }
};

class permission_range : public Permission {
public:
  explicit permission_range(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
