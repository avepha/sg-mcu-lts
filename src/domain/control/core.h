#include "./model.h"
#include "./controlEnum.h"
#include "domain/channel/core.h"
#include "domain/sequence/core.h"

#ifndef SG_MCU_CONTROL_CORE_H
#define SG_MCU_CONTROL_CORE_H

class ControlCore {
public:
  ControlCore() {
    ControlModel model;
    type = model.get().type;

    channelCore = ChannelCore::instance();
    sequenceCore = SequenceCore::instance();

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
  SequenceCore *sequenceCore;
  CONTROL_ENUM type = CTRL_NONE;

  void activateControl(CONTROL_ENUM _type) {
    type = _type;
    channelCore->deactivateControls();
    sequenceCore->deactivateControls();

    switch (_type) {
      case CTRL_CHANNEL: {
        Debug::Print("CTRL-CHANNEL started");
        channelCore->activateControls();
        return;
      }
      case CTRL_SEQUENCE: {
        Serial.println("CTRL-SEQUENCE started");
        sequenceCore->activateControls();
        return;
      }
      default: {
        return;
      }
    }
  }
};

#endif //SG_MCU_CONTROL_CORE_H
