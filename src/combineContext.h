#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/precondition/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H


class CombineContext {
public:
  RtcContext *rtcContext;
  SensorContext *sensorContext;
  PreConditionContext *preConditionContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
  sensorContext = new SensorContext;
  preConditionContext = new PreConditionContext;
};


#endif //SG_MCU_COMBINECONTEXT_H
