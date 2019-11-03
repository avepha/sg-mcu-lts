#include <ArduinoJson.h>

#ifndef SG_MCU_ABSTRACT_CONTINUOUS_STATE_H
#define SG_MCU_ABSTRACT_CONTINUOUS_STATE_H

class ContinuousState {
public:
  virtual JsonDocument report();
};

#endif //SG_MCU_ABSTRCT_STATE_H
