#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_P_SENSOR_PERMISSION_H
#define SG_MCU_P_SENSOR_PERMISSION_H

class permission_sensor : public Permission {
public:
  explicit permission_sensor() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["id"].isNull()) {
      InvalidInputError err("id must not be null");
      throw err;
    }

    if (!reqData["id"].is<int>()) {
      InvalidInputError err("id must be an integer");
      throw err;
    }

    uint8_t id = reqData["id"];
    bool flag = false;
    for (const auto &kv : Sensor::SENSORMAP) {
      if (kv.second == id) {
       flag = true;
      }
    }
    if (!flag) {
      SensorNotFoundError err;
      throw err;
    }
  }
};

#endif //SG_MCU_CONTROL_PERMISSION_H
