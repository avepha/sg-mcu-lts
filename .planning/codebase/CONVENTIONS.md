# Coding Conventions

**Analysis Date:** 2026-04-15

## Naming Patterns

**Files:**
- Use lowercase or mixed-case header names grouped by role, with domain folders mirroring firmware concepts such as `src/domain/channel/resolvers.h`, `src/domain/station/permission.h`, `src/domain/sequence-control/sequence-par/model.h`, and `src/logger/log.h`.
- Use paired role filenames repeatedly inside a domain: `context.h`, `model.h`, `core.h`, `permission.h`, and `resolvers.h` under paths like `src/domain/station/` and `src/domain/channel/`.
- Use utility filenames that describe a single helper or mapping, such as `src/util/JsonRequest.h`, `src/domain/precondition/util/resolvePreconditionEnum.h`, and `src/domain/station/util/resolveStationTypeEnum.h`.

**Functions:**
- Use `camelCase` for free functions and methods: `bootstrap()` in `src/bootstrap.h`, `initModel()` in `src/initModel.h`, `updateLogLevel()` in `src/logger/log.h`, and `resolve()` / `exec()` in `src/domain/resolvers.h`.
- Use descriptive test helper names in `test/`, for example `rs485BasicCommand()` in `test/system/rs485.h`.

**Variables:**
- Use `camelCase` for locals, parameters, and members: `requestString`, `responseJson`, `notificationFrame`, and `currentNotificationIndex` in `src/main.cpp`.
- Prefix some temporary JSON variables with short type hints such as `jarrPreconditions` and `joPrecondition` in `src/domain/channel/resolvers.h`.
- Pointer members often use noun names without ownership markers, such as `context`, `resolvers`, `loggerTray`, and `notificationTray` in `src/bootstrap.h` and `src/main.cpp`.

**Types:**
- Use `PascalCase` for classes and structs: `CombineContext` in `src/combineContext.h`, `ValidationError` in `src/validationError.h`, `LoggerTray` in `src/logger/log.h`, and `ModbusPacket` in `src/domain/station/modbus/modbusPacket.h`.
- Use `snake_case` class names for endpoint resolver types, matching API topic names, such as `query_channel`, `mutation_channel_save`, and `query_station_data` in `src/domain/channel/resolvers.h` and `src/combineResolvers.h`.
- Use `UPPER_SNAKE_CASE` for macros and compile flags, such as `SG_VERSION_V2`, `SG_MODE_DEVELOPMENT`, `RS485_DIR_PIN`, and header guards like `SG_MCU_CHANNEL_RESOLVERS_H` in `src/config.h` and `src/domain/channel/resolvers.h`.

## Code Style

**Formatting:**
- No formatter config is checked in. No `.clang-format`, `.editorconfig`, `eslint`, or `prettier` config was detected at the repository root.
- Follow the repository-local style from `AGENTS.md` and nearby files: 2-space indentation, braces on the same line, and compact inline comments.
- Keep implementation inline in headers for most domain code. Files such as `src/domain/channel/resolvers.h`, `src/logger/log.h`, and `src/domain/resolvers.h` define class methods directly in `.h` files.

**Linting:**
- No linter configuration was detected.
- Consistency is maintained manually by matching neighboring files, especially under `src/domain/**`.

## Import Organization

**Order:**
1. Standard or framework headers, for example `<Arduino.h>`, `<Wire.h>`, and `<ArduinoJson.h>` in `src/main.cpp`.
2. Project headers using relative paths such as `./config.h`, `./bootstrap.h`, and `./logger/log.h` in `src/main.cpp`.
3. Domain-specific headers grouped by feature, such as `domain/configuration/resolvers.h`, `domain/channel/resolvers.h`, and `domain/station/resolvers.h` in `src/combineResolvers.h`.

**Path Aliases:**
- No path alias system is configured.
- Includes use relative paths like `./permission.h` in `src/domain/channel/resolvers.h` and project-root-relative includes like `domain/permission.h` in `src/domain/station/permission.h`.

## Error Handling

