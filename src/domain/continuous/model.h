#include "domain/continuous-control/continuousControlTypeEnum.h"
#include "domain/precondition/precondition.h"
#include "domain/model.h"

#ifndef SG_MCU_CONTINUOUS_MODEL_H
#define SG_MCU_CONTINUOUS_MODEL_H

struct ContinuousSchema {
  struct Continuous {
    bool isActive = false;

    struct Control {
      CONTINUOUS_CONTROL_TYPE_ENUM type = CON_CTRL_NONE;
      uint8_t channelOrderAndTimingSize = 3;
      struct channelOrderAndTiming {
        uint8_t channel;
        uint32_t workingTimeInSec;
      } channelOrderAndTiming[8] = {
          {0, 5},
          {1, 5},
          {2, 5},
          {3, 5},
          {4, 5},
          {5, 5},
          {6, 5},
          {7, 5}
      };
    } control;

    struct Precondition {
      PRECONDITION_TYPE_ENUM type = PREC_NONE;
      uint8_t value = 0;
    } preconditions[3];
  } continuous;
};

class ContinuousModel : public RomModel<ContinuousSchema> {
public:
  ContinuousModel() : RomModel("ContinuousModel") {};
};


#endif //SG_MCU_CONTINUOUS_MODEL_H
