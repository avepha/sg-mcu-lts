#define TABLE_SIZE 2
#define SENSOR_ROM 0
#define PRECONDITION_TIMER_ROM 1

#ifndef SG_MCU_NVMEMORY_TABLE_H
#define SG_MCU_NVMEMORY_TABLE_H

#include "domain/sensor/model/sensor_model.h"
#include "domain/precondition/timer/model/precondition_timer_model.h"

class NvMemoryTable {
public:
  static NvMemoryTable& getInstance()
  {
    static NvMemoryTable instance;
    return instance;
  }

  uint16_t addressOf(int addr) {
    return table[addr];
  }

private:
  NvMemoryTable() {
    schemaSize[SENSOR_ROM] = sizeof(byte) + sizeof(int) + sizeof(SensorSchema);
    schemaSize[PRECONDITION_TIMER_ROM] = sizeof(byte) + sizeof(int) + sizeof(PreConditionTimerSchema);

    for(int i = 1 ; i < TABLE_SIZE; i++) {
      uint16_t prevSize = schemaSize[i - 1];
      uint16_t prevOffset = table[i - 1];

      table[i] = prevOffset + prevSize;
    }
  }

  uint16_t schemaSize[TABLE_SIZE] = {0};
  uint16_t table[TABLE_SIZE] = {0};
};

#endif //SG_MCU_NVMEMORY_TABLE_H
