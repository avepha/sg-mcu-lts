#include <TaskScheduler.h>

#ifndef SG_MCU_GPIO_TASK_H
#define SG_MCU_GPIO_TASK_H

enum GpioTaskEnum {
  GPIO_TASK_ONCE = 0,
  GPIO_TASK_FOREVER = 1,
};

class GpioTask : public Task {
public:
  GpioTask(std::string name, uint8_t channel, void (*dWrite)(int channel, int value),
           void (*selfDestruct)(GpioTask *gpioTask))
      : Task(TASK_MINUTE, TASK_FOREVER, &gpioScheduler, false),
        name(std::move(name)),
        channel(channel),
        gpioTaskType(GPIO_TASK_FOREVER),
        dWrite(dWrite),
        selfDestruct(selfDestruct)
        {}

  GpioTask(std::string name, uint8_t channel, uint16_t timeout, void (*dWrite)(int channel, int value),
           void (*selfDestruct)(GpioTask *gpioTask))
      : Task(timeout, TASK_ONCE, &gpioScheduler, false),
        name(std::move(name)),
        channel(channel),
        gpioTaskType(GPIO_TASK_ONCE),
        dWrite(dWrite),
        selfDestruct(selfDestruct)
        {}

  ~GpioTask() {
    dWrite(channel, LOW);
  }

  std::string getTaskName() {
    return name;
  }

  uint8_t getChannel() {
    return channel;
  }

  bool Callback() override {
    if (gpioTaskType == GPIO_TASK_ONCE) {
      selfDestruct(this);
    }
    return true;
  }

  bool OnEnable() override {
    dWrite(channel, HIGH);
    return true;
  }

  void OnDisable() override {} // TODO: please find the cause. when call .disable(), system will be crashed.

private:
  std::string name;
  uint8_t channel;
  GpioTaskEnum gpioTaskType;
  void (*dWrite)(int channel, int value);

  void (*selfDestruct)(GpioTask *gpioTask);
};

#endif //SG_MCU_GPIO_TASK_H
