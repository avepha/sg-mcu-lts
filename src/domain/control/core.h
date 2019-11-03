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

    checkAndActiveControl(type);
  }

  CONTROL_ENUM getType() {
    return type;
  }

  bool checkAndActiveControl(CONTROL_ENUM _type) {
    if (_type == type) {
      return false;
    }

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

    return true;
  }

private:
  ChannelCore *channelCore;
  CONTROL_ENUM type = CTRL_NONE;
};
#endif //SG_MCU_CONTROL_CORE_H
