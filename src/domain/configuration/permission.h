#include "domain/permission.h"
#include "validationError.h"
#include "domain/channel-control/util/resolveControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CONFIGURATION_PERMISSION_H
#define SG_MCU_CONFIGURATION_PERMISSION_H

class permission_debug_save : public Permission {
public:
  explicit permission_debug_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["debug"].isNull()) {
      InvalidInputError err("debug field must not be null");
      throw err;
    }

    if (!reqData["debug"].is<bool>()) {
      InvalidInputError err("debug field must be boolean");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
