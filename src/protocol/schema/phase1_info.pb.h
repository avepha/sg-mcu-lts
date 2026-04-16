#ifndef SG_MCU_PHASE1_INFO_PB_H
#define SG_MCU_PHASE1_INFO_PB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _Phase1Info_DeviceEnvironment {
  Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_UNSPECIFIED = 0,
  Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_PRODUCTION = 1,
  Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_DEVELOPMENT = 2,
} Phase1Info_DeviceEnvironment;

typedef struct _Phase1InfoQueryRequest {
  bool has_client_capabilities;
  uint32_t client_capabilities;
} Phase1InfoQueryRequest;

typedef struct _Phase1InfoQueryResponse {
  char version[33];
  char model[33];
  Phase1Info_DeviceEnvironment device_environment;
} Phase1InfoQueryResponse;

static const Phase1InfoQueryRequest Phase1InfoQueryRequest_init_default = {
    false,
    0,
};

static const Phase1InfoQueryResponse Phase1InfoQueryResponse_init_default = {
    "",
    "",
    Phase1Info_DeviceEnvironment_DEVICE_ENVIRONMENT_UNSPECIFIED,
};

#ifdef __cplusplus
}
#endif

#endif // SG_MCU_PHASE1_INFO_PB_H
