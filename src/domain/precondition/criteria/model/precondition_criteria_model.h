#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_MODEL_H
#define SG_MCU_PRECONDITION_CRITERIA_MODEL_H

const int PRECONDITION_CRITERIA_ROM_ADDR = 900;

struct PreConditionCriteriaSchema {
  struct criteria {
    byte sensor = 0;
    float criteria = 0.0F;
    bool greater = false;
  } criterias[8];
}; // total size: 96

class PreConditionCriteriaModel : public RomModel<PreConditionCriteriaSchema> {
public:
  PreConditionCriteriaModel() : RomModel(PRECONDITION_CRITERIA_ROM_ADDR, "PreConditionCriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
