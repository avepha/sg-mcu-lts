#ifndef SG_MCU_BINARY_FRAME_H
#define SG_MCU_BINARY_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include <cstring>
#include <string>
#include <vector>

#include "binaryProtocol.h"

extern "C" {
#include "schema/phase1_info.pb.h"
}

#if __has_include(<FastCRC.h>)
#include <FastCRC.h>
#endif

struct BinaryEnvelope {
  uint8_t version;
  uint8_t messageType;
  uint8_t operationId;
  uint16_t requestId;
  uint8_t status;
  uint16_t payloadLength;
};

struct BinaryFrameDecodeResult {
  uint8_t status;
  BinaryEnvelope envelope;
  std::vector<uint8_t> payload;
};

static inline BinaryEnvelope binaryMakeEnvelope(
    uint8_t version,
    uint8_t messageType,
    uint8_t operationId,
    uint16_t requestId,
    uint8_t status,
    size_t payloadLength) {
  BinaryEnvelope envelope = {
      version,
      messageType,
      operationId,
      requestId,
      status,
      static_cast<uint16_t>(payloadLength),
  };
  return envelope;
}

static inline uint16_t binaryFrameReadLe16(const uint8_t *data) {
  return static_cast<uint16_t>(data[0]) |
         static_cast<uint16_t>(static_cast<uint16_t>(data[1]) << 8);
}

