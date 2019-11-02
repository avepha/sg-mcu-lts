#include "domain/continuous-control/continuousControl.h"
#include "domain/precondition/precondition.h"
#include "domain/model.h"
#include "./core.h"

#ifndef SG_MCU_CONTINUOUS_MODEL_H
#define SG_MCU_CONTINUOUS_MODEL_H

struct ContinuousSchema {
  struct Continuous {
    bool isActive = false;

    struct Control {
      CONTINUOUS_CONTROL_TYPE_ENUM type = CON_CTRL_NONE;
      uint8_t channelOrders[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    } control;

    struct Precondition {
      PRECONDITION_TYPE_ENUM type = PREC_NONE;
      uint8_t value = 0;
    } preconditions[3];
  } continuous;
};

class ContinuousModel : public RomModel<ContinuousSchema> {
public:
  ContinuousModel() : RomModel("ContinuousModel") {};
};


#endif //SG_MCU_CONTINUOUS_MODEL_H
