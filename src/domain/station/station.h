#include "./modbus/modbusPacket.h"

#ifndef SG_MCU_STATION_H
#define SG_MCU_STATION_H

enum STATION_ENUM {
  STATION_GSENSOR,
  STATION_SOLUTION
};

class Station {
public:
  Station(STATION_ENUM _type, uint8_t _address) {
    type = _type;
    address = _address;
    lastSeen = 0;

    byte data[4] = {0x00, 0x0, 0x0, 0x0};
    requestPacket = new ModbusPacket(address, 0x04, data, sizeof(data));
  }

  STATION_ENUM getType() {
    return type;
  }

  uint8_t getAddress() {
    return address;
  }

  uint32_t getLastSeen() {
    return lastSeen;
  }

  bool isAvailable() {
    return lastSeen != 0 && millis() - lastSeen < 1000 * 10; // never updated or 10 seconds
  }

  virtual void onPacketReceived(const std::vector<byte> &vPacket) = 0;

  ModbusPacket *getRequest() {
    return requestPacket;
  }

private:
  STATION_ENUM type;
  uint8_t address;
  ModbusPacket *requestPacket;
  uint32_t lastSeen;
};

#endif //SG_MCU_STATION_H
