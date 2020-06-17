#include <RTClib.h>
#include "./notification.h"
#include "domain/rtc/core.h"

#ifndef SG_MCU_GPIONOTIFICATION_H
#define SG_MCU_GPIONOTIFICATION_H

class GpioNotification : public Notification {
public:
  explicit GpioNotification(uint16_t _channel, uint16_t _status) : Notification(NOTI_GPIO) {
    channel = _channel;
    dateTime = RtcCore::instance()->getUtcDate();
    status = _status;
  };

  DateTime getDateTime() {
    return this->dateTime;
  }

  uint16_t getChannel() {
    return channel;
  }

  int getStatus() {
    return status;
  }

private:
  DateTime dateTime;
  uint16_t channel;
  int status;
};

#endif
