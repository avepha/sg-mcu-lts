#include <Arduino.h>
#include "./haftFloat.h"

#ifndef SG_MCU_UTIL_CONVERTER_H
#define SG_MCU_UTIL_CONVERTER_H

void printBytes(byte *values, byte size)
{
    for (int i = 0; i < size; i++)
    {
        Serial.print(values[i], HEX);
        Serial.print(' ');
    }
    Serial.println();
}

void floatToBytes(float val, byte *bytes_array)
{
    union {
        float float_variable;
        byte temp_array[4];
    } u;
    u.float_variable = val;
    memcpy(bytes_array, u.temp_array, 4);
}

void Uint16ToBytes(uint16_t val, byte *bytes_array) {
  union {
    uint16_t variable;
    byte temp_array[2];
  } u;
  u.variable = val;
  memcpy(bytes_array, u.temp_array, 2);
}

uint16_t BytesToUint16(byte* bytes_array) {
  union {
    uint16_t variable;
    byte temp_array[2];
  } u;
  memcpy(u.temp_array, bytes_array, 2);
  return u.variable;
}

byte modsum(byte *payload, byte payloadSize) {
  int sum = 0;
  for(int i = 0; i < payloadSize; i++) {
    sum += payload[i];
  }

  return sum % 255;
}

void restoreBytesArrayToSensorPayload(byte *bytesPayload, float *sensorBucket, int sensorBucketSize) {
  for(int i = 0 ; i < sensorBucketSize; i++) {
    byte bytes[2];
    memcpy(bytes, &bytesPayload[sizeof(uint16_t) * i], 2);
    sensorBucket[i] = Float16Compressor::decompress(BytesToUint16(bytes));
  }
}

#endif