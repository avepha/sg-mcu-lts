//
// Created by Farhan Poharsae on 21/3/2021 AD.
//

#ifndef SG_MCU_LTS_GPIOSTATE_H
#define SG_MCU_LTS_GPIOSTATE_H

class GpioState {
public:
  static GpioState *instance() {
    if (!s_instance)
      s_instance = new GpioState;

    return s_instance;
  }

  void setState(uint8_t ch, uint8_t st) {
    state[ch] = st;
  }

  uint8_t* getStates() {
    return state;
  }

private:
  GpioState() = default;

  static GpioState *s_instance;
  uint8_t state[8] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
};

GpioState *GpioState::s_instance = nullptr;

#endif //SG_MCU_LTS_GPIOSTATE_H
