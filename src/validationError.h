//
// Created by Alfarie-MBP on 2019-08-06.
//
#include <Arduino.h>
#ifndef SG_MCU_VALIDATIONERROR_H
#define SG_MCU_VALIDATIONERROR_H


class ValidationError {
public:
  ValidationError(String code, String message):
    topic("Error"),
    message(message),
    code(code)
    {};
  String topic;
  String message;
  String code;

  String toJsonString() {
    StaticJsonDocument<200> doc;
    doc["topic"] = topic;
    doc["code"] = code;
    doc["message"] = message;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
  }
};

class InvalidJsonFormatError: public ValidationError {
public: InvalidJsonFormatError() : ValidationError("invalid-json-format", "Wrong Json format") {};
};

class InvalidRequestFormatError: public ValidationError {
public: InvalidRequestFormatError(String message = "Request format is invalid") : ValidationError("invalid-request-format", message) {};
};


#endif //SG_MCU_VALIDATIONERROR_H
