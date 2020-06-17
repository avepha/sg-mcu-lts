#include "./notification.h"
#include "./notificationTray.h"

#include "./gpioNotification.h"

#ifndef SG_MCU_NOTIFICATION_MANAGER_H
#define SG_MCU_NOTIFICATION_MANAGER_H

class NotificationManager {
public:
  static NotificationTray *getNotificationTray() {
    return tray;
  }

  static void setNotificationTray(NotificationTray *_tray) {
    tray = _tray;
  }

  static void addNotification(Notification *noti) {
    noti->setNotificationId(currentId++);
    tray->add(noti);

    if (currentId >= 50) {
      currentId = 1;
    }
  }

  static boolean removeByNotificationId(uint16_t id) {
    if (tray->getList().empty()) {
      return false;
    }

    std::vector<Notification *> lNotifications = tray->getList();
    for (int i = 0 ; i < lNotifications.size(); i++) {
      if (lNotifications[i]->getNotificationId() == id) {
        tray->remove(lNotifications[i]);
        return true;
      }
    }

    return false;
  }

  static Notification* getNotificationById(uint16_t id) {
    if (tray->getList().empty()) {
      return nullptr;
    }

    std::vector<Notification *> lNotifications = tray->getList();
    for (int i = 0 ; i < lNotifications.size(); i++) {
      if (lNotifications[i]->getNotificationId() == id) {
        return lNotifications[i];
      }
    }

    return nullptr;
  }

private:
private:
  static uint32_t currentId;
  static NotificationTray *tray;
};

uint32_t NotificationManager::currentId = 1;
NotificationTray* NotificationManager::tray = nullptr;
#endif
