//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <ArduinoJson.h>
#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  Resolvers(String name);
  String name;
  virtual String resolve(){
    StaticJsonDocument<1024> json;
    String jsonString;
    json["topic"] = "default_resolver";
    json["method"] = "query";
    json["data"] = VERSION;

    serializeJson(json, jsonString);
    return jsonString;
  };
};

Resolvers::Resolvers(String name):name(name) {};


#endif //SG_MCU_RESOLVERS_H
