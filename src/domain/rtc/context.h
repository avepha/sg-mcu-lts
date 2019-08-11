//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "core/rtc_core.h"

#ifndef SG_MCU_RTC_CONTEXT_H
#define SG_MCU_RTC_CONTEXT_H


class RtcContext {
public:
  RtcCore *core;
  RtcContext();
};

RtcContext::RtcContext() {
  core = new RtcCore();
}


#endif //SG_MCU_CONTEXT_H
