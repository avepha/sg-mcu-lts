#include "domain/permission.h"
#include "validationError.h"
#include "logger/log.h"

#ifndef SG_MCU_CONFIGURATION_PERMISSION_H
#define SG_MCU_CONFIGURATION_PERMISSION_H

class permission_log_level_save : public Permission {
public:
  explicit permission_log_level_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["level"].isNull()) {
      InvalidInputError err("level field must not be null");
      throw err;
    }

    if (!reqData["level"].is<String>()) {
      InvalidInputError err("level field must be String");
      throw err;
    }

    const String level = reqData["level"];
    if (StringToLoggerLevelEnum(level) == NONE) {
      InvalidInputError err("Invalid level");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
