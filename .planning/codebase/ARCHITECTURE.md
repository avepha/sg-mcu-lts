# Architecture

**Analysis Date:** 2026-04-15

## Pattern Overview

**Overall:** Domain-oriented embedded firmware with a central request router, EEPROM-backed models, and scheduler-driven runtime tasks.

**Key Characteristics:**
- `src/main.cpp` owns boot, endpoint wiring, request handling, and scheduler execution.
- `src/combineContext.h` builds a single shared object graph of domain contexts.
- `src/combineResolvers.h` registers topic-based query and mutation handlers that dispatch into domain modules.

## Layers

**Boot and runtime orchestration:**
- Purpose: Initialize hardware, serial ports, schedulers, endpoints, global trays, and domain wiring.
- Location: `src/main.cpp`, `src/bootstrap.h`, `src/initModel.h`
- Contains: `setup()`, `loop()`, `loop1()`, serial boot config, scheduler globals, initial model bootstrapping.
- Depends on: `src/config.h`, `src/deviceEndpoint.h`, `src/loraEndpoint.h`, `src/combineContext.h`, `src/combineResolvers.h`.
- Used by: PlatformIO firmware entrypoint and Unity test bootstrap in `test/test_main.cpp`.

**Transport and protocol boundary:**
- Purpose: Convert serial traffic into request payloads and send responses/events/logs back out.
- Location: `src/deviceEndpoint.h`, `src/loraEndpoint.h`
- Contains: line-delimited JSON framing for `Serial`/`Serial1`, byte-packet framing for `Serial2`.
- Depends on: `HardwareSerial`, `src/logger/log.h`, station packet verification in `src/domain/station/modbus/modbusPacket.h`.
- Used by: `src/main.cpp`.

**Request routing layer:**
- Purpose: Map `topic` + `method` to concrete handlers and normalize success/error responses.
- Location: `src/combineResolvers.h`, `src/domain/resolvers.h`, `src/domain/query.h`, `src/domain/mutation.h`
- Contains: query map, mutation map, permission hooks, response envelope creation.
- Depends on: `src/combineContext.h`, per-domain `resolvers.h`, `src/validationError.h`.
- Used by: `src/main.cpp` request execution path.

**Domain context layer:**
- Purpose: Construct and expose domain services through one aggregate runtime context.
- Location: `src/combineContext.h`, plus per-domain `context.h` files such as `src/domain/channel/context.h`, `src/domain/sequence/context.h`, `src/domain/station/context.h`.
- Contains: pointers to models, cores, and singleton-backed services.
- Depends on: each domain's `context.h`.
- Used by: every resolver through `CombineContext *context`.

**Persistence layer:**
- Purpose: Persist controller state and configuration in EEPROM with fixed schema slots.
- Location: `src/domain/model.h`, per-domain model files like `src/domain/channel/model.h`, `src/domain/sequence/model.h`, `src/domain/station/model.h`, `src/domain/configuration/model.h`.
- Contains: `RomOrganizer`, templated `RomModel<T>`, schema structs, EEPROM read/write logic.
- Depends on: `EEPROM`, schema definitions.
- Used by: domain cores, resolvers, and startup code in `src/initModel.h`.

**Control/task execution layer:**
- Purpose: Run channel, sequence, GPIO, RTC, and station polling behavior over `TaskScheduler`.
- Location: `src/domain/channel-control/control.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/gpio/core.h`, `src/domain/rtc/util/RtcTask.h`, `src/domain/station/modbus/modbusTask.h`
- Contains: task subclasses, task factories, GPIO scheduling, Modbus polling, RTC synchronization.
- Depends on: scheduler globals from `src/bootstrap.h`, model/config data, hardware APIs.
- Used by: channel/sequence/station/rtc cores and indirectly by mutations that reactivate control paths.

## Data Flow

**JSON API request flow:**

1. `src/main.cpp` reads a newline-terminated payload from `src/deviceEndpoint.h` on `Serial` or `Serial1`.
2. `src/main.cpp` validates `topic`, `method`, and `reqId`, then passes the JSON document to `CombineResolvers::execute()` in `src/combineResolvers.h`.
3. `src/domain/resolvers.h` optionally runs a permission object, then calls the resolver's `resolve()` implementation.
4. Domain resolvers read/write EEPROM-backed state through models in files such as `src/domain/channel/model.h` or trigger runtime behavior through cores such as `src/domain/channel/core.h`.
5. `src/main.cpp` wraps the result with `reqId`, serializes JSON, and sends the response back through `src/deviceEndpoint.h`.

**Channel activation flow:**

1. `mutation_channel_save` or `mutation_channel_activate` in `src/domain/channel/resolvers.h` updates `ChannelSchema` via `src/domain/channel/model.h`.
2. `src/domain/channel/core.h` reads the selected channel configuration.
3. `src/domain/channel-control/controlFactory.h` creates a concrete control task such as `TimerCore`, `CriteriaCore`, `RangeCore`, or `ParCore`.
4. `src/domain/precondition/preconditionFactory.h` attaches up to three preconditions to that control.
5. The resulting `Control` task in `src/domain/channel-control/control.h` schedules GPIO work through `src/domain/p_gpio/gpioTask.h` and `controlScheduler`/`gpioScheduler`.

