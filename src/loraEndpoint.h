class LoraEndpoint {
public:
  LoraEndpoint(HardwareSerial *entryPoint);

  bool embrace(std::vector<byte> &vByte);

  void unleash(String message);

private:
  HardwareSerial *entryPoint;
};

LoraEndpoint::LoraEndpoint(HardwareSerial *entryPoint) : entryPoint(entryPoint) {}

void LoraEndpoint::unleash(String message) {
  entryPoint->println(message);
}

bool LoraEndpoint::embrace(std::vector<byte> &vByte) {
  if (!entryPoint->available()) {
    return false;
  }

  if (entryPoint->available()) {
    uint32_t ts = micros();
    while (true) {
      if (entryPoint->available()) {
        vByte.push_back(entryPoint->read());
        ts = micros();
      }

      if (micros() - ts >= 10000) {
        if (!ModbusPacket::verifyPacket(vByte)) {
          Log::error("lora", "crc failure");
          return false;
        }
        else {
          return true;
        }
      }
    }
  }

  return false;
}

