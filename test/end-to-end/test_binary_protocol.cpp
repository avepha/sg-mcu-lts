#ifndef SG_MCU_TEST_BINARY_PROTOCOL_CPP
#define SG_MCU_TEST_BINARY_PROTOCOL_CPP

#include <unity.h>

#include <stdint.h>
#include <vector>

#include "protocol/binaryProtocol.h"

static std::vector<uint8_t> makeInfoQueryRequestBody() {
  std::vector<uint8_t> body(SG_BINARY_ENVELOPE_SIZE, 0);
  body[SG_BINARY_OFFSET_VERSION] = SG_BINARY_PROTOCOL_VERSION;
  body[SG_BINARY_OFFSET_MESSAGE_TYPE] = SG_BINARY_MESSAGE_TYPE_REQUEST;
  body[SG_BINARY_OFFSET_OPERATION_ID] = SG_BINARY_OPERATION_INFO_QUERY;
  body[SG_BINARY_OFFSET_REQUEST_ID] = 0x34;
  body[SG_BINARY_OFFSET_REQUEST_ID + 1] = 0x12;
  body[SG_BINARY_OFFSET_STATUS] = SG_BINARY_STATUS_OK;
  body[SG_BINARY_OFFSET_PAYLOAD_LENGTH] = 0x00;
  body[SG_BINARY_OFFSET_PAYLOAD_LENGTH + 1] = 0x00;
  return body;
}

void testBinaryProtocolFrameConstantsStayConsistent() {
  TEST_ASSERT_EQUAL_HEX8(0xA5, SG_BINARY_SYNC_1);
  TEST_ASSERT_EQUAL_HEX8(0x5A, SG_BINARY_SYNC_2);
  TEST_ASSERT_EQUAL_UINT8(1, SG_BINARY_PROTOCOL_VERSION);
  TEST_ASSERT_EQUAL_UINT8(8, SG_BINARY_ENVELOPE_SIZE);
  TEST_ASSERT_EQUAL_UINT32(
      SG_BINARY_MAX_BODY_SIZE + SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE + SG_BINARY_FRAME_CRC_SIZE,
      binaryProtocolFrameSize(SG_BINARY_MAX_BODY_SIZE));
}

void testBinaryProtocolRejectsCrcMismatch() {
  std::vector<uint8_t> body = makeInfoQueryRequestBody();
  uint16_t crc = binaryProtocolFrameCrc(body);
  TEST_ASSERT_TRUE(binaryProtocolVerifyFrame(body, crc));
  TEST_ASSERT_FALSE(binaryProtocolVerifyFrame(body, static_cast<uint16_t>(crc ^ 0x00FF)));
}

void testBinaryProtocolRejectsOversizeBodyBeforeDecode() {
  TEST_ASSERT_FALSE(binaryProtocolBodyLengthIsValid(SG_BINARY_MAX_BODY_SIZE + 1));
  TEST_ASSERT_TRUE(binaryProtocolBodyLengthIsValid(SG_BINARY_ENVELOPE_SIZE));
}

void testBinaryProtocolInfoFixturesFitDeclaredBounds() {
  TEST_ASSERT_LESS_OR_EQUAL_UINT32(SG_BINARY_MAX_PAYLOAD_SIZE, binaryProtocolInfoQueryRequestFixtureSize());
  TEST_ASSERT_LESS_OR_EQUAL_UINT32(SG_BINARY_MAX_PAYLOAD_SIZE, binaryProtocolInfoQueryResponseFixtureSize());
}

#endif
