#include "domain/configuration/context.h"
#include "domain/rtc/context.h"
#include "domain/sensor/context.h"
#include "domain/nsensor/context.h"
#include "domain/gpio/context.h"

#include "domain/channel-control/criteria/context.h"
#include "domain/channel-control/timer/context.h"
#include "domain/channel-control/range/context.h"
#include "domain/channel-control/par/context.h"

#include "domain/sequence-control/sequence-criteria/context.h"
#include "domain/sequence-control/sequence-timer/context.h"
#include "domain/sequence-control/sequence-range/context.h"

#include "domain/precondition/criteria/context.h"
#include "domain/precondition/timer/context.h"
#include "domain/precondition/range/context.h"

#include "domain/control/context.h"
#include "domain/channel/context.h"
#include "domain/sequence/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  ConfigurationContext *config;
  RtcContext *rtc;
  SensorContext *sensor;
  NSensorContext *nsensors;
  GpioContext *gpio;

  ControlContext *control;
  ChannelContext *channel;
  SequenceContext *sequence;

  CriteriaContext *criteria;
  TimerContext *timer;
  RangeContext *range;
  ParContext *par;

  SequenceCriteriaContext *sequenceCriteria;
  SequenceTimerContext *sequenceTimer;
  SequenceRangeContext *sequenceRange;

  PrecTimerContext *precTimer;
  PrecCriteriaContext *precCriteria;
  PrecRangeContext *precRange;
  CombineContext();
};

CombineContext::CombineContext() {
  config = new ConfigurationContext;
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
  par = new ParContext;

  control = new ControlContext;
  sequence = new SequenceContext;
  sequenceCriteria = new SequenceCriteriaContext;
  sequenceTimer = new SequenceTimerContext;
  sequenceRange = new SequenceRangeContext;
}

#endif //SG_MCU_COMBINECONTEXT_H
