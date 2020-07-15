#include "domain/model.h"
#include "./utils/channelTimerEnum.h"

#ifndef SG_MCU_CONTROL_TIMER_MODEL_H
#define SG_MCU_CONTROL_TIMER_MODEL_H

struct TimerSchema {
  struct Timer {
    ChannelTimerModeEnum mode = CH_TIMER_INTERVAL;
    uint8_t size = 2;
    struct TimePair {
      uint16_t start;
      uint16_t stop;
      uint16_t workingInSecond;
    } timePair[20] = {
        {.start = 6 * HOUR_IN_MINUTE, .stop = 12 * HOUR_IN_MINUTE, .workingInSecond = 30},
        {.start = 13 * HOUR_IN_MINUTE, .stop = 18 * HOUR_IN_MINUTE, .workingInSecond = 30}
    };
  } timers[8];
};

class TimerModel : public RomModel<TimerSchema> {
public:
  TimerModel() : RomModel("TimerModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
