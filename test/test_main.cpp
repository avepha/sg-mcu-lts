#include "bootstrap.h"
#include "./system/rs485.h"

void setup() {
  delay(500);

  bootstrap();

  UNITY_BEGIN();

  RUN_TEST(rs485BasicCommand);

  UNITY_END();
}

void loop() {

}