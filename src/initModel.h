#include "combineContext.h"

#ifndef SG_MCU_LTS_INITMODEL_H
#define SG_MCU_LTS_INITMODEL_H

void initModel(CombineContext *context) {
  ConfigurationSchema configurationSchema = context->config->model->get();
  StationSchema stationSchema = context->station->model->get();
  if (configurationSchema.isPresetInit == 0) {
#ifdef SG_MODEL_GSOLUTION
    if (configurationSchema.preset != MODEL_GSOLUTION) {
      configurationSchema.preset = MODEL_GSOLUTION;
      stationSchema.solutionStation.size  = 0;
      stationSchema.gSensorStation.size = 0;
      stationSchema.gSolutionStation.size = 1;
      Serial.println("init model: GSOLUTION");
    }
#else  //SG_MODEL_GENERAL
    if (configurationSchema.preset != MODEL_GENERAL) {
      configurationSchema.preset = MODEL_GENERAL;

      stationSchema.gSolutionStation.size  = 1;
      stationSchema.gSensorStation.size = 1;
      stationSchema.gSolutionStation.size = 0;
      Serial.println("init model: GENERAL");
    }
#endif
    context->station->model->save(stationSchema);
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
