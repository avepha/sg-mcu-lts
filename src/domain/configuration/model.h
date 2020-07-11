#include "domain/model.h"
#include "logger/loggerUtil.h"
#include "util/presetEnum.h"

#ifndef SG_MCU_CONFIGURATION_MODEL_H
#define SG_MCU_CONFIGURATION_MODEL_H

struct ConfigurationSchema {
  LoggerLevelEnum logLevel = INFO;
  boolean isPresetInit = false;
  SgModelEnum preset = MODEL_NONE;
}; // size 288

class ConfigurationModel : public RomModel<ConfigurationSchema> {
public:
  ConfigurationModel() : RomModel("ConfigurationModel") {};
};

#endif //SG_MCU_CONFIGURATION_MODEL_H
