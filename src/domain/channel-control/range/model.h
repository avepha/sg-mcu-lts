#include "domain/model.h"

#ifndef SG_MCU_CONTROL_RANGE_MODEL_H
#define SG_MCU_CONTROL_RANGE_MODEL_H

struct RangeSchema {
  struct Range {
    byte sensor = 1;
    float upper = 0;
    float lower = 0;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t workingTimeInSecond = 10;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } ranges[8];
}; // total size: 160

class RangeModel : public RomModel<RangeSchema> {
public:
  RangeModel() : RomModel("RangeModel") {};
};

#endif //SG_MCU_RANGE_MODEL_H
