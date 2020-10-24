#include <RTClib.h>

#include "combineContext.h"

#include "../resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_LOW_LEVEL_COMMAND_H
#define SG_MCU_LOW_LEVEL_COMMAND_H

// @mutation: date_save
class mutation_clear_nvmemory : public Mutation {
public:
  explicit mutation_clear_nvmemory() : Mutation("clear_nvmemory") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    for(int i = 0 ; i < EEPROM_SIZE; i++) {
      EEPROM.put(i, 255);
    }
    EEPROM.commit();

    ESP.restart();
    DynamicJsonDocument data(64);
    data["status"] = "success";
    return data;
  };
};

class mutation_restart : public Mutation {
public:
  explicit mutation_restart() : Mutation("restart") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    bootCount = 0;
    ESP.restart();
    DynamicJsonDocument data(64);
    data["status"] = "success";
    return data;
  };
};

#endif //SG_MCU_LOW_LEVEL_COMMAND_H
