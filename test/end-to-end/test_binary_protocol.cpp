#ifndef SG_MCU_TEST_BINARY_PROTOCOL_CPP
#define SG_MCU_TEST_BINARY_PROTOCOL_CPP

#include <unity.h>

#include <stdint.h>
#include <vector>

#include "protocol/binaryProtocol.h"

extern "C" {
#include "protocol/schema/phase1_info.pb.h"
}

#include "protocol/binaryFrame.h"
#include "protocol/binaryEndpoint.h"
#include "protocol/binaryRouter.h"

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

void testBinaryFrameRoundTripsInfoQueryRequest() {
  Phase1InfoQueryRequest request = Phase1InfoQueryRequest_init_default;
  request.has_client_capabilities = true;
  request.client_capabilities = 7;

  std::vector<uint8_t> payload = binaryEncodeInfoQueryRequestPayload(request);
  BinaryEnvelope envelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x1234,
      SG_BINARY_STATUS_OK,
      payload.size());

  std::vector<uint8_t> frame = binaryFrameEncode(envelope, payload);
  BinaryFrameDecodeResult decoded = binaryFrameDecode(frame);

  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_OK, decoded.status);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_PROTOCOL_VERSION, decoded.envelope.version);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_OPERATION_INFO_QUERY, decoded.envelope.operationId);
  TEST_ASSERT_EQUAL_UINT16(0x1234, decoded.envelope.requestId);
  TEST_ASSERT_EQUAL_UINT32(payload.size(), decoded.payload.size());

  Phase1InfoQueryRequest requestOut = Phase1InfoQueryRequest_init_default;
  TEST_ASSERT_TRUE(binaryDecodeInfoQueryRequestPayload(decoded.payload, &requestOut));
  TEST_ASSERT_TRUE(requestOut.has_client_capabilities);
  TEST_ASSERT_EQUAL_UINT32(7, requestOut.client_capabilities);
}

void testBinaryFrameRejectsCrcMismatchWithoutPayloadDecode() {
  std::vector<uint8_t> payload;
  BinaryEnvelope envelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x0001,
      SG_BINARY_STATUS_OK,
      payload.size());
  std::vector<uint8_t> frame = binaryFrameEncode(envelope, payload);

  frame[frame.size() - 1] ^= 0xFF;

  BinaryFrameDecodeResult decoded = binaryFrameDecode(frame);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_CRC_FAIL, decoded.status);
  TEST_ASSERT_EQUAL_UINT32(0, decoded.payload.size());
}

void testBinaryFrameRejectsOversizeDeclaredBodyLength() {
  std::vector<uint8_t> frame = {
      SG_BINARY_SYNC_1,
      SG_BINARY_SYNC_2,
      static_cast<uint8_t>((SG_BINARY_MAX_BODY_SIZE + 1) & 0xFF),
      static_cast<uint8_t>(((SG_BINARY_MAX_BODY_SIZE + 1) >> 8) & 0xFF),
  };

  BinaryFrameDecodeResult decoded = binaryFrameDecode(frame);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_OVERSIZE, decoded.status);
  TEST_ASSERT_EQUAL_UINT32(0, decoded.payload.size());
}

void testBinaryFrameMapsUnsupportedVersionAndUnknownOperation() {
  std::vector<uint8_t> payload;
  BinaryEnvelope badVersionEnvelope = binaryMakeEnvelope(
      2,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x0033,
      SG_BINARY_STATUS_OK,
      payload.size());
  BinaryFrameDecodeResult badVersion = binaryFrameDecode(binaryFrameEncode(badVersionEnvelope, payload));
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_UNSUPPORTED_VERSION, badVersion.status);

  BinaryEnvelope badOperationEnvelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      0xFE,
      0x0034,
      SG_BINARY_STATUS_OK,
      payload.size());
  BinaryFrameDecodeResult badOperation = binaryFrameDecode(binaryFrameEncode(badOperationEnvelope, payload));
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_UNKNOWN_OPERATION, badOperation.status);
}

void testBinaryEndpointKeepsJsonTrafficOnLegacyPath() {
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_TRANSPORT_JSON, binarySelectTransport('{'));
}

void testBinaryEndpointRoutesSyncBytesToBinaryResponse() {
  Phase1InfoQueryRequest request = Phase1InfoQueryRequest_init_default;
  std::vector<uint8_t> requestPayload = binaryEncodeInfoQueryRequestPayload(request);
  BinaryEnvelope envelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x2222,
      SG_BINARY_STATUS_OK,
      requestPayload.size());
  std::vector<uint8_t> requestFrame = binaryFrameEncode(envelope, requestPayload);

  BinaryInfoSnapshot snapshot = binaryMakeInfoSnapshot(
      "2.2.4",
      "v2",
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT);
  BinaryRouterResult result = binaryRouteFrame(requestFrame, snapshot);

  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_TRANSPORT_BINARY, binarySelectTransport(SG_BINARY_SYNC_1));
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_OK, result.status);
  TEST_ASSERT_TRUE(result.responseFrame.size() > 0);
}

void testBinaryRouterReturnsInfoPayloadFromAdapterSnapshot() {
  std::vector<uint8_t> requestPayload;
  BinaryEnvelope requestEnvelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x2223,
      SG_BINARY_STATUS_OK,
      requestPayload.size());
  std::vector<uint8_t> requestFrame = binaryFrameEncode(requestEnvelope, requestPayload);

  BinaryInfoSnapshot snapshot = binaryMakeInfoSnapshot(
      "2.2.4",
      "v2_lora",
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT);
  BinaryRouterResult result = binaryRouteFrame(requestFrame, snapshot);
  BinaryFrameDecodeResult response = binaryFrameDecode(result.responseFrame);

  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_OK, response.status);
  TEST_ASSERT_EQUAL_UINT16(0x2223, response.envelope.requestId);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_MESSAGE_TYPE_RESPONSE, response.envelope.messageType);

  Phase1InfoQueryResponse payload = Phase1InfoQueryResponse_init_default;
  TEST_ASSERT_TRUE(binaryDecodeInfoQueryResponsePayload(response.payload, &payload));
  TEST_ASSERT_EQUAL_STRING("2.2.4", payload.version);
  TEST_ASSERT_EQUAL_STRING("v2_lora", payload.model);
  TEST_ASSERT_EQUAL_UINT8(
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT,
      payload.device_environment);
}

void testBinaryEndpointRejectsMalformedTrafficWithoutTakingJsonPath() {
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_TRANSPORT_DISCARD, binarySelectTransport(0x00));

  BinaryInfoSnapshot snapshot = binaryMakeInfoSnapshot(
      "2.2.4",
      "v2",
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT);
  BinaryRouterResult result = binaryRouteFrame({SG_BINARY_SYNC_1, SG_BINARY_SYNC_2, 0xFF}, snapshot);

  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_BAD_FRAME, result.status);
  TEST_ASSERT_TRUE(result.responseFrame.size() > 0);
}

#endif
