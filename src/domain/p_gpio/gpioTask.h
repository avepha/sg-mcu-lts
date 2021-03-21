//
// Created by Farhan Poharsae on 20/3/2021 AD.
//

#include <TaskScheduler.h>
#include "./gpioState.h"
#include "logger/log.h"

#ifndef SG_MCU_LTS_GPIOTASK_H
#define SG_MCU_LTS_GPIOTASK_H

class PGpioTask {
public:
  uint8_t channel;
  uint8_t state = LOW;

  static uint8_t setState(uint8_t ch, uint8_t state) {
    digitalWrite(CHANNEL_GPIO_MAP[ch], state);
    GpioState::instance()->setState(ch, state);
    Log::info("gpio", "gpio channel " + String(ch) + " -> " + String(state == HIGH ? "HIGH" : "LOW"));
  }

  explicit PGpioTask(uint8_t ch) {
    gpioState = GpioState::instance();

    channel = ch;
    pinMode(ch, OUTPUT);
  }

  void high() {
    state = HIGH;
    digitalWrite(CHANNEL_GPIO_MAP[channel], state);
    gpioState->setState(channel, state);
    Log::info("gpio", "gpio channel " + String(channel) + " -> HIGH");

  }

  void low() {
    state = LOW;
    digitalWrite(CHANNEL_GPIO_MAP[channel], state);
    gpioState->setState(channel, state);
    Log::info("gpio", "gpio channel " + String(channel) + " -> LOW");
  }

private:
  GpioState *gpioState;
};


#endif //SG_MCU_LTS_GPIOTASK_H
