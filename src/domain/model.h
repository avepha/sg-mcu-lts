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
  RomModel(int addr, String name) :
      INIT_ADDR(addr),
      writeOpTime(0),
      modelName(name) {
    T sch;
    WRITE_OP_ADDR = INIT_ADDR + sizeof(byte);
    SCHEMA_ADDR = WRITE_OP_ADDR + sizeof(int);

    byte initValue = 0;
    EEPROM.get(INIT_ADDR, initValue);

    if (initValue != EEPROM_MODEL_INITIALIZED) {
      Serial.println("Create model " + String(modelName));
      EEPROM.put(WRITE_OP_ADDR, 1);
      EEPROM.put(INIT_ADDR, EEPROM_MODEL_INITIALIZED);
      EEPROM.put(SCHEMA_ADDR, sch);
      EEPROM.commit();
      return;
    }

    Serial.println("Get model " + String(modelName));
    EEPROM.get(SCHEMA_ADDR, sch);
  };

  int save(T sch) {
    int writeOps;
    EEPROM.put(SCHEMA_ADDR, sch);
    EEPROM.get(WRITE_OP_ADDR, writeOps);
    EEPROM.put(WRITE_OP_ADDR, writeOps + 1);
    EEPROM.commit();

    return writeOps + 1;
  }

  T get() {
    T sch;
    EEPROM.get(SCHEMA_ADDR, sch);
    return sch;
  }

private:
  uint16_t INIT_ADDR, WRITE_OP_ADDR, SCHEMA_ADDR;
  int writeOpTime;
  String modelName = "RomModel";
};


#endif //SG_MCU_MODEL_H
