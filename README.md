# sg-mcu-lts

ESP32 firmware for the Smart-Farm controller. The project is built with PlatformIO on top of Arduino and exposes a line-delimited JSON API over serial links for configuration, telemetry, station communication, scheduling, and control logic.

This README is aimed at getting a developer productive quickly: build the firmware, flash the right environment, inspect the serial output, and understand where to change behavior.

## What This Firmware Does

At runtime, the MCU:

- boots the ESP32 and initializes EEPROM-backed configuration
- exposes JSON request handling on `Serial` and `Serial1`
- communicates with stations on `Serial2`
- coordinates domain modules for channels, sequences, preconditions, sensors, RTC, notifications, configuration, and stations
- returns structured JSON responses, logs, events, and notifications

The request router is assembled in `src/combineResolvers.h`, and shared runtime state is wired in `src/combineContext.h`.

## Repository Layout

```text
src/
  main.cpp                  Main loop and request processing
  bootstrap.h               Hardware init, serial setup, boot log
  combineContext.h          Shared domain context
  combineResolvers.h        Query/mutation registration
  domain/                   Feature modules grouped by domain
  logger/                   Logging helpers
  util/                     Shared utilities
test/
  test_main.cpp             Unity test entrypoint
  system/                   Hardware-oriented test helpers
docs/
  */query/*.json            Request examples for query endpoints
  */mutation/*.json         Request examples for mutation endpoints
platformio.ini              Environments, board config, dependencies
AGENTS.md                   Contributor guide for this repository
```

## Prerequisites

- PlatformIO Core 6.x
- ESP32 development board compatible with `board = node32s`
- USB serial access to the controller
- Python environment capable of running `pio`

If you are working inside this Codex workspace, prefix shell commands with `rtk`, for example `rtk pio run -e v2_dev`. Outside this workspace, use plain `pio`.

## Quick Start

1. Install dependencies and build the development firmware:

```bash
pio run -e v2_dev
```

2. Flash the board:

```bash
pio run -e v2_dev -t upload --upload-port /dev/ttyUSB0
```

3. Open the serial monitor:

```bash
pio device monitor -b 345600 --port /dev/ttyUSB0
```

4. Confirm the boot banner includes values similar to:

```text
VERSION: 2.2.4
PROJECT: SMART-FARM
SG_MODEL: v2
SG_MODE: DEVELOPMENT
```

## Build Environments

Defined in `platformio.ini`:

- `v1`: SG 2019 firmware
- `v1_simplex`: SG 2019 with simplex communication
- `v1_dev`: SG 2019 development build
- `v2`: SG LTS production build
- `v2_simplex`: SG LTS with simplex communication
- `v2_dev`: default development build
- `v2_uno`: v2 with different upload speed

`v2_dev` is the default environment and the best starting point for local development.

## How the Device API Works

Requests are JSON objects sent as a single line ending in `\n`. Every request must include:

- `topic`
- `method` as either `query` or `mutation`
- `reqId`
- optional `data`

Example query:

```json
{
  "topic": "info",
  "method": "query",
  "reqId": "req-1"
}
```

Example mutation:

```json
{
  "topic": "channel_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "control": { "type": "timer", "value": 0 }
  },
  "reqId": "req-1"
}
```

See `docs/` for endpoint examples, including:

- `docs/info/query/info.json`
- `docs/configuration/query/configuration.json`
- `docs/channel-control/query/channel.json`
- `docs/channel-control/mutation/channel_save.json`
- `docs/station/query/stations.json`

Successful responses include `topic`, `method`, `execTime`, `data`, and the original `reqId`.

## Development Workflow

### Build

```bash
pio run -e v2_dev
```

This currently builds cleanly with the checked-in `platformio.ini`.

### Run Tests

```bash
pio test -e v2_dev
```

Tests use Unity through PlatformIO. The current test suite in `test/system/rs485.h` is hardware-oriented and may require attached station/RS485 hardware rather than a host-only environment.
It also performs an upload step, so expect `pio test -e v2_dev` to fail unless a target board and upload port are available.

### Clean Build Artifacts

```bash
pio run -t clean -e v2_dev
```

## Key Hardware and Serial Settings

From `src/config.h` and `src/bootstrap.h`:

- USB debug serial baud: `345600`
- RPi/entry serial baud: `345600`
- station serial baud: `9600`
- RS485 direction pin: GPIO `4`
- station RX/TX: GPIO `16` / `17`
- v2 MPU RX/TX: GPIO `19` / `18`
- v1 MPU RX/TX: GPIO `18` / `19`

Channel outputs are mapped in `CHANNEL_GPIO_MAP` in `src/config.h`.

## Where To Make Changes

- add or update request handlers in `src/domain/**/resolvers.h`
- extend shared state in `src/combineContext.h`
- register new queries and mutations in `src/combineResolvers.h`
- change boot or serial initialization in `src/bootstrap.h`
- adjust environment flags and dependencies in `platformio.ini`
- add API examples in `docs/`

When adding a new endpoint, update both the resolver and the matching example JSON under `docs/`.

## Troubleshooting

- Build dependency issues: remove `.pio/` and rebuild if PlatformIO has stale packages.
- Serial parsing issues: requests must start with `{` and end with a newline.
- Missing responses: check the serial baud rate and confirm the request includes `topic`, `method`, and `reqId`.
- Unexpected restarts on first boot: `initModel()` can trigger a restart after initializing presets in EEPROM.

## Next Files To Read

If you are new to the codebase, read these in order:

1. `platformio.ini`
2. `src/main.cpp`
3. `src/bootstrap.h`
4. `src/combineContext.h`
5. `src/combineResolvers.h`
6. one domain under `src/domain/` relevant to your change
