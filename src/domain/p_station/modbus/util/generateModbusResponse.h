#include <Arduino.h>
#include <vector>

#include "../modbusPacket.h"
#ifndef SG_MCU_GENERATE_MODBUS_RESPONSE_H
#define SG_MCU_GENERATE_MODBUS_RESPONSE_H

class GenerateModbusResponse {
public:
  static std::vector<byte> genGSensorPacket(uint8_t address) {
    float sensors[8];
    sensors[0] = (float)random(2500, 2550) / 100; // temperature
    sensors[1] = (float)random(5000, 5050) / 100; // humidity
    sensors[2] = getVpd(sensors[0], sensors[1]); // vpd
    sensors[3] = (float)random(2500, 2550) / 100; // soil temperature
    sensors[4] = (float)random(5000, 5050) / 100; // soil moisture
    sensors[5] = (float)random(1500, 1510); // co2
    sensors[6] = (float)random(150, 160); // par
    sensors[7] += sensors[6] + (float)random(150, 160); // par acc

    byte data[sizeof(sensors)];
    memcpy(data, sensors, sizeof(sensors));
    ModbusPacket packet(address, 0x04, data, sizeof(data));

    return packet.getVectorPacket();
  }
  static std::vector<byte> genGSensorPacketWithFailedAirModule(uint8_t address) {
    float sensors[8];
    sensors[0] = (float)0xFFFFFFFF; // temperature
    sensors[1] = (float)0xFFFFFFFF; // humidity
    sensors[2] = (float)0xFFFFFFFF; // vpd
    sensors[3] = (float)random(2500, 2550) / 100; // soil temperature
    sensors[4] = (float)random(5000, 5050) / 100; // soil moisture
    sensors[5] = (float)random(1500, 1510); // co2
    sensors[6] = (float)random(150, 160); // par
    sensors[7] += sensors[6] + (float)random(150, 160); // par acc

    byte data[sizeof(sensors)];
    memcpy(data, sensors, sizeof(sensors));
    ModbusPacket packet(address, 0x04, data, sizeof(data));

    return packet.getVectorPacket();
  }

  static std::vector<byte> getSolutionPacket(uint8_t address) {
    float sensors[3];
    sensors[0] = (float)random(150, 160) / 100; // ec
    sensors[1] = (float)random(600, 650) / 100; // ph
    sensors[2] = (float)random(2500, 2550) / 100; // water temperature

    byte data[sizeof(sensors)];
    memcpy(data, sensors, sizeof(sensors));
    ModbusPacket packet(address, 0x04, data, sizeof(data));
    return packet.getVectorPacket();
  }
  static std::vector<byte> getSolutionPacketWithFailedModule(uint8_t address) {
    float sensors[3];
    sensors[0] = (float)0xFFFFFFFF; // ec
    sensors[1] = (float)0xFFFFFFFF; // ph
    sensors[2] = (float)0xFFFFFFFF; // water temperature

    byte data[sizeof(sensors)];
    memcpy(data, sensors, sizeof(sensors));
    ModbusPacket packet(address, 0x04, data, sizeof(data));
    return packet.getVectorPacket();
  }

  static std::vector<byte> getGSolutionPacket(uint8_t address) {
    float sensors[7];
    sensors[0] = (float)random(2500, 2550) / 100; // temperature
    sensors[1] = (float)random(5000, 5050) / 100; // humidity
    sensors[2] = getVpd(sensors[0], sensors[1]); // vpd
    sensors[3] = (float)random(1500, 1510); // co2
    sensors[4] = (float)random(150, 160) / 100; // ec
    sensors[5] = (float)random(600, 650) / 100; // ph
    sensors[6] = (float)random(2500, 2550) / 100; // water temperature

    byte data[sizeof(sensors)];
    memcpy(data, sensors, sizeof(sensors));
    ModbusPacket packet(address, 0x04, data, sizeof(data));
    return packet.getVectorPacket();
  }
private:
  static float getVpd(float _temperature, float _humidity) {
    float spv = 610.7 * pow(10, ((7.5 * _temperature) / (237.3 + _temperature)));
    return (1 - (_humidity / 100)) * spv;
  }
};
#endif
