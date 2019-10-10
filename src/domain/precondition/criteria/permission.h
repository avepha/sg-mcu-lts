#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_PERMISSION_H
#define SG_MCU_PRECONDITION_CRITERIA_PERMISSION_H

class permission_precondition_criteria_save : public Permission {
public:
  explicit permission_precondition_criteria_save(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    if (reqData["sensor"].isNull()) {
      InvalidInputError err("sensor is not specified");
      throw err;
    }

    if (!reqData["sensor"].is<int>()) {
      InvalidInputError err("sensor must be type of integer");
      throw err;
    }

    SensorSchema sensorSchema = context->sensor->model->get();
    if (reqData["sensor"] < 0 || reqData["sensor"] > sensorSchema.numberOfSensor) {
      InvalidInputError err("sensor index out of bound");
      throw err;
    }

    if (reqData["criteria"].isNull()) {
      InvalidInputError err("criteria is not specified");
      throw err;
    }

    if (!reqData["criteria"].is<float>()) {
      InvalidInputError err("sensor must be type of double");
      throw err;
    }

    if (!reqData["greater"].is<bool>()) {
      InvalidInputError err("Greater field must be boolean");
      throw err;
    }
  }
};

class permission_precondition_criteria : public Permission {
public:
  explicit permission_precondition_criteria(CombineContext *context) : Permission(context) {};

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
