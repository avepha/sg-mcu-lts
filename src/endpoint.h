class EndPoint
{
public:
  EndPoint(HardwareSerial* entryPoint);

  String embrace();
  void unleash(String message);
private:
  HardwareSerial *entryPoint;
};

EndPoint::EndPoint(HardwareSerial *entryPoint): entryPoint(entryPoint) {}

void EndPoint::unleash(String message) {
  entryPoint->println(message);
}

String EndPoint::embrace() {
  while (!entryPoint->available())
    delay(10);

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
            String message = raw;
            return message;
          }
          else {
            raw[readIndex++] = ch;
          }
        }
      }
    }
  }

  return "NULL";
}

