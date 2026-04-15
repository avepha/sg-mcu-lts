# External Integrations

**Analysis Date:** 2026-04-15

## APIs & External Services

**Host serial JSON API:**
- Line-delimited JSON request/response protocol - Main control surface used for configuration, telemetry, queries, mutations, logs, events, and notifications
  - SDK/Client: Custom ArduinoJson-based protocol implemented in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h`
  - Auth: Not applicable
  - Transport: `Serial` and `Serial1` initialized in `src/bootstrap.h`
  - Request examples: `docs/info/query/info.json`, `docs/channel-control/mutation/channel_save.json`, `docs/station/query/stations.json`, `docs/rtc/query/date.json`

**Station field bus:**
- Modbus-like binary station polling over RS485/UART - Used to query external station hardware and ingest sensor packets
  - SDK/Client: Custom packet and scheduler implementation in `src/domain/station/modbus/modbusPacket.h` and `src/domain/station/modbus/modbusTask.h`
  - Auth: Not applicable
  - CRC: `FastCRC` in `src/domain/station/modbus/modbusPacket.h`
  - Direction control: RS485 TX/RX switching via `RS485_DIR_PIN` in `src/config.h` and `sendDataToStation(...)` in `src/domain/station/modbus/modbusTask.h`

**Simplex/Lora mode transport:**
- Alternate station transport path enabled by `SG_COMMUNICATION_SIMPLEX` - The code names this `LoraEndpoint`, but it still reads bytes from `Serial2` and validates them as Modbus packets
  - SDK/Client: `src/loraEndpoint.h`
  - Auth: Not applicable
  - Enable flag: `SG_COMMUNICATION_SIMPLEX` from `platformio.ini`

## Data Storage

**Databases:**
- None

**Persistent device storage:**
- ESP32 EEPROM emulation
  - Connection: Not applicable
  - Client: Arduino `EEPROM` API in `src/bootstrap.h`, `src/domain/model.h`, and `src/domain/low-lv-command/resolvers.h`
  - Usage: Stores schema-backed model data, initialization marker, and write counters in `src/domain/model.h`

**File Storage:**
- Local source-controlled JSON fixtures only in `docs/**/*.json`
- No SPIFFS, SD, or cloud file storage detected in `src/**`

**Caching:**
- None detected

## Authentication & Identity

**Auth Provider:**
- None
  - Implementation: Requests on `Serial` and `Serial1` are accepted based on message shape only (`topic`, `method`, `reqId`) in `src/main.cpp`

## Monitoring & Observability

**Error Tracking:**
- None

**Logs:**
- In-process log queue with serial emission
  - Implementation: `src/logger/log.h`, `src/logger/event.h`, and `src/main.cpp`
  - Outbound format: JSON messages with `method = "log"` emitted on `Serial` and `Serial1` in `src/main.cpp`

**Events and notifications:**
- Device events are queued through `EventManager` in `src/logger/event.h` and emitted as JSON with `method = "event"` in `src/main.cpp`
- Operational notifications are queued through `NotificationManager` in `src/domain/notification/notificationManager.h` and emitted as JSON with `method = "notification"` in `src/main.cpp`
- Notification acknowledgment API example: `docs/notification/mutation/notification_ack.json`

## CI/CD & Deployment

**Hosting:**
- Physical ESP32 hardware flashed locally via PlatformIO per `README.md` and `platformio.ini`

**CI Pipeline:**
- Not detected

## Environment Configuration

**Required env vars:**
- None detected

**Secrets location:**
- No secret-bearing configuration files detected in the repository root

**Compile-time configuration:**
- PlatformIO environments and build flags in `platformio.ini`
- Hardware pins and baud rates in `src/config.h`
- Firmware identity constants in `src/bootstrap.h`

## Hardware & Device Integrations

**ESP32 board:**
- Target board `node32s` configured in `platformio.ini`
- Dual-core execution uses `xTaskCreatePinnedToCore(...)` in `src/main.cpp`

**Serial host links:**
- USB serial `Serial` for laptop/debug interaction in `src/main.cpp` and `src/bootstrap.h`
- Entry controller/RPi serial `Serial1` on `SG_MPU_RX` / `SG_MPU_TX` in `src/config.h` and `src/bootstrap.h`

**Station bus:**
- `Serial2` on `SG_STATION_RX` / `SG_STATION_TX` in `src/config.h` and `src/bootstrap.h`
- Station polling and parsing in `src/domain/station/modbus/modbusTask.h`
- Hardware RS485 smoke test in `test/system/rs485.h`

**RTC hardware:**
- DS1307 hardware clock through `RTClib` in `src/domain/rtc/core.h` and `src/domain/rtc/util/RtcTask.h`
- Software fallback clock via `RTC_Millis` in `src/domain/rtc/core.h`

**GPIO/channel outputs:**
- Channel-to-pin mapping in `src/config.h`
- Runtime control domains under `src/domain/channel/**`, `src/domain/channel-control/**`, and `src/domain/p_gpio/**`

**Station device types modeled in firmware:**
- GSensor, Solution, and GSolution packet generators in `src/domain/station/modbus/util/generateModbusResponse.h`
- Station request/response handling in `src/domain/station/station.h` and `src/domain/station/modbus/modbusTask.h`

## Webhooks & Callbacks

**Incoming:**
- None

**Outgoing:**
- None over HTTP
- Outgoing device callbacks are serial JSON streams (`log`, `event`, `notification`, and query/mutation responses) emitted from `src/main.cpp`

---

*Integration audit: 2026-04-15*
