#include "domain/model.h"

#ifndef SG_MCU_RTC_MODEL_H
#define SG_MCU_RTC_MODEL_H

const int RTC_ROM_ADDR = 0;

struct RtcSchema {
  int8_t timezone = 7; // +07 ICT
}; // total size: 1

class RtcModel : public RomModel<RtcSchema> {
public:
  RtcModel() : RomModel(RTC_ROM_ADDR, "RtcModel") {};
};

#endif //SG_MCU_RTC_MODEL_H
