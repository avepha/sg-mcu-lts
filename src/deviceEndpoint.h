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
  if (!entryPoint->available()) {
    return false;
  }

  char raw[2048];
  int readIndex = 0;
  while (entryPoint->available())
  {
    char firstChar = (char)entryPoint->read();
    if (firstChar == '{') {
      raw[readIndex++] = '{';
      int loop = 0;
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
        else {
          // ensure this process can not hold resource more than 50ms
          loop++;
          delay(1);
          if (loop >= 50) {
            Debug::Print("Request timeout, request format is not invalid");
            return false; // timeout
          }
        }
      }
    }
    else {
      Debug::Print("got unknown byte " + String(firstChar, HEX));
    }
  }

  return false;
}

