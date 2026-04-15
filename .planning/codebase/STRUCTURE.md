# Codebase Structure

**Analysis Date:** 2026-04-15

## Directory Layout

```text
sg-mcu-lts/
├── `src/`                 # Firmware source, runtime wiring, and domain modules
├── `src/domain/`          # Domain-oriented feature modules and shared abstractions
├── `src/logger/`          # Log/event queue helpers used by the runtime loop
├── `src/util/`            # Small shared utilities for JSON and conversions
├── `src/dev/`             # Development-only helpers for simplex simulation
├── `docs/`                # Example JSON payloads grouped by endpoint area
├── `test/`                # PlatformIO Unity tests and hardware-oriented helpers
├── `include/`             # Public include placeholder; currently only `README`
├── `lib/`                 # Local library placeholder; currently only `README`
├── `platformio.ini`       # Build environments, board config, dependencies
├── `README.md`            # Developer-oriented project overview and API notes
└── `.planning/codebase/`  # Generated architecture/reference documents
```

## Directory Purposes

**`src/`:**
- Purpose: Top-level firmware bootstrap and transport code.
- Contains: `src/main.cpp`, `src/bootstrap.h`, `src/config.h`, `src/combineContext.h`, `src/combineResolvers.h`, endpoint adapters.
- Key files: `src/main.cpp`, `src/bootstrap.h`, `src/deviceEndpoint.h`, `src/loraEndpoint.h`.

**`src/domain/`:**
- Purpose: Feature modules grouped by capability rather than by technical layer.
- Contains: subdirectories for `channel`, `sequence`, `station`, `sensor`, `rtc`, `configuration`, plus shared base files like `src/domain/model.h` and `src/domain/resolvers.h`.
- Key files: `src/domain/model.h`, `src/domain/query.h`, `src/domain/mutation.h`, `src/domain/control/core.h`.

**`src/domain/channel-control/`:**
- Purpose: Channel control strategies and their schemas.
- Contains: shared `control.h`, `controlFactory.h`, type enums, and strategy subdirectories `criteria/`, `timer/`, `range/`, `par/`.
- Key files: `src/domain/channel-control/control.h`, `src/domain/channel-control/controlFactory.h`, `src/domain/channel-control/timer/core.h`.

**`src/domain/sequence-control/`:**
- Purpose: Sequence control strategies parallel to channel-control.
- Contains: shared `sequenceControl.h`, `sequenceControlFactory.h`, type enums, and strategy subdirectories `sequence-criteria/`, `sequence-timer/`, `sequence-range/`, `sequence-par/`.
- Key files: `src/domain/sequence-control/sequenceControl.h`, `src/domain/sequence-control/sequenceControlFactory.h`, `src/domain/sequence-control/sequence-timer/core.h`.

**`src/logger/`:**
- Purpose: Runtime log and event buffering.
- Contains: `src/logger/log.h`, `src/logger/event.h`, `src/logger/loggerUtil.h`.
- Key files: `src/logger/log.h`, `src/logger/event.h`.

**`src/util/`:**
- Purpose: Shared helpers used across resolvers and low-level code.
- Contains: `src/util/util.h`, `src/util/JsonRequest.h`, conversion/printing helpers.
- Key files: `src/util/util.h`, `src/util/converter.h`.

**`src/dev/`:**
- Purpose: Development-mode simulation helpers enabled from `src/main.cpp` under `SG_MODE_DEVELOPMENT` and simplex mode.
- Contains: `src/dev/devSimplexStationTask.h`.
- Key files: `src/dev/devSimplexStationTask.h`.

**`docs/`:**
- Purpose: Example request payloads by endpoint family.
- Contains: folders such as `docs/channel-control/`, `docs/configuration/`, `docs/sequence-control/`, `docs/station/`.
- Key files: `docs/info/query/info.json`, `docs/channel-control/mutation/channel_save.json`, `docs/station/query/stations.json`.

**`test/`:**
- Purpose: Unity test entrypoint and hardware/system test fixtures.
- Contains: `test/test_main.cpp`, `test/system/rs485.h`.
- Key files: `test/test_main.cpp`, `test/system/rs485.h`.