static inline void binaryFrameWriteLe16(uint16_t value, std::vector<uint8_t> *output) {
  output->push_back(static_cast<uint8_t>(value & 0xFF));
  output->push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

static inline uint16_t binaryFrameCrc16(const uint8_t *data, size_t length) {
#if __has_include(<FastCRC.h>)
  FastCRC16 crc16;
  return crc16.modbus(data, length);
#else
  return binaryProtocolCrc16(data, length);
#endif
}

static inline uint16_t binaryFrameCrc16(const std::vector<uint8_t> &data) {
  if (data.empty()) {
    return binaryFrameCrc16(nullptr, 0);
  }

  return binaryFrameCrc16(data.data(), data.size());
}

static inline void binaryFrameAppendVarint(uint32_t value, std::vector<uint8_t> *output) {
  while (value > 0x7F) {
    output->push_back(static_cast<uint8_t>((value & 0x7F) | 0x80));
    value >>= 7;
  }
  output->push_back(static_cast<uint8_t>(value & 0x7F));
}

static inline bool binaryFrameReadVarint(
    const std::vector<uint8_t> &input,
    size_t *offset,
    uint32_t *value) {
  uint32_t decoded = 0;
  uint8_t shift = 0;
  while (*offset < input.size() && shift < 35) {
    uint8_t current = input[*offset];
    (*offset)++;
    decoded |= static_cast<uint32_t>(current & 0x7F) << shift;
    if ((current & 0x80) == 0) {
      *value = decoded;
      return true;
    }
    shift = static_cast<uint8_t>(shift + 7);
  }
  return false;
}

static inline bool binaryFrameSkipField(
    const std::vector<uint8_t> &input,
    size_t *offset,
    uint8_t wireType) {
  if (wireType == 0) {
    uint32_t ignored = 0;
    return binaryFrameReadVarint(input, offset, &ignored);
  }

  if (wireType == 2) {
    uint32_t length = 0;
    if (!binaryFrameReadVarint(input, offset, &length)) {
      return false;
    }
    if (*offset + length > input.size()) {
      return false;
    }
    *offset += length;
    return true;
  }

  return false;
}

static inline std::vector<uint8_t> binaryEncodeInfoQueryRequestPayload(const Phase1InfoQueryRequest &request) {
  std::vector<uint8_t> payload;
  if (request.has_client_capabilities) {
    payload.push_back(0x08);
    binaryFrameAppendVarint(request.client_capabilities, &payload);
  }
  return payload;
}

static inline bool binaryDecodeInfoQueryRequestPayload(
    const std::vector<uint8_t> &payload,
    Phase1InfoQueryRequest *request) {
  if (request == nullptr) {
    return false;
  }

  *request = Phase1InfoQueryRequest_init_default;

  size_t offset = 0;
  while (offset < payload.size()) {
    uint32_t key = 0;
    if (!binaryFrameReadVarint(payload, &offset, &key)) {
      return false;
    }

    uint32_t fieldNumber = key >> 3;
    uint8_t wireType = static_cast<uint8_t>(key & 0x07);
    if (fieldNumber == 1 && wireType == 0) {
      uint32_t capabilities = 0;
      if (!binaryFrameReadVarint(payload, &offset, &capabilities)) {
        return false;
      }
      request->has_client_capabilities = true;
      request->client_capabilities = capabilities;
      continue;
    }

    if (!binaryFrameSkipField(payload, &offset, wireType)) {
      return false;
    }
  }

  return true;
}

static inline std::vector<uint8_t> binaryEncodeInfoQueryResponsePayload(const Phase1InfoQueryResponse &response) {
  std::vector<uint8_t> payload;

  size_t versionLength = strnlen(response.version, sizeof(response.version));
  if (versionLength > 0) {
    payload.push_back(0x0A);
    binaryFrameAppendVarint(versionLength, &payload);
    payload.insert(payload.end(), response.version, response.version + versionLength);
  }

  size_t modelLength = strnlen(response.model, sizeof(response.model));
  if (modelLength > 0) {
    payload.push_back(0x12);
    binaryFrameAppendVarint(modelLength, &payload);
    payload.insert(payload.end(), response.model, response.model + modelLength);
  }

  if (response.device_environment != Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_UNSPECIFIED) {
    payload.push_back(0x18);
    binaryFrameAppendVarint(response.device_environment, &payload);
  }

  return payload;
}

static inline bool binaryDecodeInfoQueryResponsePayload(
    const std::vector<uint8_t> &payload,
    Phase1InfoQueryResponse *response) {
  if (response == nullptr) {
    return false;
  }

  *response = Phase1InfoQueryResponse_init_default;
  size_t offset = 0;
  while (offset < payload.size()) {
    uint32_t key = 0;
    if (!binaryFrameReadVarint(payload, &offset, &key)) {
      return false;
    }

    uint32_t fieldNumber = key >> 3;
    uint8_t wireType = static_cast<uint8_t>(key & 0x07);
    if ((fieldNumber == 1 || fieldNumber == 2) && wireType == 2) {
      uint32_t length = 0;
      if (!binaryFrameReadVarint(payload, &offset, &length)) {
        return false;
      }
      if (*&offset + length > payload.size()) {
        return false;
      }

      char *target = fieldNumber == 1 ? response->version : response->model;
      size_t targetSize = fieldNumber == 1 ? sizeof(response->version) : sizeof(response->model);
      if (length >= targetSize) {
        return false;
      }
      memcpy(target, payload.data() + offset, length);
      target[length] = '\0';
      offset += length;
      continue;
    }

    if (fieldNumber == 3 && wireType == 0) {
      uint32_t environment = 0;
      if (!binaryFrameReadVarint(payload, &offset, &environment)) {
        return false;
      }
      response->device_environment = static_cast<Phase1Info_DeviceEnvironment>(environment);
      continue;
    }

    if (!binaryFrameSkipField(payload, &offset, wireType)) {
      return false;
    }
  }

  return true;
}

static inline std::vector<uint8_t> binaryFrameSerializeEnvelope(const BinaryEnvelope &envelope) {
  std::vector<uint8_t> body;
  body.reserve(SG_BINARY_ENVELOPE_SIZE + envelope.payloadLength);
  body.push_back(envelope.version);
  body.push_back(envelope.messageType);
  body.push_back(envelope.operationId);
  binaryFrameWriteLe16(envelope.requestId, &body);
  body.push_back(envelope.status);
  binaryFrameWriteLe16(envelope.payloadLength, &body);
  return body;
}

static inline uint8_t binaryFrameValidateEnvelope(const BinaryEnvelope &envelope) {
  if (envelope.version != SG_BINARY_PROTOCOL_VERSION) {
    return SG_BINARY_STATUS_UNSUPPORTED_VERSION;
  }
  if (envelope.operationId != SG_BINARY_OPERATION_INFO_QUERY) {
    return SG_BINARY_STATUS_UNKNOWN_OPERATION;
  }
  if (envelope.payloadLength > SG_BINARY_MAX_PAYLOAD_SIZE) {
    return SG_BINARY_STATUS_OVERSIZE;
  }
  return SG_BINARY_STATUS_OK;
}

static inline std::vector<uint8_t> binaryFrameEncode(
    const BinaryEnvelope &envelope,
    const std::vector<uint8_t> &payload) {
  if (payload.size() != envelope.payloadLength || payload.size() > SG_BINARY_MAX_PAYLOAD_SIZE) {
    return {};
  }

  std::vector<uint8_t> body = binaryFrameSerializeEnvelope(envelope);
  body.insert(body.end(), payload.begin(), payload.end());
  if (!binaryProtocolBodyLengthIsValid(body.size())) {
    return {};
  }

  std::vector<uint8_t> frame;
  frame.reserve(binaryProtocolFrameSize(body.size()));
  frame.push_back(SG_BINARY_SYNC_1);
  frame.push_back(SG_BINARY_SYNC_2);
  binaryFrameWriteLe16(static_cast<uint16_t>(body.size()), &frame);
  frame.insert(frame.end(), body.begin(), body.end());
  binaryFrameWriteLe16(binaryFrameCrc16(body), &frame);
  return frame;
}

static inline BinaryFrameDecodeResult binaryFrameDecode(const std::vector<uint8_t> &frame) {
  BinaryFrameDecodeResult result = {
      SG_BINARY_STATUS_BAD_FRAME,
      binaryMakeEnvelope(0, 0, 0, 0, SG_BINARY_STATUS_BAD_FRAME, 0),
      {},
  };

  if (frame.size() < SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE) {
    return result;
  }
  if (frame[0] != SG_BINARY_SYNC_1 || frame[1] != SG_BINARY_SYNC_2) {
    return result;
  }

  uint16_t bodyLength = binaryFrameReadLe16(frame.data() + SG_BINARY_FRAME_SYNC_SIZE);
  if (bodyLength > SG_BINARY_MAX_BODY_SIZE) {
    result.status = SG_BINARY_STATUS_OVERSIZE;
    return result;
  }
  if (bodyLength < SG_BINARY_ENVELOPE_SIZE) {
    return result;
  }

  size_t expectedFrameSize = binaryProtocolFrameSize(bodyLength);
  if (frame.size() < expectedFrameSize) {
    return result;
  }

  std::vector<uint8_t> body(
      frame.begin() + SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE,
      frame.begin() + SG_BINARY_FRAME_SYNC_SIZE + SG_BINARY_FRAME_LENGTH_SIZE + bodyLength);

  uint16_t receivedCrc = binaryFrameReadLe16(frame.data() + expectedFrameSize - SG_BINARY_FRAME_CRC_SIZE);
  if (binaryFrameCrc16(body) != receivedCrc) {
    result.status = SG_BINARY_STATUS_CRC_FAIL;
    return result;
  }

  BinaryEnvelope envelope = {
      body[SG_BINARY_OFFSET_VERSION],
      body[SG_BINARY_OFFSET_MESSAGE_TYPE],
      body[SG_BINARY_OFFSET_OPERATION_ID],
      binaryFrameReadLe16(body.data() + SG_BINARY_OFFSET_REQUEST_ID),
      body[SG_BINARY_OFFSET_STATUS],
      binaryFrameReadLe16(body.data() + SG_BINARY_OFFSET_PAYLOAD_LENGTH),
  };
  result.envelope = envelope;

  uint8_t envelopeStatus = binaryFrameValidateEnvelope(envelope);
  if (envelopeStatus != SG_BINARY_STATUS_OK) {
    result.status = envelopeStatus;
    return result;
  }

  size_t actualPayloadLength = body.size() - SG_BINARY_ENVELOPE_SIZE;
  if (actualPayloadLength != envelope.payloadLength) {
    return result;
  }

  result.payload.assign(body.begin() + SG_BINARY_ENVELOPE_SIZE, body.end());
  result.status = SG_BINARY_STATUS_OK;
  return result;
}

#endif // SG_MCU_BINARY_FRAME_H
