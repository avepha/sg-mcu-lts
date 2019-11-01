#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_RANGE_MODEL_H
#define SG_MCU_PRECONDITION_RANGE_MODEL_H

struct PrecRangeSchema {
  struct Range {
    byte sensor = 0;
    float upper = 0.0F;
    float lower = 0.0F;
  } ranges[8];
}; // total size: 128

class PrecRangeModel : public RomModel<PrecRangeSchema> {
public:
  PrecRangeModel() : RomModel("PreConditionRangeModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
