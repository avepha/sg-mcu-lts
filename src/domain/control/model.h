#include "domain/model.h"
#include "./controlEnum.h"
#ifndef SG_MCU_CONTROL_MODEL_H
#define SG_MCU_CONTROL_MODEL_H

struct ControlSchema {
  CONTROL_ENUM type = CTRL_CHANNEL;
};

class ControlModel : public RomModel<ControlSchema> {
public:
  ControlModel() : RomModel("ControlModel") {};
};


#endif //SG_MCU_CONTROL_MODEL_H
