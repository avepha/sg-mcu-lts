#include <ArduinoJson.h>

#ifndef SG_MCU_ABSTRACT_STATE_H
#define SG_MCU_ABSTRACT_STATE_H

class ControlState {
public:
  virtual JsonDocument report();
};

#endif //SG_MCU_ABSTRCT_STATE_H
