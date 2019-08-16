class DeviceEndpoint
{
public:
  DeviceEndpoint(HardwareSerial* entryPoint);

  bool embrace(String *message);
  void unleash(String message);
private:
  HardwareSerial *entryPoint;
};

DeviceEndpoint::DeviceEndpoint(HardwareSerial *entryPoint): entryPoint(entryPoint) {}

void DeviceEndpoint::unleash(String message) {
  entryPoint->println(message);
}

bool DeviceEndpoint::embrace(String *message) {
  while (!entryPoint->available()) {
    delay(10);
    return false;
  }

  char raw[1024];
  int readIndex = 0;
  while (entryPoint->available())
  {
    if (entryPoint->read() == '{') {
      raw[readIndex++] = '{';
      while(true) {
        if (entryPoint->available()) {
          char ch = (char)entryPoint->read();
          if (ch == '\n') {
            raw[readIndex] = '\0';
            *message = String(raw);
            return true;
          }
          else {
            raw[readIndex++] = ch;
          }
        }
      }
    }
  }

  return false;
}