**Patterns:**
- Represent request and validation failures as typed error objects from `src/validationError.h`, such as `InvalidInputError`, `TopicNotFoundError`, and `StationNotFoundError`.
- Perform request validation in `permission` classes before resolver logic. `src/domain/resolvers.h` calls `permission->resolve(reqData, context)` inside `exec()`, and files like `src/domain/station/permission.h` throw `InvalidInputError` when fields are missing or invalid.
- Catch `ValidationError` at the router level in `src/combineResolvers.h` and convert it to JSON with `err.toJson()`. New request handlers should throw `ValidationError` subclasses instead of returning ad hoc error payloads.
- Validate outer request shape early in `src/main.cpp` by checking JSON parsing and required keys (`topic`, `method`, `reqId`) before dispatching.

## Logging

**Framework:** custom logger queue in `src/logger/log.h`

**Patterns:**
- Use `Log::fatal`, `Log::error`, `Log::warn`, `Log::info`, `Log::debug`, or `Log::trace` with a topic string and message string, as in `src/main.cpp` and `src/domain/station/modbus/modbusTask.h`.
- Queue logs through `LoggerTray` and flush them to endpoints from `src/main.cpp` instead of printing directly from feature code.
- Use raw `Serial.println` mainly for boot diagnostics in `src/bootstrap.h` and hardware-oriented debug output in tests such as `test/system/rs485.h`.

## Comments

**When to Comment:**
- Use short inline comments for hardware assumptions, sequencing, or non-obvious branches, such as `// init function` and `// for laptop` in `src/main.cpp`, `// TODO: pin 2 is reserved pin for activating uploading mode` in `src/config.h`, and protocol notes in `src/domain/station/modbus/modbusPacket.h`.
- Avoid long narrative comments. Most files rely on names and structure rather than prose.

**JSDoc/TSDoc:**
- Not used.
- Special marker comments appear for resolver grouping, such as `// @query` in `src/domain/channel/resolvers.h` and `src/domain/sequence/resolvers.h`.

## Function Design

**Size:**
- Keep small abstractions for shared base behavior, such as `Query` in `src/domain/query.h`, `Mutation` in `src/domain/mutation.h`, and `Resolvers` in `src/domain/resolvers.h`.
- Accept larger procedural functions for device loops and endpoint handlers. `setup()` / `loop1()` in `src/main.cpp` and `CombineQuery()` / `CombineMutation()` in `src/combineResolvers.h` are long but centralized.

**Parameters:**
- Pass `JsonObject reqData` and `CombineContext *context` to request handlers, as shown throughout `src/domain/**/resolvers.h`.
- Use explicit constructors for single-argument classes where applied, for example `explicit query_channel()` in `src/domain/channel/resolvers.h` and `explicit InvalidInputError(...)` in `src/validationError.h`.

**Return Values:**
- Return `DynamicJsonDocument` from resolver methods and error serializers, as in `src/domain/channel/resolvers.h`, `src/combineResolvers.h`, and `src/validationError.h`.
- Return empty or default JSON documents from abstract/base implementations only, such as `Resolvers::resolve()` in `src/domain/resolvers.h`.

## Module Design

**Exports:**
- Define classes and functions directly in headers protected by include guards. This is the default module pattern across `src/`.
- Group domain logic by concern under one folder: permissions in `permission.h`, state wiring in `context.h`, persistence in `model.h`, runtime logic in `core.h`, and API handlers in `resolvers.h`.

**Barrel Files:**
- Not used as standalone index files.
- Central aggregation happens through `src/combineContext.h` and `src/combineResolvers.h`, which wire all domain modules together.

## Configuration and Maintenance Conventions

- Keep board environments and compile-time feature flags in `platformio.ini`; use environment-specific `build_flags` like `-DSG_VERSION_V2` and `-DSG_MODE_DEVELOPMENT` rather than hardcoding behavior in source.
- Gate hardware or feature variants with compile-time flags in source, as seen in `src/config.h`, `src/bootstrap.h`, `src/main.cpp`, and `src/domain/station/core.h`.
- Keep hardware constants centralized in `src/config.h` and boot/runtime initialization in `src/bootstrap.h`.
- Keep build artifacts out of version control; `.gitignore` excludes `.pio`, `.pioenvs`, `.piolibdeps`, and `build`.
- When adding new API behavior, mirror the existing pattern: add permission checks and resolver classes in a domain file such as `src/domain/channel/resolvers.h`, register them in `src/combineResolvers.h`, and keep example request JSON in `docs/` as described in `README.md`.

---

*Convention analysis: 2026-04-15*
