#ifndef SG_MCU_BINARY_ENDPOINT_H
#define SG_MCU_BINARY_ENDPOINT_H

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "binaryProtocol.h"

enum BinaryTransportRoute : uint8_t {
  SG_BINARY_TRANSPORT_NONE = 0,
  SG_BINARY_TRANSPORT_JSON = 1,
  SG_BINARY_TRANSPORT_BINARY = 2,
  SG_BINARY_TRANSPORT_DISCARD = 3,
};

static inline uint8_t binarySelectTransport(int firstByte) {
  if (firstByte < 0) {
    return SG_BINARY_TRANSPORT_NONE;
  }
  if (firstByte == '{') {
    return SG_BINARY_TRANSPORT_JSON;
  }
  if (firstByte == SG_BINARY_SYNC_1) {
    return SG_BINARY_TRANSPORT_BINARY;
  }
  return SG_BINARY_TRANSPORT_DISCARD;
}

#ifdef ARDUINO

#include <Arduino.h>

#include "logger/log.h"

class BinaryEndpoint {
public:
  explicit BinaryEndpoint(HardwareSerial *entryPoint) : entryPoint(entryPoint) {}

  bool embrace(std::vector<uint8_t> *frame) {
    if (!entryPoint->available()) {
      return false;
    }

    if (entryPoint->peek() != SG_BINARY_SYNC_1) {
      return false;
    }

    std::vector<uint8_t> header;
    if (!readBytes(4, &header)) {
      Log::error("binary-endpoint", "Timed out while reading frame header");
      *frame = header;
      return !frame->empty();
    }

    if (header[0] != SG_BINARY_SYNC_1 || header[1] != SG_BINARY_SYNC_2) {
      Log::error("binary-endpoint", "Invalid binary sync prefix");
      *frame = header;
      return true;
    }

    uint16_t bodyLength = static_cast<uint16_t>(header[2]) |
                          static_cast<uint16_t>(static_cast<uint16_t>(header[3]) << 8);
    if (bodyLength > SG_BINARY_MAX_BODY_SIZE) {
      Log::error("binary-endpoint", "Binary frame exceeds configured body length");
      *frame = header;
      return true;
    }

    std::vector<uint8_t> remainder;
    if (!readBytes(bodyLength + SG_BINARY_FRAME_CRC_SIZE, &remainder)) {
      Log::error("binary-endpoint", "Timed out while reading binary frame body");
    }

    frame->clear();
    frame->reserve(header.size() + remainder.size());
    frame->insert(frame->end(), header.begin(), header.end());
    frame->insert(frame->end(), remainder.begin(), remainder.end());
    return true;
  }

  void unleash(const std::vector<uint8_t> &frame) {
    if (frame.empty()) {
      return;
    }
    entryPoint->write(frame.data(), frame.size());
  }

private:
  HardwareSerial *entryPoint;

  bool readBytes(size_t expectedSize, std::vector<uint8_t> *buffer) {
    buffer->clear();
    buffer->reserve(expectedSize);
    int loop = 0;
    while (buffer->size() < expectedSize) {
      while (entryPoint->available() && buffer->size() < expectedSize) {
        buffer->push_back(static_cast<uint8_t>(entryPoint->read()));
      }

      if (buffer->size() >= expectedSize) {
        return true;
      }

      loop++;
      delay(1);
      if (loop >= 50) {
        return false;
      }
    }
    return true;
  }
};

#endif

#endif // SG_MCU_BINARY_ENDPOINT_H
