#include "domain/model.h"

#ifndef SG_MCU_CONTROL_SEQUENCE_PAR_MODEL_H
#define SG_MCU_CONTROL_SEQUENCE_PAR_MODEL_H

struct SequenceParSchema {
  struct Par {
    float parSumInKJ = 1000;
    struct Timing {
      uint16_t workingTimeInSecond = 10;
    } timing;
  } par;
}; // total size: 128

class SequenceParModel : public RomModel<SequenceParSchema> {
public:
  SequenceParModel() : RomModel("SequenceParModel") {};
};

#endif //SG_MCU_CONTROL_SEQUENCE_PAR_MODEL_H
