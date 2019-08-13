#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/precondition/context.h"

#include "domain/control/criteria/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  RtcContext *rtcContext;
  SensorContext *sensorContext;
  PreConditionContext *preConditionContext;
  CriteriaContext *criteriaContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
  sensorContext = new SensorContext;
  preConditionContext = new PreConditionContext;
  criteriaContext = new CriteriaContext;
};


#endif //SG_MCU_COMBINECONTEXT_H
