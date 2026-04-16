#ifdef ARDUINO

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

#endif

#ifndef ARDUINO

#include <unity.h>

#include "./end-to-end/test_binary_protocol.cpp"

int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(testBinaryProtocolFrameConstantsStayConsistent);
  RUN_TEST(testBinaryProtocolRejectsCrcMismatch);
  RUN_TEST(testBinaryProtocolRejectsOversizeBodyBeforeDecode);
  RUN_TEST(testBinaryProtocolInfoFixturesFitDeclaredBounds);

  return UNITY_END();
}

#endif
