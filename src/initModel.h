#include "combineContext.h"

#ifndef SG_MCU_LTS_INITMODEL_H
#define SG_MCU_LTS_INITMODEL_H

void initModel(CombineContext *context) {

  ConfigurationSchema configurationSchema = context->config->model->get();
  if (configurationSchema.isPresetInit == 0) {
    context->station->model->initModel();
    configurationSchema.isPresetInit = true;
    context->config->model->save(configurationSchema);
    delay(100);

    Log::info("model", "system restart.");
    ESP.restart();
  }
  else {
    Log::info("model", "Current preset: " + String(ModelEnumToString(configurationSchema.preset)));
  }
}

#endif //SG_MCU_LTS_INITMODEL_H
