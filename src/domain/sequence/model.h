#include "domain/sequence-control/sequenceControlTypeEnum.h"
#include "domain/precondition/precondition.h"
#include "domain/model.h"

#ifndef SG_MCU_SEQUENCE_MODEL_H
#define SG_MCU_SEQUENCE_MODEL_H

struct SequenceSchema {
  struct Sequence {
    bool isActive = false;
    uint8_t channelOrderAndTimingSize = 3;
    struct channelOrderAndTiming {
      uint8_t channel;
      uint32_t workingTimeInSec;
    } channelOrderAndTiming[8] = {
        {0, 5},
        {1, 5},
        {2, 5},
        {3, 5},
        {4, 5},
        {5, 5},
        {6, 5},
        {7, 5}
    };

    struct Control {
      SEQUENCE_CONTROL_TYPE_ENUM type = SEQ_CTRL_CRITERIA;
    } control;

    struct Precondition {
      PRECONDITION_TYPE_ENUM type = PREC_NONE;
      uint8_t value = 0;
    } preconditions[3];
  } sequence;
};

class SequenceModel : public RomModel<SequenceSchema> {
public:
  SequenceModel() : RomModel("SequenceModel") {};
};


#endif //SG_MCU_SEQUENCE_MODEL_H
