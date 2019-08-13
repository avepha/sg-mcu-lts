
//
// Created by Alfarie-MBP on 2019-08-11.
//
#include "domain/model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_MODEL_H
#define SG_MCU_PRECONDITION_TIMER_MODEL_H
#define HOUR_IN_SECOND 3600

const int PRECONDITION_TIMER_ROM_ADDR = 300;

struct PreConditionTimerSchema {
  int timer_size[8] = {2, 2, 2, 2, 2, 2, 2, 2}; // 16
  unsigned long timer[8][10][2] = {
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
      {{6 * HOUR_IN_SECOND, 12 * HOUR_IN_SECOND}, {13 * HOUR_IN_SECOND, 18 * HOUR_IN_SECOND}},
  }; // 640
}; // total size: 656

class PreConditionTimerModel : public RomModel<PreConditionTimerSchema> {
public:
  PreConditionTimerModel() : RomModel(PRECONDITION_TIMER_ROM_ADDR, "PreconditionTimerModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
