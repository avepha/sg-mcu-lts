#include "./core.h"
#include "./model.h"

#ifndef SG_MCU_RTC_CONTEXT_H
#define SG_MCU_RTC_CONTEXT_H

class RtcContext {
public:
  RtcCore *core;
  RtcModel *model;
  RtcContext();
};

RtcContext::RtcContext() {
  core = RtcCore::instance();
  model = new RtcModel;
}

#endif //SG_MCU_CONTEXT_H