## Key File Locations

**Entry Points:**
- `src/main.cpp`: Primary firmware entrypoint and runtime loop orchestration.
- `test/test_main.cpp`: Unity test entrypoint that reuses `bootstrap()`.

**Configuration:**
- `platformio.ini`: PlatformIO environments and library dependencies.
- `src/config.h`: Pin mappings, baud rates, and build-flag-selected board wiring.
- `src/bootstrap.h`: Runtime globals, scheduler setup, serial initialization, and boot metadata.

**Core Logic:**
- `src/combineContext.h`: Shared domain dependency graph.
- `src/combineResolvers.h`: Topic registration and dispatch.
- `src/domain/model.h`: EEPROM model base implementation.
- `src/domain/control/core.h`: Global controller mode switch between channel and sequence operation.
- `src/domain/channel/core.h`: Channel activation/deactivation behavior.
- `src/domain/sequence/core.h`: Sequence activation and GPIO chain construction.
- `src/domain/station/core.h`: Station bootstrapping and Modbus task registration.

**Testing:**
- `test/test_main.cpp`: Registers Unity tests with `RUN_TEST(...)`.
- `test/system/rs485.h`: Current hardware-oriented RS485 test helper.

## Naming Conventions

**Files:**
- Domain modules use role-based lowercase file names: `context.h`, `core.h`, `model.h`, `resolvers.h`, `permission.h`.
- Type-specific strategy folders use dashed names for grouped variants: `src/domain/channel-control/`, `src/domain/sequence-control/`, `src/domain/low-lv-command/`.
- Top-level entry headers use descriptive camel/pascal mixes already established in the repo: `src/combineContext.h`, `src/combineResolvers.h`, `src/deviceEndpoint.h`.

**Directories:**
- Feature areas live under `src/domain/<feature>/`, for example `src/domain/channel/`, `src/domain/sequence/`, `src/domain/station/`.
- Variant implementations live one level deeper under a shared feature family, for example `src/domain/channel-control/timer/` and `src/domain/precondition/range/`.

## Where to Add New Code

**New JSON API feature:**
- Primary code: add a new domain or extend an existing one under `src/domain/<feature>/`.
- Handler registration: wire new `Query`/`Mutation` classes into `src/combineResolvers.h`.
- Shared state: if the feature needs runtime dependencies, add a context pointer in `src/combineContext.h` and create a matching `context.h`.
- Payload examples: add matching example JSON under `docs/<feature>/query/` or `docs/<feature>/mutation/`.

**New control strategy:**
- Channel control implementation: place it under `src/domain/channel-control/<strategy>/` and add selection logic in `src/domain/channel-control/controlFactory.h`.
- Sequence control implementation: place it under `src/domain/sequence-control/<strategy>/` and add selection logic in `src/domain/sequence-control/sequenceControlFactory.h`.

**New persistent schema:**
- Implementation: define the schema and model in `src/domain/<feature>/model.h` as a `RomModel<Schema>` specialization.
- Activation/runtime behavior: keep hardware logic in `core.h` rather than directly in `resolvers.h`.

**Utilities:**
- Shared helpers: use `src/util/` for generic helpers or a domain-local `util/` folder such as `src/domain/station/util/` when the helper is feature-specific.

## Special Directories

**`src/domain/`:**
- Purpose: Main feature boundary for business logic and persistence.
- Generated: No.
- Committed: Yes.

**`docs/`:**
- Purpose: Request/response contract examples for serial API consumers.
- Generated: No.
- Committed: Yes.

**`test/`:**
- Purpose: PlatformIO Unity tests.
- Generated: No.
- Committed: Yes.

**`.planning/codebase/`:**
- Purpose: Generated repository mapping/reference docs for planning and execution tools.
- Generated: Yes.
- Committed: Typically yes if the workflow tracks planning artifacts.

**`.pio/`:**
- Purpose: PlatformIO build artifacts and downloaded dependencies.
- Generated: Yes.
- Committed: No.

---

*Structure analysis: 2026-04-15*
