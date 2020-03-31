#include <Arduino.h>
#ifndef SG_MCU_LOGGERUTIL_H
#define SG_MCU_LOGGERUTIL_H

enum LoggerLevelEnum {
  NONE = 0,
  FATAL = 1,
  ERROR = 2,
  WARNING = 3,
  INFO = 4,
  DEBUG = 5,
  TRACE = 6,
};

LoggerLevelEnum StringToLoggerLevelEnum(String st) {
  if (st == "fatal")
    return FATAL;
  else if (st == "error")
    return ERROR;
  else if (st == "warning")
    return WARNING;
  else if (st == "info")
    return INFO;
  else if (st == "debug")
    return DEBUG;
  else if (st == "trace")
    return TRACE;
  else
    return NONE;
}

String LoggerLevelEnumToString(LoggerLevelEnum level) {
  switch (level) {
    case FATAL:
      return "fatal";
    case ERROR:
      return "error";
    case WARNING:
      return "warning";
    case INFO:
      return "info";
    case DEBUG:
      return "debug";
    case TRACE:
      return "trace";
  }
  return "none";
}


#endif
