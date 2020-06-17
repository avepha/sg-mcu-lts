#include "./notification.h"

#ifndef SG_MCU_NOTIFICATION_TRAY_H
#define SG_MCU_NOTIFICATION_TRAY_H

class NotificationTray {
public:
  void add(Notification *noti) {
    if (lNotifications.size() >= MAX_TRAY) {
      lNotifications.erase(lNotifications.begin());
    }
    lNotifications.push_back(noti);
  }

  void remove(Notification * pos) {
    if (pos == nullptr) {
      return;
    }

    for (int i = 0 ;i  < lNotifications.size(); i++) {
      if (pos->getNotificationId() == lNotifications[i]->getNotificationId()) {
        lNotifications.erase(lNotifications.begin() + i);
      }
    }
    delete pos;
  }

  std::vector<Notification *> getList() {
    return lNotifications;
  }

  bool isEmpty() {
    return this->lNotifications.empty();
  }

private:
  static uint8_t MAX_TRAY;
  std::vector<Notification *> lNotifications{};
};

uint8_t NotificationTray::MAX_TRAY = 12;

#endif
