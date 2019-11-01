#include "domain/model.h"

#ifndef SG_MCU_RTC_MODEL_H
#define SG_MCU_RTC_MODEL_H

struct RtcSchema {
  int8_t tzOffsetHour = 7; // +07 :00ICT
  int8_t tzOffsetMin = 0;
}; // total size: 1

class RtcModel : public RomModel<RtcSchema> {
public:
  RtcModel() : RomModel("RtcModel") {};
};

#endif //SG_MCU_RTC_MODEL_H
