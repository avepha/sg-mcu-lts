#include "domain/permission.h"
#include "validationError.h"
#include "./util/resolveControlEnum.h"
#ifndef SG_MCU_CONTROL_PERMISSION_H
#define SG_MCU_CONTROL_PERMISSION_H

class permission_control_type_save : public Permission {
public:
  explicit permission_control_type_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["type"].isNull() || !reqData["type"].is<String>()) {
      InvalidInputError err("type field must be a string");
      throw err;
    }

    if (ControlStringToEnum(reqData["type"]) == CTRL_UNKNOWN) {
      InvalidInputError err("Unknown control");
      throw err;
    }
  }
};

#endif //SG_MCU_CONTROL_PERMISSION_H
