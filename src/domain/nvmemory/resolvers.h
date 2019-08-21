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

  String resolve(JsonObject reqJson) override {
    for(int i = 0 ; i < EEPROM_SIZE; i++) {
      EEPROM.put(i, 255);
    }
    EEPROM.commit();

    JsonRequest response(reqJson["topic"], reqJson["method"], "SUCCESS");
    return response.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
