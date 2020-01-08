#include <Arduino.h>
#include "../station.h"
#include "../modbus/modbusPacket.h"

#ifndef SG_MCU_GSENSOR_STATION_H
#define SG_MCU_GSENSOR_STATION_H

class GSensorStation : public Station {
public:
  explicit GSensorStation(uint8_t addr) : Station(STATION_GSENSOR, addr) {

  }

  void onPacketReceived(const std::vector<byte> &vPacket) override {
    Serial.print("gsensor onPacketReceived: ");
    for (std::size_t i = 0; i < vPacket.size(); i++) {
      Serial.print(" ");
      Serial.print(vPacket[i], HEX);
    }
    Serial.println();

    byte address = vPacket[0];
    byte funcCode = vPacket[1];
    uint8_t dataSize = vPacket.size() - 4;
    byte data[dataSize];
    memcpy(data, vPacket.data() + 2, dataSize);

    if (funcCode == 0x04 && vPacket.size() >= 36) {
      float sensors[dataSize / 4];
      for (uint16_t dataIndex = 0, sensorIndex = 0; dataIndex < sizeof(data); dataIndex+=4, sensorIndex++) {
        memcpy(&sensors[sensorIndex], &data[dataIndex], 4);
      }

      Serial.print("func 0x04 got sensors: ");
      for (uint16_t i = 0 ; i < 6; i++) {
        Serial.print(" ");
        Serial.print(sensors[i], HEX);
      }
      Serial.println();
    }
    else {
      ;;
    }

//    byte packets[50];
//    uint16_t size = vPacket.size();
//    memcpy(packets, vPacket.data(), size);
//
//    byte address = packets[0];
//    byte funcCode = packets[1];
//
//    if (funcCode == 0x04) { // sensors
//      byte *firstDataByte = &packets[2];
//      for (uint16_t i = 0 ; i < size; i++) {
//
//      }
//    }
  }
};

#endif
