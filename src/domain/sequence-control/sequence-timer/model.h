#include "domain/model.h"

#ifndef SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H
#define SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H

struct SequenceTimerSchema {
  struct Timer {
    uint8_t size = 2;
    struct TimePair {
      uint16_t start;
      uint16_t stop;
    } timePair[20] = {
        {.start = 6 * HOUR_IN_MINUTE, .stop = 12 * HOUR_IN_MINUTE},
        {.start = 13 * HOUR_IN_MINUTE, .stop = 18 * HOUR_IN_MINUTE}
    };
  } timer;
}; // total size: 128

class SequenceTimerModel : public RomModel<SequenceTimerSchema> {
public:
  SequenceTimerModel() : RomModel("SequenceTimerModel") {};
};

#endif //SG_MCU_CONTROL_SEQUENCE_TIMER_MODEL_H
