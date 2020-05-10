#include <Arduino.h>
#include <FastCRC.h>
#include <vector>

#ifndef SG_MCU_MODBUSPACKET_H
#define SG_MCU_MODBUSPACKET_H

FastCRC16 crc16;

class ModbusPacket {
public:
  static bool verifyPacket(const std::vector<byte>& vPacket) {
    // packet table: [address(1)][func(1)][numberOfData(1)][data(n * byte)][crc(2)]
    uint16_t calculatedCrc = crc16.modbus(vPacket.data(), vPacket.size() - 2); // tail 2 bytes, head 2 bytes
    uint16_t responseCrc;
    memcpy(&responseCrc, &vPacket[vPacket.size() - 2], sizeof(byte) * 2);
    return calculatedCrc == responseCrc;
  }

  ModbusPacket(byte _addr, byte _func, byte *_data, uint16_t _size) : address(_addr), func(_func), dataSize(_size) {
    memcpy(data, _data, _size);
    byte packets[_size + 3];
    packets[0] = _addr;
    packets[1] = _func;
    packets[2] = _size;
    memcpy(&packets[3], data, _size);
    crc = crc16.modbus(packets, sizeof(packets));
  }

  std::vector<byte> getVectorPacket() {
    std::vector<byte> _packets(dataSize + sizeof(address) + sizeof(crc) + sizeof(func) + 1); // data len
    _packets[0] = address;
    _packets[1] = func;
    _packets[2] = sizeof(data);
    memcpy(&_packets[3], data, dataSize);
    _packets[dataSize + sizeof(address) + sizeof(func) + 1] = crc & 0xff;
    _packets[dataSize + sizeof(address) + sizeof(func) + 2] = crc >> 8;

    return _packets;
  }

private:
  byte address;
  byte func;
  byte data[50];
  uint16_t dataSize;
  uint16_t crc; // 16 bit crc
};

#endif //SG_MCU_MODBUSPACKET_H
