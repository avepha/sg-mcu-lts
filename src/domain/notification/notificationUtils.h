#include <Arduino.h>

#ifndef SG_MCU_NOTIFICATION_UTILS_H
#define SG_MCU_NOTIFICATION_UTILS_H

enum NotificationType {
  NOTI_GPIO = 0,
  NOTI_NONE = -1,
};

NotificationType StringToNotificationType(String st) {
  if (st == "gpio")
    return NOTI_GPIO;
  else
    return NOTI_NONE;
}

String NotificationTypeToString(NotificationType type) {
  switch (type) {
    case NOTI_GPIO:
      return "gpio";
    case NOTI_NONE:
      return "none";
  }
  return "none";
}

#endif
