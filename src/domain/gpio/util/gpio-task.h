#include <TaskScheduler.h>

#include <utility>

#ifndef SG_MCU_GPIO_TASK_H
#define SG_MCU_GPIO_TASK_H

class GpioTask : public Task {
public:
  GpioTask(std::string name, uint8_t channel, void (*dWrite)(int channel, int value), void (*selfDestruct)(GpioTask *gpioTask))
      : Task(TASK_MINUTE, TASK_FOREVER, &gpioScheduler, false),
        name(std::move(name)),
        channel(channel),
        dWrite(dWrite),
        selfDestruct(selfDestruct){}

  GpioTask(std::string name, uint8_t channel, uint16_t timeout, void (*dWrite)(int channel, int value), void (*selfDestruct)(GpioTask *gpioTask))
      : Task(timeout, TASK_ONCE, &gpioScheduler, false),
        name(std::move(name)),
        channel(channel),
        dWrite(dWrite),
        selfDestruct(selfDestruct){}

  std::string getTaskName() {
    return name;
  }

  uint8_t getChannel() {
    return channel;
  }

  bool Callback() override {
    return true;
  }

  bool OnEnable() override {
    dWrite(channel, HIGH);
    return true;
  }

  void OnDisable() override {
    dWrite(channel, LOW);
    selfDestruct(this);
  }

private:
  std::string name;
  uint8_t channel;

  void (*dWrite)(int channel, int value);
  void (*selfDestruct)(GpioTask *gpioTask);
};

#endif //SG_MCU_GPIO_TASK_H
