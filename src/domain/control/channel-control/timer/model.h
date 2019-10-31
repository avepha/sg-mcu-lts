#include "domain/model.h"

#ifndef SG_MCU_CONTROL_TIMER_MODEL_H
#define SG_MCU_CONTROL_TIMER_MODEL_H

#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

const int CONTROL_TIMER_ROM_ADDR = 1000;

struct TimerSchema {
  struct timer {
    uint8_t size = 2;
    uint16_t data[10][2] = {{6 * HOUR_IN_MINUTE, 12 * HOUR_IN_MINUTE}, {13 * HOUR_IN_MINUTE, 18 * HOUR_IN_MINUTE}};
  } timers[8];
}; // total size: 336

class TimerModel : public RomModel<TimerSchema> {
public:
  TimerModel() : RomModel(CONTROL_TIMER_ROM_ADDR, "TimerModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
