#include <Task.h>

extern TaskManager taskManager;

class PingMessage : public Task {
public:
  static PingMessage *s_instance;

  PingMessage() : Task(MsToTaskTime(1000)) {};

  static PingMessage *instance() {
    if (!s_instance)
      s_instance = new PingMessage;
    return s_instance;
  }

private:
  float currentTime;

  virtual bool OnStart() {
    currentTime = 0;
    return true;

  }

  virtual void OnUpdate(uint32_t delta_time) {
    Serial.println("#PING#");
    mpuCom.println("#PING#");
  }
};

PingMessage *PingMessage::s_instance = 0;