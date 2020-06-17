#include <Arduino.h>
#include <utility>
#include "./notificationUtils.h"

#ifndef SG_MCU_NOTIFICATION_H
#define SG_MCU_NOTIFICATION_H

class Notification {
public:
  Notification(uint16_t id, NotificationType type) : notificationId(id),
                                                     notificationType(type) {};

  explicit Notification(NotificationType type) : notificationId(0),
                                                 notificationType(type) {};

  void setNotificationId(uint16_t id) {
    notificationId = id;
  }

  uint16_t getNotificationId();

  NotificationType getNotificationType();

private:
  uint16_t notificationId;
  NotificationType notificationType;
};

uint16_t Notification::getNotificationId() {
  return this->notificationId;
}

NotificationType Notification::getNotificationType() {
  return this->notificationType;
}

#endif
