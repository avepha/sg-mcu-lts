#ifndef SG_MCU_BINARY_PROTOCOL_H
#define SG_MCU_BINARY_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>
#include <vector>

static constexpr uint8_t SG_BINARY_SYNC_1 = 0xA5;
static constexpr uint8_t SG_BINARY_SYNC_2 = 0x5A;
static constexpr uint8_t SG_BINARY_PROTOCOL_VERSION = 1;

static constexpr size_t SG_BINARY_FRAME_SYNC_SIZE = 2;
static constexpr size_t SG_BINARY_FRAME_LENGTH_SIZE = 2;
static constexpr size_t SG_BINARY_FRAME_CRC_SIZE = 2;
static constexpr size_t SG_BINARY_ENVELOPE_SIZE = 8;
static constexpr size_t SG_BINARY_MAX_PAYLOAD_SIZE = 120;
static constexpr size_t SG_BINARY_MAX_BODY_SIZE = SG_BINARY_ENVELOPE_SIZE + SG_BINARY_MAX_PAYLOAD_SIZE;
static constexpr size_t SG_BINARY_MAX_FRAME_SIZE = SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE +
                                                   SG_BINARY_MAX_BODY_SIZE + SG_BINARY_FRAME_CRC_SIZE;
static constexpr size_t SG_BINARY_INFO_QUERY_REQUEST_FIXTURE_SIZE = 0;
static constexpr size_t SG_BINARY_INFO_QUERY_RESPONSE_VERSION_MAX_LENGTH = 32;
static constexpr size_t SG_BINARY_INFO_QUERY_RESPONSE_MODEL_MAX_LENGTH = 32;
static constexpr size_t SG_BINARY_INFO_QUERY_RESPONSE_FIXTURE_SIZE = 68;

enum BinaryMessageType : uint8_t {
  SG_BINARY_MESSAGE_TYPE_UNSPECIFIED = 0x00,
  SG_BINARY_MESSAGE_TYPE_REQUEST = 0x01,
  SG_BINARY_MESSAGE_TYPE_RESPONSE = 0x02,
};

enum BinaryOperationId : uint8_t {
  SG_BINARY_OPERATION_INFO_QUERY = 0x01,
};

enum BinaryStatusCode : uint8_t {
  SG_BINARY_STATUS_OK = 0x00,
  SG_BINARY_STATUS_BAD_FRAME = 0x01,
  SG_BINARY_STATUS_CRC_FAIL = 0x02,
  SG_BINARY_STATUS_OVERSIZE = 0x03,
  SG_BINARY_STATUS_UNSUPPORTED_VERSION = 0x04,
  SG_BINARY_STATUS_UNKNOWN_OPERATION = 0x05,
  SG_BINARY_STATUS_DECODE_FAIL = 0x06,
  SG_BINARY_STATUS_INTERNAL_ERROR = 0x07,
};

enum BinaryEnvelopeOffset : size_t {
  SG_BINARY_OFFSET_VERSION = 0,
  SG_BINARY_OFFSET_MESSAGE_TYPE = 1,
  SG_BINARY_OFFSET_OPERATION_ID = 2,
  SG_BINARY_OFFSET_REQUEST_ID = 3,
  SG_BINARY_OFFSET_STATUS = 5,
  SG_BINARY_OFFSET_PAYLOAD_LENGTH = 6,
};

static inline uint16_t binaryProtocolCrc16(const uint8_t *data, size_t length) {
  uint16_t crc = 0xFFFF;

  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      if (crc & 0x0001) {
        crc = static_cast<uint16_t>((crc >> 1) ^ 0xA001);
      } else {
        crc = static_cast<uint16_t>(crc >> 1);
      }
    }
  }

  return crc;
}

static inline size_t binaryProtocolFrameSize(size_t bodyLength) {
  return SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE + bodyLength + SG_BINARY_FRAME_CRC_SIZE;
}

static inline bool binaryProtocolBodyLengthIsValid(size_t bodyLength) {
  return bodyLength >= SG_BINARY_ENVELOPE_SIZE && bodyLength <= SG_BINARY_MAX_BODY_SIZE;
}

static inline uint16_t binaryProtocolFrameCrc(const std::vector<uint8_t> &body) {
  if (body.empty()) {
    return binaryProtocolCrc16(nullptr, 0);
  }

  return binaryProtocolCrc16(body.data(), body.size());
}

static inline bool binaryProtocolVerifyFrame(const std::vector<uint8_t> &body, uint16_t frameCrc) {
  if (!binaryProtocolBodyLengthIsValid(body.size())) {
    return false;
  }

  return binaryProtocolFrameCrc(body) == frameCrc;
}

static inline size_t binaryProtocolInfoQueryRequestFixtureSize() {
  return SG_BINARY_INFO_QUERY_REQUEST_FIXTURE_SIZE;
}

static inline size_t binaryProtocolInfoQueryResponseFixtureSize() {
  return SG_BINARY_INFO_QUERY_RESPONSE_FIXTURE_SIZE;
}

#endif // SG_MCU_BINARY_PROTOCOL_H
