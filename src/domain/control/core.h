#include "./model.h"
#include "./controlEnum.h"
#include "domain/channel/core.h"

#ifndef SG_MCU_CONTROL_CORE_H
#define SG_MCU_CONTROL_CORE_H

class ControlCore {
public:
  ControlCore() {
    ControlModel model;
    type = model.get().type;

    channelCore = ChannelCore::instance();

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
  CONTROL_ENUM type = CTRL_NONE;
  void activateControl(CONTROL_ENUM _type) {
    type = _type;
    channelCore->deactivateControls();

    switch (_type) {
      case CTRL_CHANNEL: {
        channelCore->activateControls();
      }
      case CTRL_CONTINUOUS: {

      }
      default: {

      }
    }
  }
};
#endif //SG_MCU_CONTROL_CORE_H
