#ifndef SG_MCU_TEST_BINARY_COEXISTENCE_CPP
#define SG_MCU_TEST_BINARY_COEXISTENCE_CPP

#include <unity.h>

#include <stdint.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "protocol/binaryEndpoint.h"
#include "protocol/binaryFrame.h"
#include "protocol/binaryRouter.h"

static std::vector<uint8_t> parseHexBytes(const std::string &hex) {
  std::vector<uint8_t> bytes;
  std::stringstream stream(hex);
  std::string token;

  while (stream >> token) {
    if (token.size() != 2) {
      continue;
    }
    bytes.push_back(static_cast<uint8_t>(strtoul(token.c_str(), nullptr, 16)));
  }

  return bytes;
}

static std::vector<uint8_t> loadFixtureFrame(const char *filePath, const char *heading) {
  std::ifstream input(filePath);
  if (!input.is_open()) {
    return {};
  }

  std::string line;
  bool foundHeading = false;
  bool inCodeBlock = false;
  std::string hexLine;

  while (std::getline(input, line)) {
    if (!foundHeading) {
      if (line == heading) {
        foundHeading = true;
      }
      continue;
    }

    if (line == "```text") {
      inCodeBlock = true;
      continue;
    }

    if (inCodeBlock && line == "```") {
      break;
    }

    if (inCodeBlock && !line.empty()) {
      hexLine = line;
      break;
    }
  }

  return parseHexBytes(hexLine);
}

static std::vector<uint8_t> buildUnknownOperationRequest(uint16_t requestId, uint8_t operationId) {
  BinaryEnvelope envelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      operationId,
      requestId,
      SG_BINARY_STATUS_OK,
      0);
  return binaryFrameEncode(envelope, {});
}

void testBinaryCoexistenceClassifiesJsonTrafficWithoutBinaryConsumption() {
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_TRANSPORT_JSON, binarySelectTransport('{'));
}

void testBinaryCoexistenceClassifiesSyncPrefixedTrafficAsBinary() {
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_TRANSPORT_BINARY, binarySelectTransport(SG_BINARY_SYNC_1));
}

void testBinaryCoexistenceRejectsMalformedFramesCanonically() {
  BinaryInfoSnapshot snapshot = binaryMakeInfoSnapshot(
      "2.2.4",
      "v2",
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT);

  std::vector<uint8_t> crcFrame = loadFixtureFrame("docs/protocol/examples/phase1-info.md", "## Malformed CRC Example");
  BinaryRouterResult crcResult = binaryRouteFrame(crcFrame, snapshot);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_CRC_FAIL, crcResult.status);

  std::vector<uint8_t> oversizeFrame = {
      SG_BINARY_SYNC_1,
      SG_BINARY_SYNC_2,
      static_cast<uint8_t>((SG_BINARY_MAX_BODY_SIZE + 1) & 0xFF),
      static_cast<uint8_t>(((SG_BINARY_MAX_BODY_SIZE + 1) >> 8) & 0xFF),
  };
  BinaryRouterResult oversizeResult = binaryRouteFrame(oversizeFrame, snapshot);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_OVERSIZE, oversizeResult.status);

  std::vector<uint8_t> unknownFrame = buildUnknownOperationRequest(0x4321, 0xFE);
  BinaryRouterResult unknownResult = binaryRouteFrame(unknownFrame, snapshot);
  TEST_ASSERT_EQUAL_UINT8(SG_BINARY_STATUS_UNKNOWN_OPERATION, unknownResult.status);
}

void testBinaryCoexistenceDocumentationFixturesMatchProtocolHelpers() {
  std::vector<uint8_t> requestFixture = loadFixtureFrame("docs/protocol/examples/phase1-info.md", "## Request Example");
  BinaryEnvelope requestEnvelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_REQUEST,
      SG_BINARY_OPERATION_INFO_QUERY,
      0x1234,
      SG_BINARY_STATUS_OK,
      0);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(binaryFrameEncode(requestEnvelope, {}).data(), requestFixture.data(), requestFixture.size());

  BinaryInfoSnapshot snapshot = binaryMakeInfoSnapshot(
      "2.2.4",
      "v2",
      Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT);
  BinaryRouterResult successResult = binaryRouteFrame(requestFixture, snapshot);
  std::vector<uint8_t> successFixture = loadFixtureFrame("docs/protocol/examples/phase1-info.md", "## Success Response Example");
  TEST_ASSERT_EQUAL_UINT8_ARRAY(successResult.responseFrame.data(), successFixture.data(), successFixture.size());

  std::vector<uint8_t> failureFixture = loadFixtureFrame("docs/protocol/examples/phase1-info.md", "## Failure Response Example");
  std::vector<uint8_t> unknownFrame = buildUnknownOperationRequest(0x4321, 0xFE);
  BinaryRouterResult failureResult = binaryRouteFrame(unknownFrame, snapshot);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(failureResult.responseFrame.data(), failureFixture.data(), failureFixture.size());
}

#endif
