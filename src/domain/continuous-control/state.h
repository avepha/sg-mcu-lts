#include <ArduinoJson.h>

#ifndef SG_MCU_ABSTRACT_CON_STATE_H
#define SG_MCU_ABSTRACT_CON_STATE_H

class State {
public:
  virtual JsonDocument report();
};

#endif //SG_MCU_ABSTRACT_CON_STATE_H
