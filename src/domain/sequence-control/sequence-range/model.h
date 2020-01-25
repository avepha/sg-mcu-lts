#include "domain/model.h"

#ifndef SG_MCU_CONTROL_SEQUENCE_RANGE_MODEL_H
#define SG_MCU_CONTROL_SEQUENCE_RANGE_MODEL_H

struct SequenceRangeSchema {
  struct Range {
    byte sensor = 1;
    float upper = 0;
    float lower = 0;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } range;
}; // total size: 128

class SequenceRangeModel : public RomModel<SequenceRangeSchema> {
public:
  SequenceRangeModel() : RomModel("SequenceRangeModel") {};
};

#endif //SG_MCU_CONTROL_SEQUENCE_RANGE_MODEL_H
