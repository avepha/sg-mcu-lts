#include <ArduinoJson.h>
#include "Task.h"

class EndPoint : public Task
{
public:
  EndPoint() : Task(MsToTaskTime(1)){};

private:
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    if (!mpuCom.available()) return;
    
    // waiting for first byte
    while (mpuCom.read() !=  '{');

    int readIndex = 0;
    char raws[1024];
    while (char ch = mpuCom.read() != '}')
    {
      raws[readIndex++] = ch;
    }

  }

  DynamicJsonDocument jsonParse(char *raws) {
   
  }
};