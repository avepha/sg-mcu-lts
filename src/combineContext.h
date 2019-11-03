#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/nsensor/context.h"
#include "domain/gpio/context.h"

#include "domain/channel-control/criteria/context.h"
#include "domain/channel-control/timer/context.h"
#include "domain/channel-control/range/context.h"

#include "domain/continuous-control/continuous-criteria/context.h"

#include "domain/precondition/criteria/context.h"
#include "domain/precondition/timer/context.h"
#include "domain/precondition/range/context.h"

#include "domain/control/context.h"
#include "domain/channel/context.h"
#include "domain/continuous/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  RtcContext *rtc;
  SensorContext *sensor;
  NSensorContext *nsensors;
  GpioContext *gpio;

  ControlContext *control;
  ChannelContext *channel;
  ContinuousContext *continuous;

  CriteriaContext *criteria;
  TimerContext *timer;
  RangeContext *range;

  ContinuousCriteriaContext *continuousCriteria;

  PrecTimerContext *precTimer;
  PrecCriteriaContext *precCriteria;
  PrecRangeContext *precRange;
  CombineContext();
};

CombineContext::CombineContext() {
  rtc = new RtcContext;
  sensor = new SensorContext;
  nsensors = new NSensorContext;
  gpio = new GpioContext;

  precTimer = new PrecTimerContext;
  precCriteria = new PrecCriteriaContext;
  precRange = new PrecRangeContext;

  channel = new ChannelContext;
  criteria = new CriteriaContext;
  range = new RangeContext;
  timer = new TimerContext;

  control = new ControlContext;
  continuous = new ContinuousContext;
  continuousCriteria = new ContinuousCriteriaContext;

}

#endif //SG_MCU_COMBINECONTEXT_H
