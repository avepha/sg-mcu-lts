#include "util/converter.h"

class SensorEndpoint {
public:
  SensorEndpoint(HardwareSerial *entryPoint);

  int embrace(byte *message);

  void unleash(String message);

private:
  HardwareSerial *entryPoint;
};

SensorEndpoint::SensorEndpoint(HardwareSerial *entryPoint) : entryPoint(entryPoint) {}

void SensorEndpoint::unleash(String message) {
  entryPoint->println(message);
}

int SensorEndpoint::embrace(byte *bytes) {
  while (!entryPoint->available()) {
    delay(5);
    return -1;
  }

  byte raw[64];
  int readIndex = 0;
  while (entryPoint->available()) {
    if (entryPoint->read() == 0xEE) {
      int loop = 0;
      while (true) {
        if (entryPoint->available()) {
          byte byteVal = entryPoint->read();
          if (byteVal == 0xEF) {
            byte checkSum = modsum(raw, readIndex - 1);
            memccpy(bytes, raw, checkSum, readIndex - 1); // bytes = [sta, payload]
            return checkSum == raw[readIndex - 1] ? readIndex - 1: -2; // return -2 = wrong checksum
          } else {
            raw[readIndex++] = byteVal;
          }
          loop = 0; // reset counter when data is coming
        } else {
          // ensure this process can not hold resource more than 50ms
          loop++;
          delay(1);
          if (loop >= 50) return -3; // timeout
        }
      }
    }
  }

  return -1;
}

