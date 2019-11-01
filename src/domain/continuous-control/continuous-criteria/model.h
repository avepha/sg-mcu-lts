#include "domain/model.h"

#ifndef SG_MCU_CONTROL_CONTINUOUS_CRITERIA_MODEL_H
#define SG_MCU_CONTROL_CONTINUOUS_CRITERIA_MODEL_H

struct ContinuousCriteriaSchema {
  struct Criteria {
    byte sensor = 0;
    float criteria = 0.0F;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t workingTimeInSecond = 10;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } criteria;
}; // total size: 128

class ContinuousCriteriaModel : public RomModel<ContinuousCriteriaSchema> {
public:
  ContinuousCriteriaModel() : RomModel("ContinuousCriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
