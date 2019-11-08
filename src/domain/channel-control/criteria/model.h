#include "domain/model.h"

#ifndef SG_MCU_CONTROL_CRITERIA_MODEL_H
#define SG_MCU_CONTROL_CRITERIA_MODEL_H

struct CriteriaSchema {
  struct Criteria {
    byte sensor = 0;
    float criteria = 0.0F;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t workingTimeInSecond = 10;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } criterias[8];
};

class CriteriaModel : public RomModel<CriteriaSchema> {
public:
  CriteriaModel() : RomModel("CriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
