#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_MODEL_H
#define SG_MCU_PRECONDITION_TIMER_MODEL_H

#ifndef  HOUR_IN_SECOND
#define HOUR_IN_SECOND 3600
#endif

const int PRECONDITION_TIMER_ROM_ADDR = 300;

struct PreConditionTimerSchema {
  struct timer {
    int size = 2;
    unsigned long data[10][2] = {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}};
  } timers[8];
}; // total size: 656

class PreConditionTimerModel : public RomModel<PreConditionTimerSchema> {
public:
  PreConditionTimerModel() : RomModel(PRECONDITION_TIMER_ROM_ADDR, "PreconditionTimerModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
