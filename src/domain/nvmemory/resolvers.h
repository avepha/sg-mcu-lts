#include <RTClib.h>

#include "combineContext.h"

#include "../resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_MEMORY_RESOLVERS_H
#define SG_MCU_MEMORY_RESOLVERS_H

// @mutation: date_save
class mutation_clear_nvmemory : public Resolvers {
public:
  explicit mutation_clear_nvmemory(CombineContext *context) : Resolvers("clear_nvmemory", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    for(int i = 0 ; i < EEPROM_SIZE; i++) {
      EEPROM.put(i, 255);
    }
    EEPROM.commit();

    DynamicJsonDocument data(64);
    data["status"] = "success";
    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
