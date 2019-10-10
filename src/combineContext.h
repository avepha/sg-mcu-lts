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
  RtcContext *rtc;
  SensorContext *sensor;
  PrecCriteriaContext *precondition;
  CriteriaContext *criteria;
  TimerContext *timer;
  NSensorContext *nsensors;
  ChannelContext *channel;
  CombineContext();
};

CombineContext::CombineContext() {
  rtc = new RtcContext;
  sensor = new SensorContext;
  precondition = new PrecCriteriaContext;
  criteria = new CriteriaContext;
  timer = new TimerContext;
  nsensors = new NSensorContext;
  channel = new ChannelContext;
}

#endif //SG_MCU_COMBINECONTEXT_H
