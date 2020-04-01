#include <ArduinoJson.h>

#ifndef SG_MCU_ABSTRACT_SEQUENCE_STATE_H
#define SG_MCU_ABSTRACT_SEQUENCE_STATE_H

class SequenceState {
public:
  virtual DynamicJsonDocument report();
};

#endif //SG_MCU_ABSTRCT_STATE_H
