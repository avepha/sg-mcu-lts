#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_MODEL_H
#define SG_MCU_PRECONDITION_CRITERIA_MODEL_H

struct PrecCriteriaSchema {
  struct Criteria {
    byte sensor = 1;
    float criteria = 0.0F;
    bool greater = false;
  } criterias[8];
}; // total size: 96

class PrecCriteriaModel : public RomModel<PrecCriteriaSchema> {
public:
  PrecCriteriaModel() : RomModel("PreConditionCriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
