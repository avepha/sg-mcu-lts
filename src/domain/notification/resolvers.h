#include "domain/mutation.h"
#include "validationError.h"
#include "combineContext.h"
#include "./permission.h"
#include "./notificationManager.h"

#include "util/util.h"

#ifndef SG_MCU_NOTIFICATION_RESOLVERS_H
#define SG_MCU_NOTIFICATION_RESOLVERS_H

class mutation_notification_ack : public Mutation {
public:
  explicit mutation_notification_ack() : Mutation("notification_ack", new permission_notification_ack) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(1024);
    uint16_t id = reqData["id"];

    auto *noti = NotificationManager::getNotificationById(id);

    if (noti != nullptr) {
      data["op"] = "updated";
      NotificationManager::removeByNotificationId(id);
    }
    else {
      data["op"] = "noop";
    }
    return data;
  }
};

#endif //SG_MCU_RESOLVERS_H
