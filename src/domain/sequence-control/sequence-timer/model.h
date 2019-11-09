#include "domain/model.h"

#ifndef SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H
#define SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H

struct SequenceTimerSchema {
  struct Timer {
    uint8_t size = 2;
    uint16_t data[10][2] = {{6 * HOUR_IN_MINUTE, 12 * HOUR_IN_MINUTE}, {13 * HOUR_IN_MINUTE, 18 * HOUR_IN_MINUTE}};
  } timer;
}; // total size: 128

class SequenceTimerModel : public RomModel<SequenceTimerSchema> {
public:
  SequenceTimerModel() : RomModel("SequenceTimerModel") {};
};

#endif //SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H
