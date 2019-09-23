#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/precondition/context.h"
#include "domain/nsensor/context.h"

#include "domain/control/criteria/context.h"
#include "domain/control/timer/context.h"

#include "domain/channel/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  RtcContext *rtcContext;
  SensorContext *sensorContext;
  PreConditionContext *preConditionContext;
  CriteriaContext *criteriaContext;
  TimerContext *timerContext;
  NSensorContext *nSensorContext;
  ChannelContext *channelContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
  sensorContext = new SensorContext;
  preConditionContext = new PreConditionContext;
  criteriaContext = new CriteriaContext;
  timerContext = new TimerContext;
  nSensorContext = new NSensorContext;
  channelContext = new ChannelContext;
};


#endif //SG_MCU_COMBINECONTEXT_H
