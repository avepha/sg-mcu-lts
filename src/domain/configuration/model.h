#include "domain/model.h"
#include "logger/loggerUtil.h"

#ifndef SG_MCU_CONFIGURATION_MODEL_H
#define SG_MCU_CONFIGURATION_MODEL_H

struct ConfigurationSchema {
  LoggerLevelEnum logLevel = FATAL;
}; // size 288

class ConfigurationModel : public RomModel<ConfigurationSchema> {
public:
  ConfigurationModel() : RomModel("ConfigurationModel") {};
};

#endif //SG_MCU_CONFIGURATION_MODEL_H
