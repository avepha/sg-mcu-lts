#ifndef SG_MCU_LTS_CHANNELTIMERENUM_H
#define SG_MCU_LTS_CHANNELTIMERENUM_H

enum ChannelTimerModeEnum {
  CH_TIMER_INTERVAL = 0,
  CH_TIMER_WORKING = 1,
  CH_TIMER_NONE = -1
};

String ChannelTimerModeEnumToString(ChannelTimerModeEnum anEnum) {
  switch (anEnum) {
    case CH_TIMER_INTERVAL:
      return "interval";
    case CH_TIMER_WORKING:
      return "working";
    default:
      return "none";
  }
}

ChannelTimerModeEnum ChannelTimerModeStringToEnum(const String& aString) {
  if (aString == "interval") {
    return CH_TIMER_INTERVAL;
  }
  else if (aString == "working") {
    return CH_TIMER_WORKING;
  }
  else {
    return CH_TIMER_NONE;
  }
}

#endif //SG_MCU_LTS_CHANNELTIMERENUM_H
