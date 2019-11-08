#include "domain/model.h"

#ifndef SG_MCU_CONTROL_CONTINUOUS_RANGE_MODEL_H
#define SG_MCU_CONTROL_CONTINUOUS_RANGE_MODEL_H

struct ContinuousRangeSchema {
  struct Range {
    byte sensor = 0;
    float upper = 0;
    float lower = 0;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } range;
}; // total size: 128

class ContinuousRangeModel : public RomModel<ContinuousRangeSchema> {
public:
  ContinuousRangeModel() : RomModel("ContinuousRangeModel") {};
};

#endif //SG_MCU_CONTROL_CONTINUOUS_RANGE_MODEL_H
