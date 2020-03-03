class LoraEndpoint {
public:
  LoraEndpoint(HardwareSerial *entryPoint);

  bool embrace(String *message);

  void unleash(String message);

private:
  HardwareSerial *entryPoint;
};

LoraEndpoint::LoraEndpoint(HardwareSerial *entryPoint) : entryPoint(entryPoint) {}

void LoraEndpoint::unleash(String message) {
  entryPoint->println(message);
}

bool LoraEndpoint::embrace(String *message) {
  if (!entryPoint->available()) {
    return false;
  }
  
  Serial.println("Got: " + String(entryPoint->available()));

  while(entryPoint->available()) {
    Serial.print(entryPoint->read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  return true;
}

