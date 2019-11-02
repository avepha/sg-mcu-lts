#ifndef SG_MCU_CONTINUOUS_CORE_H
#define SG_MCU_CONTINUOUS_CORE_H

class ContinuousCore {
public:
  ContinuousCore() {
    // initialize pin
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }
};

#endif //SG_MCU_CONTINUOUS_CORE_H
