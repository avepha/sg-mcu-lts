//
// Created by Alfarie-MBP on 2019-08-14.
//
#include "domain/model.h"

#ifndef SG_MCU_CRITERIA_MODEL_H
#define SG_MCU_CRITERIA_MODEL_H

const int CRITERIA_ROM_ADDR = 1000;

struct CriteriaSchema {
  struct criteria {
    byte sensor = 0;
    float criteria = 0.0F;
    bool greater = false;
  } criterias[8];
}; // total size: 96

class CriteriaModel : public RomModel<CriteriaSchema> {
public:
  CriteriaModel() : RomModel(CRITERIA_ROM_ADDR, "CriteriaModel") {};
};

#endif //SG_MCU_CRITERIA_MODEL_H
