#include <Arduino.h>
#include <ArduinoJson.h>
#include <queue>

#ifndef SG_MCU_LTS_EVENT_H
#define SG_MCU_LTS_EVENT_H

class EventManager {
public:
  class Event {
  public:
    Event(const String &topic, JsonObject data) {
      this->data = data;
      this->topic = topic;
    }
    String getTopic() {return topic;}
    StaticJsonDocument<1024> getData() {return data;}
  private:
    String topic;
    StaticJsonDocument<1024> data;
  };

  class EventTray {
  public:
    void push(const Event &event) {
      this->qEvent.push(event);
    }

    Event pop() {
      Event event = this->qEvent.front();
      this->qEvent.pop();
      return event;
    }

    bool isEmpty() {
      return this->qEvent.empty();
    }

  private:
    std::queue<Event> qEvent;
  };

  static void init() {
    if (!s_instance) {
      s_instance = new EventManager();
    }
  }

  static void create(const String &topic, JsonObject data) {
    s_instance->eventTray->push(Event(topic, data));
  }

  static bool isEmptyTray() {
    return s_instance->eventTray->isEmpty();
  }

  static Event popFromTray() {
    return s_instance->eventTray->pop();
  }

private:
  EventManager() {
    eventTray = new EventTray();
  }

  EventTray *eventTray;
  static EventManager *s_instance;
};

EventManager *EventManager::s_instance = nullptr;

#endif //SG_MCU_LTS_EVENT_H
