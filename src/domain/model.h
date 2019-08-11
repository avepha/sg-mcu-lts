//
// Created by Alfarie-MBP on 2019-08-11.
//
#include <EEPROM.h>

#ifndef SG_MCU_MODEL_H
#define SG_MCU_MODEL_H

#define EEPROM_MODEL_INITIALIZED 2

template<typename T>
class RomModel {
public:
  explicit RomModel(int addr) :
      INIT_ADDR(addr),
      writeOpTime(0) {
    T sch;
    schema = sch;
    WRITE_OP_ADDR = INIT_ADDR + sizeof(byte);
    SCHEMA_ADDR = WRITE_OP_ADDR + sizeof(int);

    byte initValue = 0;
    EEPROM.get(INIT_ADDR, initValue);

    if (initValue != EEPROM_MODEL_INITIALIZED) {
      EEPROM.put(WRITE_OP_ADDR, 1);
      EEPROM.put(INIT_ADDR, EEPROM_MODEL_INITIALIZED);
      EEPROM.put(SCHEMA_ADDR, schema);
      return;
    }

    EEPROM.get(SCHEMA_ADDR, schema);
  };

  void save(T sch) {
    int writeOps;
    EEPROM.get(WRITE_OP_ADDR, writeOps);
    EEPROM.put(WRITE_OP_ADDR, writeOps + 1);

    EEPROM.put(SCHEMA_ADDR, *sch);
  }

  T get() {
    T sch;
    EEPROM.put(SCHEMA_ADDR, sch);
    return sch;
  }

private:
  uint16_t INIT_ADDR, WRITE_OP_ADDR, SCHEMA_ADDR;
  int writeOpTime;
  T schema;
};


#endif //SG_MCU_MODEL_H
