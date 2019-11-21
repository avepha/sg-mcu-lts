#include "domain/model.h"

#ifndef SG_MCU_CONTROL_TIMER_MODEL_H
#define SG_MCU_CONTROL_TIMER_MODEL_H

struct TimerSchema {
  struct Timer {
    uint8_t size = 2;
    struct TimePair {
      uint16_t start;
      uint16_t stop;
    } timePair[20] = {
        {.start = 6 * HOUR_IN_MINUTE, .stop = 12 * HOUR_IN_MINUTE},
        {.start = 13 * HOUR_IN_MINUTE, .stop = 18 * HOUR_IN_MINUTE}
    };
  } timers[8];
};

class TimerModel : public RomModel<TimerSchema> {
public:
  TimerModel() : RomModel("TimerModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
