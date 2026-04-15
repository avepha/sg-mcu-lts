# Repository Guidelines

## Project Structure & Module Organization
This repository is an ESP32 firmware project built with PlatformIO and Arduino. Application code lives in `src/`, with most features grouped by domain under paths such as `src/domain/channel-control/`, `src/domain/sequence-control/`, and `src/domain/station/`. Shared bootstrapping and device entrypoints are in files like `src/main.cpp`, `src/bootstrap.h`, and `src/*Endpoint.h`.

Public headers and library placeholders live in `include/` and `lib/`. Protocol examples and request payload fixtures are stored in `docs/` as JSON files. Tests live under `test/`, with the current Unity-based MCU test entrypoint in `test/test_main.cpp`.

## Build, Test, and Development Commands
Use the local `rtk` wrapper for shell commands in this workspace.

- `rtk pio run -e v2_dev`: build the default development firmware for ESP32.
- `rtk pio run -e v2`: build the LTS production variant without development flags.
- `rtk pio test`: run PlatformIO Unity tests from `test/`.
- `rtk pio device monitor -b 345600`: open the serial monitor with the configured baud rate.

Run commands from the repository root. Do not commit `.pio/` build artifacts.

## Coding Style & Naming Conventions
Follow the existing C++ style: 2-space indentation, opening braces on the same line, and short inline comments only where logic is not obvious. Keep feature code scoped to its domain directory instead of adding cross-cutting utilities to unrelated modules.

Match current naming patterns:
- Types and classes: `PascalCase`
- Functions and variables: `camelCase`
- Macros and compile flags: `UPPER_SNAKE_CASE`
- Test helpers and fixtures: descriptive names such as `rs485BasicCommand`

No formatter or linter is checked in, so consistency with nearby files is the standard.

## Testing Guidelines
Tests use PlatformIO's Unity support. Add new MCU tests under `test/` and register them in `test/test_main.cpp` with `RUN_TEST(...)`. Prefer small hardware-focused tests that validate packet parsing, transport behavior, and domain rules independently where possible.

## Commit & Pull Request Guidelines
Recent history follows short, imperative messages with conventional prefixes when useful, for example `fix(espressif): ...`, `fix: ...`, and `chore: ...`. Keep commits focused on one change.

Pull requests should describe the target environment (`v1`, `v2`, `v2_dev`, etc.), summarize firmware behavior changes, list test/build commands run, and include serial logs or payload examples when the change affects device I/O or protocol handling.

<!-- GSD:project-start source:PROJECT.md -->
## Project

**SG MCU LTS Protocol Rework**

This project is an ESP32 firmware codebase for a controller device that currently exposes a JSON request/response protocol over its serial transport boundary. The next project milestone is to design and implement a more efficient custom binary protocol for communication with a new in-house backend, starting with one end-to-end request/response operation and expanding from there.

**Core Value:** The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.

### Constraints

