#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/nsensor/context.h"

#include "domain/control/criteria/context.h"
#include "domain/control/timer/context.h"

#include "domain/precondition/criteria/context.h"
#include "domain/precondition/timer/context.h"

#include "domain/channel/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  RtcContext *rtc;
  SensorContext *sensor;
  NSensorContext *nsensors;
  ChannelContext *channel;

  CriteriaContext *criteria;
  TimerContext *timer;

  PrecTimerContext *precTimer;
  PrecCriteriaContext *precCriteria;
  CombineContext();
};

CombineContext::CombineContext() {
  rtc = new RtcContext;
  sensor = new SensorContext;
  criteria = new CriteriaContext;
  timer = new TimerContext;
  nsensors = new NSensorContext;
  channel = new ChannelContext;
  precTimer = new PrecTimerContext;
  precCriteria = new PrecCriteriaContext;
}

#endif //SG_MCU_COMBINECONTEXT_H