**Sequence execution flow:**

1. Mutations in `src/domain/sequence/resolvers.h` save sequence metadata and channel order into `src/domain/sequence/model.h`.
2. `src/domain/sequence/core.h` creates a `SequenceGpioChain` from `src/domain/sequence/util/sequenceGpioChain.h`.
3. `src/domain/sequence-control/sequenceControlFactory.h` selects a concrete sequence controller.
4. Preconditions are attached with `src/domain/precondition/preconditionFactory.h`.
5. The controller drives a GPIO chain over scheduler tasks until deactivated.

**Station polling flow:**

1. `src/domain/station/core.h` reads station definitions from `src/domain/station/model.h` and registers them with `ModbusTask` in `src/domain/station/modbus/modbusTask.h`.
2. `ModbusTask` issues packets over `stationPort` from `src/bootstrap.h` and waits for responses.
3. Valid packets are routed to matching `Station` objects from `src/domain/station/station.h`.
4. Station query resolvers in `src/domain/station/resolvers.h` expose availability and sensor readings back to the JSON API.

**State Management:**
- Persistent state lives in EEPROM schemas managed by `RomModel<T>` in `src/domain/model.h`.
- Runtime service references live in `CombineContext` from `src/combineContext.h`.
- Long-running behavior is held in singleton cores such as `ChannelCore`, `SequenceCore`, `GpioCore`, `RtcCore`, and `StationCore`.
- Transient outbound messages are buffered in trays managed by `src/logger/log.h`, `src/logger/event.h`, and `src/domain/notification/notificationManager.h`.

## Key Abstractions

**CombineContext:**
- Purpose: Aggregate all domain contexts into one dependency root.
- Examples: `src/combineContext.h`, `src/domain/channel/context.h`, `src/domain/station/context.h`
- Pattern: Manual service locator / composition root.

**Resolvers / Query / Mutation:**
- Purpose: Standardize topic handlers with optional permission checks.
- Examples: `src/domain/resolvers.h`, `src/domain/query.h`, `src/domain/mutation.h`, `src/domain/channel/resolvers.h`
- Pattern: Command-style dispatch with string-key registration in `src/combineResolvers.h`.

**RomModel<T>:**
- Purpose: Persist strongly typed schemas to EEPROM with automatic address assignment.
- Examples: `src/domain/model.h`, `src/domain/channel/model.h`, `src/domain/sequence/model.h`, `src/domain/configuration/model.h`
- Pattern: Templated repository over fixed memory slots.

**Core classes:**
- Purpose: Encapsulate runtime behavior and hardware-facing business logic.
- Examples: `src/domain/channel/core.h`, `src/domain/sequence/core.h`, `src/domain/control/core.h`, `src/domain/station/core.h`, `src/domain/rtc/core.h`
- Pattern: Singleton-heavy service layer.

**Task-based controls:**
- Purpose: Represent active control loops as scheduler tasks.
- Examples: `src/domain/channel-control/control.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/gpio/util/gpioTask.h`, `src/domain/station/modbus/modbusTask.h`
- Pattern: Inheritance from `TaskScheduler::Task` with factory selection.

## Entry Points

**Firmware boot:**
- Location: `src/main.cpp`
- Triggers: MCU startup.
- Responsibilities: Call `bootstrap()`, create endpoints and context, register resolvers, initialize persistent models, spawn `loop1()`, and execute schedulers in `loop()`.

**Hardware/bootstrap setup:**
- Location: `src/bootstrap.h`
- Triggers: `setup()` in `src/main.cpp` and test startup in `test/test_main.cpp`.
- Responsibilities: Initialize EEPROM, pins, serial ports, boot counters, scheduler globals, and hardware serial aliases.

**Request dispatcher:**
- Location: `src/combineResolvers.h`
- Triggers: `resolvers->execute(requestJson)` from `src/main.cpp`.
- Responsibilities: Look up topic handlers, run permission validation, catch `ValidationError`, and shape response documents.

**Test boot entrypoint:**
- Location: `test/test_main.cpp`
- Triggers: `pio test`.
- Responsibilities: Reuse `bootstrap()` and run Unity tests.

## Error Handling

**Strategy:** Fail fast at the request boundary with JSON error documents, while runtime device tasks log faults and continue polling/executing.

**Patterns:**
- Request validation errors are represented by `ValidationError` subclasses in `src/validationError.h` and caught centrally in `src/combineResolvers.h`.
- Transport parsing faults are logged in `src/deviceEndpoint.h` and converted to error responses in `src/main.cpp`.
- Background failures such as Modbus CRC/timeouts are logged inline in `src/domain/station/modbus/modbusTask.h` instead of surfacing through exceptions.

## Cross-Cutting Concerns

**Logging:** Buffered logging is centralized through `src/logger/log.h` and emitted from `src/main.cpp` as `method = "log"` messages.

**Validation:** Input validation is resolver-local through permission classes declared beside domains, such as `src/domain/channel/permission.h`, `src/domain/sequence/permission.h`, and `src/domain/station/permission.h`.

**Authentication:** Not detected. The JSON serial API in `src/main.cpp` and `src/deviceEndpoint.h` accepts requests based on transport access rather than an auth layer.

---

*Architecture analysis: 2026-04-15*