- **Tech stack**: Must fit the current ESP32 firmware environment in `platformio.ini` and the existing C++/Arduino codebase under `src/` — the protocol design must work within embedded runtime and memory constraints
- **Brownfield architecture**: The new protocol should integrate with the existing request routing and domain model structure centered in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h` — the firmware is not being rewritten from scratch
- **Initial milestone**: First release only needs at least one production-relevant request/response operation implemented end to end — this is a proving milestone, not a full replacement
- **Interoperability**: The protocol should be designed with a schema definition approach that can later support generated types for Rust, Go, and TypeScript — even if code generation is deferred initially
- **Compatibility**: No backward compatibility requirement with the old JSON backend — this allows protocol and backend decisions to optimize for the new design
<!-- GSD:project-end -->

<!-- GSD:stack-start source:codebase/STACK.md -->
## Technology Stack

## Languages
- C++ (Arduino-style embedded C++) - Main firmware implementation in `src/main.cpp`, `src/bootstrap.h`, and domain modules under `src/domain/**`
- JSON - Request/response payload fixtures and protocol examples in `docs/**/*.json`
- INI - PlatformIO build and environment configuration in `platformio.ini`
- Markdown - Developer documentation in `README.md` and `AGENTS.md`
## Runtime
- ESP32 microcontroller firmware targeting Espressif32 platform in `platformio.ini`
- Arduino framework on ESP32 in `platformio.ini`
- FreeRTOS-backed tasking via ESP32 Arduino APIs such as `xTaskCreatePinnedToCore(...)` in `src/main.cpp`
- PlatformIO Library Manager via `lib_deps` in `platformio.ini`
- Lockfile: missing
## Frameworks
- Arduino framework - Device runtime, GPIO, serial, EEPROM, timing, and ESP restart APIs used in `src/main.cpp`, `src/bootstrap.h`, and `src/domain/**`
- PlatformIO - Build, upload, dependency resolution, and test orchestration configured in `platformio.ini`
- Unity via PlatformIO test runner - MCU tests bootstrapped from `test/test_main.cpp`
- CMake integration for IDEs generated by PlatformIO in `CMakeLists.txt`
- `rtk` wrapper is the expected local command prefix per `AGENTS.md` and `README.md`
## Key Dependencies
- `TaskScheduler@3` - Schedules background control, GPIO, RTC, and Modbus polling tasks in `src/bootstrap.h`, `src/domain/station/modbus/modbusTask.h`, `src/domain/rtc/util/RtcTask.h`, and multiple `src/domain/**/core.h` files
- `ArduinoJson@6.21.5` - Implements the device JSON API request/response contract in `src/main.cpp`, `src/combineResolvers.h`, `src/validationError.h`, and resolver files under `src/domain/**/resolvers.h`
- `RTClib@2.1.1` - Drives RTC access and software fallback timekeeping in `src/domain/rtc/core.h`, `src/domain/rtc/util/RtcTask.h`, and `src/domain/notification/gpioNotification.h`
- `FastCRC` (library id `6497` in `platformio.ini`) - Computes and validates Modbus CRC16 frames in `src/domain/station/modbus/modbusPacket.h`
- `EEPROM.h` from Arduino/ESP32 core - Persists firmware schemas and initialization state in `src/bootstrap.h`, `src/domain/model.h`, and `src/domain/low-lv-command/resolvers.h`
- `Wire.h` / I2C support from Arduino core - Enabled by inclusion in `src/main.cpp` and used by RTC integration via `RTClib`
- `HardwareSerial` from Arduino core - Provides USB serial, entry serial, and station serial transports in `src/bootstrap.h`, `src/deviceEndpoint.h`, and `src/loraEndpoint.h`
## Configuration
- Firmware variants are selected with PlatformIO environments in `platformio.ini`: `v1`, `v1_simplex`, `v1_dev`, `v2`, `v2_simplex`, `v2_dev`, and `v2_uno`
- Compile-time behavior is driven by build flags in `platformio.ini`, especially `SG_VERSION_V1`, `SG_VERSION_V2`, `SG_COMMUNICATION_SIMPLEX`, and `SG_MODE_DEVELOPMENT`
- Device pins, baud rates, model names, and RS485 direction settings are hard-coded in `src/config.h`
- Boot constants such as firmware version, project name, EEPROM size, and serial port bindings are defined in `src/bootstrap.h`
- Primary build config: `platformio.ini`
- IDE/CMake bridge: `CMakeLists.txt`
- Project cleanup exclusions: `.gitignore` excludes `.pio/`, `.pioenvs`, `.piolibdeps`, and `build`
## Platform Requirements
- PlatformIO Core 6.x per `README.md`
- ESP32 board compatible with `board = node32s` in `platformio.ini`
- USB serial access for `Serial` monitor and uploads described in `README.md`
- Python environment capable of running `pio` per `README.md`
- Deployment target is an ESP32 Smart-Farm controller firmware image built from `platformio.ini` environments such as `v2`
- Runtime expects attached serial peripherals on `Serial1` and `Serial2`, plus an RTC and RS485-connected stations as configured in `src/bootstrap.h` and `src/config.h`
<!-- GSD:stack-end -->

<!-- GSD:conventions-start source:CONVENTIONS.md -->
## Conventions

## Naming Patterns
- Use lowercase or mixed-case header names grouped by role, with domain folders mirroring firmware concepts such as `src/domain/channel/resolvers.h`, `src/domain/station/permission.h`, `src/domain/sequence-control/sequence-par/model.h`, and `src/logger/log.h`.
- Use paired role filenames repeatedly inside a domain: `context.h`, `model.h`, `core.h`, `permission.h`, and `resolvers.h` under paths like `src/domain/station/` and `src/domain/channel/`.
- Use utility filenames that describe a single helper or mapping, such as `src/util/JsonRequest.h`, `src/domain/precondition/util/resolvePreconditionEnum.h`, and `src/domain/station/util/resolveStationTypeEnum.h`.
- Use `camelCase` for free functions and methods: `bootstrap()` in `src/bootstrap.h`, `initModel()` in `src/initModel.h`, `updateLogLevel()` in `src/logger/log.h`, and `resolve()` / `exec()` in `src/domain/resolvers.h`.
- Use descriptive test helper names in `test/`, for example `rs485BasicCommand()` in `test/system/rs485.h`.
- Use `camelCase` for locals, parameters, and members: `requestString`, `responseJson`, `notificationFrame`, and `currentNotificationIndex` in `src/main.cpp`.
- Prefix some temporary JSON variables with short type hints such as `jarrPreconditions` and `joPrecondition` in `src/domain/channel/resolvers.h`.
- Pointer members often use noun names without ownership markers, such as `context`, `resolvers`, `loggerTray`, and `notificationTray` in `src/bootstrap.h` and `src/main.cpp`.
- Use `PascalCase` for classes and structs: `CombineContext` in `src/combineContext.h`, `ValidationError` in `src/validationError.h`, `LoggerTray` in `src/logger/log.h`, and `ModbusPacket` in `src/domain/station/modbus/modbusPacket.h`.
- Use `snake_case` class names for endpoint resolver types, matching API topic names, such as `query_channel`, `mutation_channel_save`, and `query_station_data` in `src/domain/channel/resolvers.h` and `src/combineResolvers.h`.
- Use `UPPER_SNAKE_CASE` for macros and compile flags, such as `SG_VERSION_V2`, `SG_MODE_DEVELOPMENT`, `RS485_DIR_PIN`, and header guards like `SG_MCU_CHANNEL_RESOLVERS_H` in `src/config.h` and `src/domain/channel/resolvers.h`.
## Code Style
- No formatter config is checked in. No `.clang-format`, `.editorconfig`, `eslint`, or `prettier` config was detected at the repository root.
- Follow the repository-local style from `AGENTS.md` and nearby files: 2-space indentation, braces on the same line, and compact inline comments.
- Keep implementation inline in headers for most domain code. Files such as `src/domain/channel/resolvers.h`, `src/logger/log.h`, and `src/domain/resolvers.h` define class methods directly in `.h` files.
- No linter configuration was detected.
- Consistency is maintained manually by matching neighboring files, especially under `src/domain/**`.
## Import Organization
- No path alias system is configured.
- Includes use relative paths like `./permission.h` in `src/domain/channel/resolvers.h` and project-root-relative includes like `domain/permission.h` in `src/domain/station/permission.h`.
## Error Handling
- Represent request and validation failures as typed error objects from `src/validationError.h`, such as `InvalidInputError`, `TopicNotFoundError`, and `StationNotFoundError`.
- Perform request validation in `permission` classes before resolver logic. `src/domain/resolvers.h` calls `permission->resolve(reqData, context)` inside `exec()`, and files like `src/domain/station/permission.h` throw `InvalidInputError` when fields are missing or invalid.
- Catch `ValidationError` at the router level in `src/combineResolvers.h` and convert it to JSON with `err.toJson()`. New request handlers should throw `ValidationError` subclasses instead of returning ad hoc error payloads.
- Validate outer request shape early in `src/main.cpp` by checking JSON parsing and required keys (`topic`, `method`, `reqId`) before dispatching.
## Logging
- Use `Log::fatal`, `Log::error`, `Log::warn`, `Log::info`, `Log::debug`, or `Log::trace` with a topic string and message string, as in `src/main.cpp` and `src/domain/station/modbus/modbusTask.h`.
- Queue logs through `LoggerTray` and flush them to endpoints from `src/main.cpp` instead of printing directly from feature code.
- Use raw `Serial.println` mainly for boot diagnostics in `src/bootstrap.h` and hardware-oriented debug output in tests such as `test/system/rs485.h`.
## Comments
- Use short inline comments for hardware assumptions, sequencing, or non-obvious branches, such as `// init function` and `// for laptop` in `src/main.cpp`, `// TODO: pin 2 is reserved pin for activating uploading mode` in `src/config.h`, and protocol notes in `src/domain/station/modbus/modbusPacket.h`.
- Avoid long narrative comments. Most files rely on names and structure rather than prose.
- Not used.
- Special marker comments appear for resolver grouping, such as `// @query` in `src/domain/channel/resolvers.h` and `src/domain/sequence/resolvers.h`.
## Function Design
- Keep small abstractions for shared base behavior, such as `Query` in `src/domain/query.h`, `Mutation` in `src/domain/mutation.h`, and `Resolvers` in `src/domain/resolvers.h`.
- Accept larger procedural functions for device loops and endpoint handlers. `setup()` / `loop1()` in `src/main.cpp` and `CombineQuery()` / `CombineMutation()` in `src/combineResolvers.h` are long but centralized.
- Pass `JsonObject reqData` and `CombineContext *context` to request handlers, as shown throughout `src/domain/**/resolvers.h`.
- Use explicit constructors for single-argument classes where applied, for example `explicit query_channel()` in `src/domain/channel/resolvers.h` and `explicit InvalidInputError(...)` in `src/validationError.h`.
- Return `DynamicJsonDocument` from resolver methods and error serializers, as in `src/domain/channel/resolvers.h`, `src/combineResolvers.h`, and `src/validationError.h`.
- Return empty or default JSON documents from abstract/base implementations only, such as `Resolvers::resolve()` in `src/domain/resolvers.h`.
## Module Design
- Define classes and functions directly in headers protected by include guards. This is the default module pattern across `src/`.
- Group domain logic by concern under one folder: permissions in `permission.h`, state wiring in `context.h`, persistence in `model.h`, runtime logic in `core.h`, and API handlers in `resolvers.h`.
- Not used as standalone index files.
- Central aggregation happens through `src/combineContext.h` and `src/combineResolvers.h`, which wire all domain modules together.
## Configuration and Maintenance Conventions
- Keep board environments and compile-time feature flags in `platformio.ini`; use environment-specific `build_flags` like `-DSG_VERSION_V2` and `-DSG_MODE_DEVELOPMENT` rather than hardcoding behavior in source.
- Gate hardware or feature variants with compile-time flags in source, as seen in `src/config.h`, `src/bootstrap.h`, `src/main.cpp`, and `src/domain/station/core.h`.
- Keep hardware constants centralized in `src/config.h` and boot/runtime initialization in `src/bootstrap.h`.
- Keep build artifacts out of version control; `.gitignore` excludes `.pio`, `.pioenvs`, `.piolibdeps`, and `build`.
- When adding new API behavior, mirror the existing pattern: add permission checks and resolver classes in a domain file such as `src/domain/channel/resolvers.h`, register them in `src/combineResolvers.h`, and keep example request JSON in `docs/` as described in `README.md`.
<!-- GSD:conventions-end -->

<!-- GSD:architecture-start source:ARCHITECTURE.md -->
## Architecture

## Pattern Overview
- `src/main.cpp` owns boot, endpoint wiring, request handling, and scheduler execution.
- `src/combineContext.h` builds a single shared object graph of domain contexts.
- `src/combineResolvers.h` registers topic-based query and mutation handlers that dispatch into domain modules.
## Layers
- Purpose: Initialize hardware, serial ports, schedulers, endpoints, global trays, and domain wiring.
- Location: `src/main.cpp`, `src/bootstrap.h`, `src/initModel.h`
- Contains: `setup()`, `loop()`, `loop1()`, serial boot config, scheduler globals, initial model bootstrapping.
- Depends on: `src/config.h`, `src/deviceEndpoint.h`, `src/loraEndpoint.h`, `src/combineContext.h`, `src/combineResolvers.h`.
- Used by: PlatformIO firmware entrypoint and Unity test bootstrap in `test/test_main.cpp`.
- Purpose: Convert serial traffic into request payloads and send responses/events/logs back out.
- Location: `src/deviceEndpoint.h`, `src/loraEndpoint.h`
- Contains: line-delimited JSON framing for `Serial`/`Serial1`, byte-packet framing for `Serial2`.
- Depends on: `HardwareSerial`, `src/logger/log.h`, station packet verification in `src/domain/station/modbus/modbusPacket.h`.
- Used by: `src/main.cpp`.
- Purpose: Map `topic` + `method` to concrete handlers and normalize success/error responses.
- Location: `src/combineResolvers.h`, `src/domain/resolvers.h`, `src/domain/query.h`, `src/domain/mutation.h`
- Contains: query map, mutation map, permission hooks, response envelope creation.
- Depends on: `src/combineContext.h`, per-domain `resolvers.h`, `src/validationError.h`.
- Used by: `src/main.cpp` request execution path.
- Purpose: Construct and expose domain services through one aggregate runtime context.
- Location: `src/combineContext.h`, plus per-domain `context.h` files such as `src/domain/channel/context.h`, `src/domain/sequence/context.h`, `src/domain/station/context.h`.
- Contains: pointers to models, cores, and singleton-backed services.
- Depends on: each domain's `context.h`.
- Used by: every resolver through `CombineContext *context`.
- Purpose: Persist controller state and configuration in EEPROM with fixed schema slots.
- Location: `src/domain/model.h`, per-domain model files like `src/domain/channel/model.h`, `src/domain/sequence/model.h`, `src/domain/station/model.h`, `src/domain/configuration/model.h`.
- Contains: `RomOrganizer`, templated `RomModel<T>`, schema structs, EEPROM read/write logic.
- Depends on: `EEPROM`, schema definitions.
- Used by: domain cores, resolvers, and startup code in `src/initModel.h`.
- Purpose: Run channel, sequence, GPIO, RTC, and station polling behavior over `TaskScheduler`.
- Location: `src/domain/channel-control/control.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/gpio/core.h`, `src/domain/rtc/util/RtcTask.h`, `src/domain/station/modbus/modbusTask.h`
- Contains: task subclasses, task factories, GPIO scheduling, Modbus polling, RTC synchronization.
- Depends on: scheduler globals from `src/bootstrap.h`, model/config data, hardware APIs.
- Used by: channel/sequence/station/rtc cores and indirectly by mutations that reactivate control paths.
## Data Flow
- Persistent state lives in EEPROM schemas managed by `RomModel<T>` in `src/domain/model.h`.
- Runtime service references live in `CombineContext` from `src/combineContext.h`.
- Long-running behavior is held in singleton cores such as `ChannelCore`, `SequenceCore`, `GpioCore`, `RtcCore`, and `StationCore`.
- Transient outbound messages are buffered in trays managed by `src/logger/log.h`, `src/logger/event.h`, and `src/domain/notification/notificationManager.h`.
## Key Abstractions
- Purpose: Aggregate all domain contexts into one dependency root.
- Examples: `src/combineContext.h`, `src/domain/channel/context.h`, `src/domain/station/context.h`
- Pattern: Manual service locator / composition root.
- Purpose: Standardize topic handlers with optional permission checks.
- Examples: `src/domain/resolvers.h`, `src/domain/query.h`, `src/domain/mutation.h`, `src/domain/channel/resolvers.h`
- Pattern: Command-style dispatch with string-key registration in `src/combineResolvers.h`.
- Purpose: Persist strongly typed schemas to EEPROM with automatic address assignment.
- Examples: `src/domain/model.h`, `src/domain/channel/model.h`, `src/domain/sequence/model.h`, `src/domain/configuration/model.h`
- Pattern: Templated repository over fixed memory slots.
- Purpose: Encapsulate runtime behavior and hardware-facing business logic.
- Examples: `src/domain/channel/core.h`, `src/domain/sequence/core.h`, `src/domain/control/core.h`, `src/domain/station/core.h`, `src/domain/rtc/core.h`
- Pattern: Singleton-heavy service layer.
- Purpose: Represent active control loops as scheduler tasks.
- Examples: `src/domain/channel-control/control.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/gpio/util/gpioTask.h`, `src/domain/station/modbus/modbusTask.h`
- Pattern: Inheritance from `TaskScheduler::Task` with factory selection.
## Entry Points
- Location: `src/main.cpp`
- Triggers: MCU startup.
- Responsibilities: Call `bootstrap()`, create endpoints and context, register resolvers, initialize persistent models, spawn `loop1()`, and execute schedulers in `loop()`.
- Location: `src/bootstrap.h`
- Triggers: `setup()` in `src/main.cpp` and test startup in `test/test_main.cpp`.
- Responsibilities: Initialize EEPROM, pins, serial ports, boot counters, scheduler globals, and hardware serial aliases.
- Location: `src/combineResolvers.h`
- Triggers: `resolvers->execute(requestJson)` from `src/main.cpp`.
- Responsibilities: Look up topic handlers, run permission validation, catch `ValidationError`, and shape response documents.
- Location: `test/test_main.cpp`
- Triggers: `pio test`.
- Responsibilities: Reuse `bootstrap()` and run Unity tests.
## Error Handling
- Request validation errors are represented by `ValidationError` subclasses in `src/validationError.h` and caught centrally in `src/combineResolvers.h`.
- Transport parsing faults are logged in `src/deviceEndpoint.h` and converted to error responses in `src/main.cpp`.
- Background failures such as Modbus CRC/timeouts are logged inline in `src/domain/station/modbus/modbusTask.h` instead of surfacing through exceptions.
## Cross-Cutting Concerns
<!-- GSD:architecture-end -->

<!-- GSD:skills-start source:skills/ -->
## Project Skills

No project skills found. Add skills to any of: `.claude/skills/`, `.agents/skills/`, `.cursor/skills/`, or `.github/skills/` with a `SKILL.md` index file.
<!-- GSD:skills-end -->

<!-- GSD:workflow-start source:GSD defaults -->
## GSD Workflow Enforcement

Before using Edit, Write, or other file-changing tools, start work through a GSD command so planning artifacts and execution context stay in sync.

Use these entry points:
- `/gsd-quick` for small fixes, doc updates, and ad-hoc tasks
- `/gsd-debug` for investigation and bug fixing
- `/gsd-execute-phase` for planned phase work

Do not make direct repo edits outside a GSD workflow unless the user explicitly asks to bypass it.
<!-- GSD:workflow-end -->

<!-- GSD:profile-start -->
## Developer Profile

> Profile not yet configured. Run `/gsd-profile-user` to generate your developer profile.
> This section is managed by `generate-claude-profile` -- do not edit manually.
<!-- GSD:profile-end -->
