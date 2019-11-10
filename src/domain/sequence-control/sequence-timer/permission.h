#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_SEQUENCE_TIMER_PERMISSION_H
#define SG_MCU_SEQUENCE_TIMER_PERMISSION_H

class permission_sequence_timer_save : public Permission {
public:
  explicit permission_sequence_timer_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["timers"].isNull()) {
      InvalidInputError err("timers field must not be null");
      throw err;
    }

    if (!reqData["timers"].is<JsonArray>()) {
      InvalidInputError err("timers field must be an array");
      throw err;
    }

    if (reqData["timers"].size() <= 0) {
      InvalidInputError err("timers field must not be an empty array");
      throw err;
    }
  }
};

#endif //SG_MCU_SEQUENCE_TIMER_PERMISSION_H