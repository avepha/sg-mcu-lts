#include "bootstrap.h"
#include <unity.h>
#include <vector>

using namespace std;

#ifndef SG_MCU_RS485_H
#define SG_MCU_RS485_H

void rs485BasicCommand() {
  byte data[4] = {0, 0, 0, 0};
  ModbusPacket packet(1, 4, data, sizeof(data));

  sendDataToStation(packet.getVectorPacket().data(), packet.getVectorPacket().size());

  uint32_t ts = millis();
  while (!stationPort.available() && millis() - ts <= 1000) {}

  if (millis() - ts >= 1000) {
    TEST_ASSERT_MESSAGE(false, "RS485 timeout");
  }

  TEST_ASSERT_MESSAGE(true, "RS485 success response");

  vector<byte> vByte;

  while (stationPort.available()) {
    vByte.push_back(stationPort.read());
  }

  Serial.print("Got packet: ");
  for (int i = 0; i < vByte.size(); i++) {
    Serial.print(vByte[i], HEX); Serial.print(" ");
  }
  Serial.println();

  TEST_ASSERT_MESSAGE(ModbusPacket::verifyPacket(vByte), "Modbus packet must be verified")
}

#endif
