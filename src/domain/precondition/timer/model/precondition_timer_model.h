#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_MODEL_H
#define SG_MCU_PRECONDITION_TIMER_MODEL_H
#define HOUR_IN_SECOND 3600

const int PRECONDITION_TIMER_ROM_ADDR = 300;

// TODO: revise this structure -> change from array to struct type
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
