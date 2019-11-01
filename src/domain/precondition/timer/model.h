#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_MODEL_H
#define SG_MCU_PRECONDITION_TIMER_MODEL_H

#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

struct PrecTimerSchema {
  struct timer {
    uint8_t size = 2;
    uint16_t data[10][2] = {{6 * HOUR_IN_MINUTE, 12 * HOUR_IN_MINUTE}, {13 * HOUR_IN_MINUTE, 18 * HOUR_IN_MINUTE}};
  } timers[8];
}; // total size: 300, 300-636

class PrecTimerModel : public RomModel<PrecTimerSchema> {
public:
  PrecTimerModel() : RomModel("PreconditionTimerModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
