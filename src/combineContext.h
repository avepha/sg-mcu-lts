//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "domain/rtc/context.h"

#ifndef SG_MCU_COMBINECONTEXT_H
#define SG_MCU_COMBINECONTEXT_H


class CombineContext {
public:
  RtcContext *rtcContext;
  CombineContext();
};

CombineContext::CombineContext() {
  rtcContext = new RtcContext;
}


#endif //SG_MCU_COMBINECONTEXT_H
