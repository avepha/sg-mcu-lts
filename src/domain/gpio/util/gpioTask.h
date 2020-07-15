#include <TaskScheduler.h>

#ifndef SG_MCU_GPIO_TASK_H
#define SG_MCU_GPIO_TASK_H

enum GpioTaskEnum {
  GPIO_TASK_EXPIRED = 0,
  GPIO_TASK_NO_EXPIRED = 1,
};

class GpioTask : public Task {
public:
  GpioTask(const std::string &uid, uint8_t channel, void (*dWrite)(int channel, int value),
           void (*finishCallback)(GpioTask *gpioTask))
      : Task(TASK_MINUTE, TASK_FOREVER, &gpioScheduler, false),
        uid(std::move(uid)),
        channel(channel),
        gpioTaskType(GPIO_TASK_NO_EXPIRED),
        dWrite(dWrite),
        finishCallback(finishCallback)
        {
          Log::debug("gpio", "create task (" + String(uid.c_str()) + ") forever");
        }

  GpioTask(const std::string &uid, uint8_t channel, uint32_t timeout, void (*dWrite)(int channel, int value),
           void (*finishCallback)(GpioTask *gpioTask))
      : Task(timeout, TASK_FOREVER, &gpioScheduler, false),
        uid(std::move(uid)),
        channel(channel),
        gpioTaskType(GPIO_TASK_EXPIRED),
        dWrite(dWrite),
        finishCallback(finishCallback)
        {
          Log::debug("gpio", "create task (" + String(uid.c_str()) + ") with timeout " + String(timeout));
          setInterval(timeout);
          setTimeout(timeout);
        }

  ~GpioTask() {
    dWrite(channel, LOW);
  }

  std::string getUid() {
    return uid;
  }

  uint8_t getChannel() {
    return channel;
  }

  GpioTaskEnum getType() {
    return gpioTaskType;
  }

  bool Callback() override {
    if (gpioTaskType == GPIO_TASK_EXPIRED) {
      finishCallback(this);
    }

    if (this->getTimeout() < 0) {
      finishCallback(this);
    }
    return true;
  }

  bool OnEnable() override {
    dWrite(channel, HIGH);
    return true;
  }

  void OnDisable() override {

  } // TODO: please find the cause. when call .disable(), system will be crashed.

private:
  std::string uid;
  uint8_t channel;
  GpioTaskEnum gpioTaskType;
  void (*dWrite)(int channel, int value);

  void (*finishCallback)(GpioTask *gpioTask);
};

#endif //SG_MCU_GPIO_TASK_H
