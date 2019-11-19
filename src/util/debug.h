#include "domain/configuration/model.h"

#ifndef SG_MCU_DEBUG_H
#define SG_MCU_DEBUG_H

class Debug {
public:
  static Debug* instance()
  {
    if (!s_instance)
      s_instance = new Debug;
    return s_instance;
  }

  static void update() {
    ConfigurationModel model;
    ConfigurationSchema schema = model.get();
    debug = schema.debug;
  }

  static void Print(const String& msg) {
    if (debug) {
      Serial.println("D: " + msg);
    }
  }

  static bool isDebuggingMode() {
    return debug;
  }

private:
  Debug() {
    update();
  };
  static bool debug;
  static Debug *s_instance;
};

Debug *Debug::s_instance = nullptr;
bool Debug::debug = false;

#endif //SG_MCU_DEBUG_H
