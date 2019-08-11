//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "domain/rtc/context.h"
#include "domain/sensor/context.h"

#ifndef SG_MCU_COMBINE_CONTEXT_H
#define SG_MCU_COMBINE_CONTEXT_H


class CombineContext {
public:
  RtcContext *rtcContext;
  SensorContext *sensorContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
  sensorContext = new SensorContext;
};


#endif //SG_MCU_COMBINECONTEXT_H
