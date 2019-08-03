#include <ArduinoJson.h>
#include "Task.h"

class EndPoint : public Task
{
public:
  EndPoint() : Task(MsToTaskTime(5)){};

private:
  virtual bool OnStart()
  {
    return true;
  }

  virtual void OnUpdate(uint32_t delta_time)
  {
    if (!mpuCom.available()) return;
    
    // waiting for first byte
    int readIndex = 0;
    char raws[1024];
    char ch;
    while (ch = mpuCom.read() !=  '{');
    raws[readIndex++] = ch;

    while (ch = mpuCom.read() != '}')
    {
      raws[readIndex++] = ch;
    }
    
    raws[readIndex] = ch;
    
    Serial.println(raws);
    
    // DynamicJsonDocument jsonRequest = jsonParse(raws);

    // String method = jsonRequest["method"];
    // String topic = jsonRequest["topic"];
    // String data = jsonRequest["data"];
  }

  DynamicJsonDocument jsonParse(char *raws) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, raws);
    return doc;
  }
};