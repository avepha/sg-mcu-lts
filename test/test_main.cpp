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
#include "./end-to-end/test_binary_coexistence.cpp"

int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(testBinaryProtocolFrameConstantsStayConsistent);
  RUN_TEST(testBinaryProtocolRejectsCrcMismatch);
  RUN_TEST(testBinaryProtocolRejectsOversizeBodyBeforeDecode);
  RUN_TEST(testBinaryProtocolInfoFixturesFitDeclaredBounds);
  RUN_TEST(testBinaryFrameRoundTripsInfoQueryRequest);
  RUN_TEST(testBinaryFrameRejectsCrcMismatchWithoutPayloadDecode);
  RUN_TEST(testBinaryFrameRejectsOversizeDeclaredBodyLength);
  RUN_TEST(testBinaryFrameMapsUnsupportedVersionAndUnknownOperation);
  RUN_TEST(testBinaryEndpointKeepsJsonTrafficOnLegacyPath);
  RUN_TEST(testBinaryEndpointRoutesSyncBytesToBinaryResponse);
  RUN_TEST(testBinaryRouterReturnsInfoPayloadFromAdapterSnapshot);
  RUN_TEST(testBinaryEndpointRejectsMalformedTrafficWithoutTakingJsonPath);
  RUN_TEST(testBinaryCoexistenceClassifiesJsonTrafficWithoutBinaryConsumption);
  RUN_TEST(testBinaryCoexistenceClassifiesSyncPrefixedTrafficAsBinary);
  RUN_TEST(testBinaryCoexistenceRejectsMalformedFramesCanonically);
  RUN_TEST(testBinaryCoexistenceDocumentationFixturesMatchProtocolHelpers);

  return UNITY_END();
}

#endif
