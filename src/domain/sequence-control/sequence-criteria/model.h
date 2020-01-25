#include "domain/model.h"

#ifndef SG_MCU_CONTROL_SEQUENCE_CRITERIA_MODEL_H
#define SG_MCU_CONTROL_SEQUENCE_CRITERIA_MODEL_H

struct SequenceCriteriaSchema {
  struct Criteria {
    byte sensor = 1;
    float criteria = 0.0F;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } criteria;
}; // total size: 128

class SequenceCriteriaModel : public RomModel<SequenceCriteriaSchema> {
public:
  SequenceCriteriaModel() : RomModel("SequenceCriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
