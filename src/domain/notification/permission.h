#include "domain/permission.h"
#include "validationError.h"
#include "logger/log.h"

#include "./notificationManager.h"

#ifndef SG_MCU_NOTIFICATION_PERMISSION_H
#define SG_MCU_NOTIFICATION_PERMISSION_H

class permission_notification_ack : public Permission {
public:
  explicit permission_notification_ack() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["id"].isNull()) {
      InvalidInputError err("id field must not be null");
      throw err;
    }

    if (!reqData["id"].is<int>()) {
      InvalidInputError err("id field must be an integer");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
