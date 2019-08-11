//
// Created by Alfarie-MBP on 2019-08-06.
//
#include <Arduino.h>
#include <ArduinoJson.h>
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
public: explicit InvalidRequestFormatError(String message = "Request format is invalid") : ValidationError("invalid-request-format", message) {};
};

class TopicNotFoundError: public ValidationError {
public: explicit TopicNotFoundError(String message = "This topic is not found") : ValidationError("topic-not-found", message) {};
};

class InvalidMethodError: public ValidationError {
public: explicit InvalidMethodError(String message = "Method is invalid") : ValidationError("invalid-method", message) {};
};

class InvalidInputError: public ValidationError {
public: explicit InvalidInputError(String message = "Input is invalid") : ValidationError("invalid-input", message) {};
};

#endif //SG_MCU_VALIDATIONERROR_H
