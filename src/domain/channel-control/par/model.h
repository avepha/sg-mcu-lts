#include "domain/model.h"

#ifndef SG_MCU_CONTROL_PAR_MODEL_H
#define SG_MCU_CONTROL_PAR_MODEL_H

struct ParSchema {
  byte parSensorIndex = 5;
  struct Par {
    float parSumInKJ = 1000;
    struct Timing {
      uint16_t workingTimeInSecond = 10;
    } timing;
  } pars[8];
};

class ParModel : public RomModel<ParSchema> {
public:
  ParModel() : RomModel("ParModel") {};
};

#endif //SG_MCU_PAR_MODEL_H
