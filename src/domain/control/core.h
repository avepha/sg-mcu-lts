#include "./model.h"
#include "./controlEnum.h"
#include "domain/channel/core.h"
#include "domain/continuous/core.h"

#ifndef SG_MCU_CONTROL_CORE_H
#define SG_MCU_CONTROL_CORE_H

class ControlCore {
public:
  ControlCore() {
    ControlModel model;
    type = model.get().type;

    channelCore = ChannelCore::instance();
    continuousCore = ContinuousCore::instance();

    activateControl(type);
  }

  CONTROL_ENUM getType() {
    return type;
  }

  bool checkAndActiveControl(CONTROL_ENUM _type) {
    if (_type == type) {
      return false;
    }
    activateControl(_type);

    return true;
  }

private:
  ChannelCore *channelCore;
  ContinuousCore *continuousCore;
  CONTROL_ENUM type = CTRL_NONE;
  void activateControl(CONTROL_ENUM _type) {
    type = _type;
    channelCore->deactivateControls();
    continuousCore->deactivateControls();

    switch (_type) {
      case CTRL_CHANNEL: {
        channelCore->activateControls();
      }
      case CTRL_CONTINUOUS: {
        continuousCore->activateControls();
      }
      default: {

      }
    }
  }
};
#endif //SG_MCU_CONTROL_CORE_H
