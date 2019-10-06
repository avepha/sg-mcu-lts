#include "domain/model.h"

#ifndef SG_MCU_CONTROL_TIMER_MODEL_H
#define SG_MCU_CONTROL_TIMER_MODEL_H

#ifndef HOUR_IN_SECOND
#define HOUR_IN_SECOND 3600
#endif

const int CONTROL_TIMER_ROM_ADDR = 1200;

struct TimerSchema {
  struct timer {
    int size = 2;
    unsigned long data[10][2] = {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}};
  } timers[8];
}; // total size: 672

class TimerModel : public RomModel<TimerSchema> {
public:
  TimerModel() : RomModel(CONTROL_TIMER_ROM_ADDR, "TimerModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
