#include <Arduino.h>
#include <queue>
#include "loggerUtil.h"
#include "domain/configuration/model.h"

#ifndef SG_MCU_LOG_H
#define SG_MCU_LOG_H

class Logger {
public:
  Logger(LoggerLevelEnum lv, String tp, String msg) {
    topic = std::move(tp);
    message = std::move(msg);
    level = lv;
  }

  LoggerLevelEnum getLevel() {
    return level;
  }

  String getLevelString() {
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
      case NONE:
        return "none";
    }

    return "none";
  }

  String getMessage() {
    return message;
  }

  String getTopic() {
    return topic;
  }

private:
  LoggerLevelEnum level;
  String message;
  String topic;
};

class LoggerTray {
public:
  void push(const Logger &log) {
    this->qLoggers.push(log);
  }

  Logger pop() {
    Logger logger = this->qLoggers.front();
    this->qLoggers.pop();
    return logger;
  }

  bool isEmpty() {
    return this->qLoggers.empty();
  }

private:
  std::queue<Logger> qLoggers;
};

class Log {
public:
  static void updateLogLevel();

  static LoggerLevelEnum getLogLevel();

  static void setLoggerTray(LoggerTray *);

  static LoggerTray *getLoggerTray();

  static void fatal(const String &, const String &);

  static void error(const String &, const String &);

  static void warn(const String &, const String &);

  static void info(const String &, const String &);

  static void debug(const String &, const String &);

  static void trace(const String &, const String &);

private:
  static LoggerTray *tray;
  static LoggerLevelEnum level;

  static bool isValidLevel(LoggerLevelEnum _level) {
    return level >= _level;
  }
};

LoggerLevelEnum Log::level = FATAL;
LoggerTray *Log::tray = nullptr;

void Log::fatal(const String &topic, const String &message) {
  if (!isValidLevel(FATAL)) return;
  Logger logger(FATAL, topic, message);
  tray->push(logger);
}

void Log::error(const String &topic, const String &message) {
  if (!isValidLevel(ERROR)) return;
  Logger logger(ERROR, topic, message);
  tray->push(logger);
}

void Log::warn(const String &topic, const String &message) {
  if (!isValidLevel(WARNING)) return;
  Logger logger(WARNING, topic, message);
  tray->push(logger);
}

void Log::info(const String &topic, const String &message) {
  if (!isValidLevel(INFO)) return;
  Logger logger(INFO, topic, message);
  tray->push(logger);
}

void Log::debug(const String &topic, const String &message) {
  if (!isValidLevel(DEBUG)) return;
  Logger logger(DEBUG, topic, message);
  tray->push(logger);
}

void Log::trace(const String &topic, const String &message) {
  if (!isValidLevel(TRACE)) return;
  Logger logger(TRACE, topic, message);
  tray->push(logger);
}

LoggerTray *Log::getLoggerTray() {
  return tray;
}

void Log::setLoggerTray(LoggerTray *_tray) {
  tray = _tray;
}

void Log::updateLogLevel() {
  ConfigurationSchema schema = ConfigurationModel().get();
  level = schema.logLevel;
}

LoggerLevelEnum Log::getLogLevel() {
  return level;
}


#endif //SG_MCU_LOG_H
