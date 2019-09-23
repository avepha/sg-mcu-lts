#include "../init.h"
#ifndef SG_MCU_TEST_CLEAR_EEPROM_H
#define SG_MCU_TEST_CLEAR_EEPROM_H

void clearRom() {
  JsonRequest requestTopic("clear_nvmemory", "mutation");
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["data"]["status"] == "success");
}

void TClearRom() {
  RUN_TEST(clearRom);
}

#endif //SG_MCU_CLEAR_EEPROM_H
