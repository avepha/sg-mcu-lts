#include <map>
#include <EEPROM.h>

#ifndef SG_MCU_MODEL_H
#define SG_MCU_MODEL_H

#define EEPROM_MODEL_INITIALIZED 2

class RomOrganizer {
public:
  static RomOrganizer *instance() {
    if (!s_instance)
      s_instance = new RomOrganizer;
    return s_instance;
  }

  uint16_t addModelAndGetMemoryAddress(const std::string& name, uint16_t size) {
    if (addrMap.find(name) != addrMap.end()) {
      return addrMap[name];
    }

    uint16_t startAddr = 200;
    for (std::pair<std::string, uint16_t> size : sizeMap) {
      startAddr += size.second;
    }

    addrMap[name] = startAddr;
    sizeMap[name] = size;

    return addrMap[name];
  }

private:
  static RomOrganizer *s_instance;
  std::map<std::string, uint16_t> sizeMap;
  std::map<std::string, uint16_t> addrMap;
};

RomOrganizer *RomOrganizer::s_instance = nullptr;

template<typename T>
class RomModel {
public:
  RomModel(int addr, String name) :
      writeOpTime(0),
      modelName(name) {
    T sch;
    INIT_ADDR = RomOrganizer::instance()->addModelAndGetMemoryAddress(name.c_str(), sizeof(sch) + sizeof(byte) + sizeof(int) + 2);
    WRITE_OP_ADDR = INIT_ADDR + sizeof(byte);
    SCHEMA_ADDR = WRITE_OP_ADDR + sizeof(int);
    byte initValue = 0;
    EEPROM.get(INIT_ADDR, initValue);

    if (initValue != EEPROM_MODEL_INITIALIZED) {
      Serial.println("Create model " + String(modelName) + " size: " + String(sizeof(sch)) + " addr:" + String(INIT_ADDR));
      EEPROM.put(WRITE_OP_ADDR, 1);
      EEPROM.put(INIT_ADDR, EEPROM_MODEL_INITIALIZED);
      EEPROM.put(SCHEMA_ADDR, sch);
      EEPROM.commit();
      return;
    }

    Serial.println("Get model " + String(modelName) + " size: " + String(sizeof(sch)) + " addr:" + String(INIT_ADDR));
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

protected:
  uint16_t INIT_ADDR, WRITE_OP_ADDR, SCHEMA_ADDR;
  int writeOpTime;
  String modelName = "RomModel";
};

#endif //SG_MCU_MODEL_H
