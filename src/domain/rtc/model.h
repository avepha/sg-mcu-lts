#include "domain/model.h"

#ifndef SG_MCU_RTC_MODEL_H
#define SG_MCU_RTC_MODEL_H

const int RTC_ROM_ADDR = 0;

struct RtcSchema {
  int8_t tzOffsetHour = 7; // +07 :00ICT
  int8_t tzOffsetMin = 0;
}; // total size: 1

class RtcModel : public RomModel<RtcSchema> {
public:
  RtcModel() : RomModel(RTC_ROM_ADDR, "RtcModel") {};
};

#endif //SG_MCU_RTC_MODEL_H
