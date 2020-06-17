#include "domain/configuration/context.h"
#include "domain/rtc/context.h"
#include "domain/gpio/context.h"

#include "domain/channel-control/criteria/context.h"
#include "domain/channel-control/timer/context.h"
#include "domain/channel-control/range/context.h"
#include "domain/channel-control/par/context.h"

#include "domain/sequence-control/sequence-criteria/context.h"
#include "domain/sequence-control/sequence-timer/context.h"
#include "domain/sequence-control/sequence-range/context.h"
#include "domain/sequence-control/sequence-par/context.h"

#include "domain/precondition/criteria/context.h"
#include "domain/precondition/timer/context.h"
#include "domain/precondition/range/context.h"

#include "domain/control/context.h"
#include "domain/channel/context.h"
#include "domain/sequence/context.h"

#include "domain/sensor/context.h"
#include "domain/station/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H

class CombineContext {
public:
  ConfigurationContext *config;
  RtcContext *rtc;
  NotificationContext *gpio;

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
  SequenceParContext *sequencePar;

  PrecTimerContext *precTimer;
  PrecCriteriaContext *precCriteria;
  PrecRangeContext *precRange;

  StationContext *station;
  SensorContext *sensor;

  CombineContext();
};

CombineContext::CombineContext() {
  config = new ConfigurationContext;
  rtc = new RtcContext;
  gpio = new NotificationContext;

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
  sequencePar = new SequenceParContext;

  station = new StationContext;
  sensor = new SensorContext;
}

#endif //SG_MCU_COMBINECONTEXT_H
