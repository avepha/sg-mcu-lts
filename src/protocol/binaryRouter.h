#ifndef SG_MCU_BINARY_ROUTER_H
#define SG_MCU_BINARY_ROUTER_H

#include <string>
#include <vector>

#include "binaryFrame.h"

struct BinaryInfoSnapshot {
  std::string version;
  std::string model;
  Phase1Info_DeviceEnvironment deviceEnvironment;
};

struct BinaryRouterResult {
  uint8_t status;
  std::vector<uint8_t> responseFrame;
};

static inline BinaryInfoSnapshot binaryMakeInfoSnapshot(
    const char *version,
    const char *model,
    Phase1Info_DeviceEnvironment deviceEnvironment) {
  BinaryInfoSnapshot snapshot = {
      version == nullptr ? "" : version,
      model == nullptr ? "" : model,
      deviceEnvironment,
  };
  return snapshot;
}

static inline BinaryRouterResult binaryMakeStatusResponse(
    const BinaryEnvelope &requestEnvelope,
    uint8_t status) {
  BinaryEnvelope responseEnvelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_RESPONSE,
      requestEnvelope.operationId == 0 ? SG_BINARY_OPERATION_INFO_QUERY : requestEnvelope.operationId,
      requestEnvelope.requestId,
      status,
      0);
  BinaryRouterResult result = {
      status,
      binaryFrameEncode(responseEnvelope, {}),
  };
  return result;
}

static inline BinaryRouterResult binaryRouteFrame(
    const std::vector<uint8_t> &requestFrame,
    const BinaryInfoSnapshot &snapshot) {
  BinaryFrameDecodeResult decoded = binaryFrameDecode(requestFrame);
  if (decoded.status != SG_BINARY_STATUS_OK) {
    return binaryMakeStatusResponse(decoded.envelope, decoded.status);
  }

  if (decoded.envelope.messageType != SG_BINARY_MESSAGE_TYPE_REQUEST) {
    return binaryMakeStatusResponse(decoded.envelope, SG_BINARY_STATUS_BAD_FRAME);
  }

  Phase1InfoQueryRequest request = Phase1InfoQueryRequest_init_default;
  if (!binaryDecodeInfoQueryRequestPayload(decoded.payload, &request)) {
    return binaryMakeStatusResponse(decoded.envelope, SG_BINARY_STATUS_DECODE_FAIL);
  }

  Phase1InfoQueryResponse response = Phase1InfoQueryResponse_init_default;
  strncpy(response.version, snapshot.version.c_str(), sizeof(response.version) - 1);
  strncpy(response.model, snapshot.model.c_str(), sizeof(response.model) - 1);
  response.device_environment = snapshot.deviceEnvironment;

  std::vector<uint8_t> responsePayload = binaryEncodeInfoQueryResponsePayload(response);
  if (responsePayload.size() > SG_BINARY_MAX_PAYLOAD_SIZE) {
    return binaryMakeStatusResponse(decoded.envelope, SG_BINARY_STATUS_INTERNAL_ERROR);
  }

  BinaryEnvelope responseEnvelope = binaryMakeEnvelope(
      SG_BINARY_PROTOCOL_VERSION,
      SG_BINARY_MESSAGE_TYPE_RESPONSE,
      decoded.envelope.operationId,
      decoded.envelope.requestId,
      SG_BINARY_STATUS_OK,
      responsePayload.size());
  BinaryRouterResult result = {
      SG_BINARY_STATUS_OK,
      binaryFrameEncode(responseEnvelope, responsePayload),
  };
  return result;
}

#ifdef ARDUINO

#include <ArduinoJson.h>

#include "combineContext.h"
#include "domain/info/resolvers.h"

static inline Phase1Info_DeviceEnvironment binaryResolveDeviceEnvironment(const String &deviceEnv) {
  if (deviceEnv == "development") {
    return Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT;
  }
  return Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_PRODUCTION;
}

static inline BinaryInfoSnapshot binaryInfoSnapshotFromContext(CombineContext *context) {
  query_info queryInfo;
  DynamicJsonDocument requestData(16);
  JsonObject reqData = requestData.to<JsonObject>();
  DynamicJsonDocument infoData = queryInfo.resolve(reqData, context);
  return binaryMakeInfoSnapshot(
      infoData["version"] | "",
      infoData["model"] | "",
      binaryResolveDeviceEnvironment(infoData["device_env"] | "prod"));
}

static inline BinaryRouterResult binaryRouteFrame(
    const std::vector<uint8_t> &requestFrame,
    CombineContext *context) {
  return binaryRouteFrame(requestFrame, binaryInfoSnapshotFromContext(context));
}

#endif

#endif // SG_MCU_BINARY_ROUTER_H
