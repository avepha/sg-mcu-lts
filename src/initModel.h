#include "combineContext.h"

#ifndef SG_MCU_LTS_INITMODEL_H
#define SG_MCU_LTS_INITMODEL_H

void initModel(CombineContext *context) {
  ConfigurationSchema configurationSchema = context->config->model->get();
  if (configurationSchema.isPresetInit == 0) {
#ifdef SG_MODEL_GSOLUTION
    if (configurationSchema.preset != MODEL_GSOLUTION) {
      StationSchema stationSchema = context->station->model->get();
      configurationSchema.preset = MODEL_GSOLUTION;
      stationSchema.solutionStation.size  = 0;
      stationSchema.gSensorStation.size = 0;
      stationSchema.gSolutionStation.size = 1;
      Serial.println("init model: GSOLUTION");
      context->station->model->save(stationSchema);

      ChannelSchema channelSchema = context->channel->model->get();
      CriteriaSchema criteriaSchema = context->criteria->model->get();
      channelSchema.channels[0].isActive = false;
      channelSchema.channels[0].control.type = CH_CTRL_CRITERIA;
      criteriaSchema.criterias[0].criteria = 1.0;
      criteriaSchema.criterias[0].greater = false;
      criteriaSchema.criterias[0].sensor = Sensor::SENSORMAP["gsol_ec"];
      criteriaSchema.criterias[0].timing.enable = true;
      criteriaSchema.criterias[0].timing.workingTimeInSecond = 30;
      criteriaSchema.criterias[0].timing.waitingTimeInSecond = 30;

      channelSchema.channels[1].isActive = false;
      channelSchema.channels[1].control.type = CH_CTRL_CRITERIA;
      criteriaSchema.criterias[1].criteria = 6.5;
      criteriaSchema.criterias[1].greater = true;
      criteriaSchema.criterias[1].sensor = Sensor::SENSORMAP["gsol_ph"];
      criteriaSchema.criterias[1].timing.enable = true;
      criteriaSchema.criterias[1].timing.workingTimeInSecond = 30;
      criteriaSchema.criterias[1].timing.waitingTimeInSecond = 30;
      context->criteria->model->save(criteriaSchema);
      context->channel->model->save(channelSchema);
    }
#else  //SG_MODEL_GENERAL
    if (configurationSchema.preset != MODEL_GENERAL) {
      StationSchema stationSchema = context->station->model->get();
      configurationSchema.preset = MODEL_GENERAL;
      stationSchema.gSolutionStation.size  = 1;
      stationSchema.gSensorStation.size = 1;
      stationSchema.gSolutionStation.size = 0;
      Serial.println("init model: GENERAL");
      context->station->model->save(stationSchema);
    }
#endif

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
