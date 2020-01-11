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
  String reqId = "";

  String toJsonString() {
    DynamicJsonDocument json(200);
    json["topic"] = topic;
    json["code"] = code;
    json["message"] = message;
    json["reqId"] = reqId == "" ? (char*)0 : reqId;

    String jsonString;
    serializeJson(json, jsonString);
    return jsonString;
  }

  JsonDocument toJson() {
    DynamicJsonDocument json(200);
    json["topic"] = topic;
    json["code"] = code;
    json["message"] = message;
    json["reqId"] = reqId == "" ? (char*)0 : reqId;

    return json;
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

class IndexNotSpecifyError: public ValidationError {
public: explicit IndexNotSpecifyError(String message = "Index is not specify") : ValidationError("index-not-specify", message) {};
};

class IndexOutOfBoundError: public ValidationError {
public: explicit IndexOutOfBoundError(String message = "Index out of bound") : ValidationError("index-out-of-bound", message) {};
};

class NSensorInvalidCheckSumError: public ValidationError {
public: explicit NSensorInvalidCheckSumError(String message = "NSensor checksum is invalid") : ValidationError("invalid-checksum-nsensor", message) {};
};

class NSensorTimeoutError: public ValidationError {
public: explicit NSensorTimeoutError(String message = "NSensor is timeout") : ValidationError("nsensor-timeout", message) {};
};

class PrecondtionExceedError: public ValidationError {
public: explicit PrecondtionExceedError(String message = "Precondition per control is exceeded") : ValidationError("precondition-exceeded", message) {};
};

class InactiveControlError: public ValidationError {
public: explicit InactiveControlError(String message = "Control is inactive") : ValidationError("inactive-control", message) {};
};

class InactiveChannelControlError: public ValidationError {
public: explicit InactiveChannelControlError(String message = "Current control type is not Channel") : ValidationError("inactive-channel-control", message) {};
};

class InactiveSequenceControlError: public ValidationError {
public: explicit InactiveSequenceControlError(String message = "Current control type is not Sequence") : ValidationError("inactive-sequence-control", message) {};
};

class NoControlStateError: public ValidationError {
public: explicit NoControlStateError(String message = "Control has no state.") : ValidationError("no-control-state", message) {};
};

class StationNotFoundError: public ValidationError {
public: explicit StationNotFoundError(String message = "Station is not found") : ValidationError("no-station", message) {};
};

class SensorNotFoundError: public ValidationError {
public: explicit SensorNotFoundError(String message = "No sensor") : ValidationError("no-sensor", message) {};
};

#endif //SG_MCU_VALIDATIONERROR_H
