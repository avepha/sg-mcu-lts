#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_MODEL_H
#define SG_MCU_PRECONDITION_TIMER_MODEL_H

#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

struct PrecTimerSchema {
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
}; // total size: 300, 300-636

class PrecTimerModel : public RomModel<PrecTimerSchema> {
public:
  PrecTimerModel() : RomModel("PreconditionTimerModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
