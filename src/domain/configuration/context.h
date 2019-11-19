#include "./model.h"
#include "util/debug.h"

#ifndef SG_MCU_CONFIGURATION_CONTEXT_H
#define SG_MCU_CONFIGURATION_CONTEXT_H

class ConfigurationContext {
public:
  ConfigurationModel *model;
  ConfigurationContext();
};

ConfigurationContext::ConfigurationContext() {
  model = new ConfigurationModel();
}

#endif //SG_MCU_CONTEXT_H
