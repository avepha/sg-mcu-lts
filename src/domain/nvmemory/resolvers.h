//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <RTClib.h>

#include "combineContext.h"

#include "../resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_MEMORY_RESOLVERS_H
#define SG_MCU_MEMORY_RESOLVERS_H

// @mutation: date_save
class mutation_clear_memory : public Resolvers {
public:
  explicit mutation_clear_memory(CombineContext *context) : Resolvers("clear_memory", context) {};

  String resolve(JsonObject json) override {
    for(int i = 0 ; i < EEPROM_SIZE; i++) {
      EEPROM.put(i, 255);
    }
    EEPROM.commit();

    JsonTopic response(json["topic"], json["method"], "SUCCESS");
    return response.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
