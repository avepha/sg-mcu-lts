# Codebase Concerns

**Analysis Date:** 2026-04-15

## Tech Debt

**Unsafe serial frame parsing:**
- Issue: `DeviceEndpoint::embrace` appends bytes into a fixed `char raw[2048]` buffer without any bounds check, so a malformed or oversized line can overrun stack memory before the newline arrives.
- Files: `src/deviceEndpoint.h`, `src/main.cpp`
- Impact: Serial noise or an oversized JSON request can corrupt memory, crash the MCU, or produce nondeterministic behavior on the request-processing core.
- Fix approach: Enforce a maximum payload length before writing into `raw`, discard overlong frames, and surface a structured validation error back through the JSON API.

**Unchecked array and vector indexing across request handlers:**
- Issue: Multiple resolvers write directly from request payloads into fixed-size arrays without defensive caps at the write site. Examples include `reqData["channelOrderAndTiming"]` copied into `schema.sequence.channelOrderAndTiming` and station sensor arrays written by index.
- Files: `src/domain/sequence/resolvers.h`, `src/domain/station/resolvers.h`, `src/domain/channel/resolvers.h`
- Impact: Invalid or partially validated payloads can write past schema bounds in EEPROM-backed structs, causing corrupt persistent state and hard-to-recover field bugs.
- Fix approach: Validate sizes again at mutation entry, clamp writes to struct capacities, and return explicit errors when payloads exceed the supported slot count.

**Manual memory management dominates runtime objects:**
- Issue: Long-lived singletons and task objects are allocated with `new` across the runtime, with custom self-destruction patterns such as `delete this` used inside scheduler callbacks.
- Files: `src/main.cpp`, `src/combineContext.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/sequence/util/sequenceGpioChain.h`, `src/domain/gpio/core.h`
- Impact: Ownership is difficult to reason about, teardown paths are fragile, and lifetime bugs are likely when task timing changes.
- Fix approach: Centralize ownership in context/core objects, replace `delete this` patterns with explicit destruction from a manager, and document which scheduler owns each task.

**EEPROM writes have no wear-management policy:**
- Issue: Every settings mutation persists immediately with `EEPROM.commit()`, while `RomModel` tracks write counts but never enforces limits or batching.
- Files: `src/domain/model.h`, `src/domain/channel/resolvers.h`, `src/domain/sequence/resolvers.h`, `src/domain/configuration/resolvers.h`, `src/domain/rtc/resolvers.h`, `src/domain/station/resolvers.h`
- Impact: Frequent configuration changes can prematurely wear EEPROM sectors and cause persistent configuration loss on deployed boards.
- Fix approach: Batch saves, debounce repeated writes, and use the tracked write counter to warn or rotate storage before exhaustion.

## Known Bugs

**Modbus response handling reads out of bounds on short packets:**
- Symptoms: The code marks packets with `vBytes.size() <= 2` as failed, but still reads `vBytes[0]` and `vBytes[1]` afterward.
- Files: `src/domain/station/modbus/modbusTask.h`
- Trigger: Any truncated or noise-corrupted RS485 frame shorter than three bytes.
- Workaround: None in code; only stable wiring and valid stations reduce the chance of hitting it.

**GPIO task disable path is a known crash site:**
- Symptoms: The implementation leaves `OnDisable()` empty and includes an inline note that calling `.disable()` crashes the system.
- Files: `src/domain/gpio/util/gpioTask.h`, `src/domain/gpio/core.h`
- Trigger: Any code path that disables a `GpioTask` directly rather than letting it self-destruct through the finish callback.
- Workaround: Current code avoids direct disable calls and instead deletes tasks indirectly through `finishCallback`.

**Notification tray drops oldest notification without freeing it:**
- Symptoms: When the tray reaches capacity, the oldest pointer is erased from the vector but never deleted.
- Files: `src/domain/notification/notificationTray.h`
- Trigger: More than 12 notifications added without acknowledgements.
- Workaround: Keep notification volume low; there is no in-code cleanup for the evicted object.

**Station model edits do not rebuild the live station pool:**
- Symptoms: Station mutations update EEPROM only; the in-memory `StationCore` and registered `ModbusTask` stations are created once at startup and never refreshed.
- Files: `src/domain/station/resolvers.h`, `src/domain/station/core.h`, `src/domain/station/modbus/modbusTask.h`
- Trigger: Calling `station_data_save` or `station_data_remove` after boot.
- Workaround: Restart the device after station configuration changes so runtime state matches persisted state.

**Timezone formatting is off by one for 10 values:**
- Symptoms: Offset formatting uses `> 10` instead of `>= 10`, so values equal to `10` are rendered with an extra leading zero.
- Files: `src/domain/rtc/resolvers.h`
- Trigger: Saving or reading a timezone offset of `+10:xx`, `-10:xx`, or minute offset `10`.
- Workaround: None in code; consumers must tolerate malformed formatting.

## Security Considerations

**No authentication or authorization on the control interface:**
- Risk: The serial JSON API accepts any request containing `topic`, `method`, and `reqId`, and permission classes enforce input shape rather than caller identity.
- Files: `src/main.cpp`, `src/domain/resolvers.h`, `src/domain/permission.h`, `src/combineResolvers.h`
- Current mitigation: None detected beyond request-format validation.
- Recommendations: Add an authenticated transport or signed command envelope before permitting mutations on production hardware.

**Destructive maintenance commands are exposed as normal mutations:**
- Risk: `clear_nvmemory` wipes the full EEPROM and restarts the board, while `restart` reboots immediately.
- Files: `src/domain/low-lv-command/resolvers.h`, `src/combineResolvers.h`
- Current mitigation: None detected; both endpoints are always registered.
- Recommendations: Gate these mutations behind a development build flag, a physical service mode, or an authenticated admin command path.

**Boot metadata and internal state are emitted openly over serial:**
- Risk: The firmware prints version, upload date, model, pins, boot count, and heap telemetry on accessible serial interfaces.
- Files: `src/bootstrap.h`, `src/main.cpp`, `src/domain/info/resolvers.h`
- Current mitigation: Production mode suppresses only the explicit `SG_MODE: DEVELOPMENT` line.
- Recommendations: Reduce boot verbosity in production builds and avoid streaming heap/debug telemetry unless an explicit debug mode is enabled.

## Performance Bottlenecks

**Busy-wait serial loops tie up CPU time:**
- Problem: Serial readers spin in `while (true)` loops with short delays or no delay until a timeout/gap is reached.
- Files: `src/deviceEndpoint.h`, `src/loraEndpoint.h`, `src/domain/station/modbus/modbusTask.h`, `src/main.cpp`
- Cause: Frame parsing is implemented as polling instead of buffered/event-driven reads.
- Improvement path: Move frame assembly to bounded nonblocking state machines and yield predictably between chunks.

**High churn from large transient JSON documents:**
- Problem: Request handling allocates multiple `DynamicJsonDocument` instances up to 2048 bytes per loop iteration, plus additional 1024-byte documents for logs, events, and notifications.
- Files: `src/main.cpp`, `src/domain/**/resolvers.h`, `src/validationError.h`
- Cause: Most endpoints construct fresh heap-backed documents for every request and side-channel message.
- Improvement path: Reuse buffers where possible, reduce document capacities to observed payload sizes, and shift static response shapes to `StaticJsonDocument` where safe.

**Unbounded log and event queues can accumulate under backpressure:**
- Problem: `LoggerTray` and `EventTray` are plain `std::queue` containers with no capacity limit.
- Files: `src/logger/log.h`, `src/logger/event.h`, `src/main.cpp`
- Cause: Producers can push faster than the serial links drain messages.
- Improvement path: Add queue caps, drop policies, and observability for overflow so telemetry does not consume all free heap.

## Fragile Areas

**Sequence shutdown relies on deferred self-deletion:**
- Files: `src/domain/sequence/core.h`, `src/domain/sequence-control/sequenceControl.h`, `src/domain/sequence/util/sequenceGpioChain.h`
- Why fragile: `deactivateControls()` nulls pointers immediately, but actual object destruction is deferred to future scheduler callbacks via `deactivationFlag` and `delete this`.
- Safe modification: Treat sequence deactivation as asynchronous; if changing scheduler intervals or activation order, verify that old tasks cannot run after new pointers are published.
- Test coverage: No automated tests exercise activation/deactivation races.

**Station packet decoding assumes trusted payload shape:**
- Files: `src/domain/station/station.h`, `src/domain/station/modbus/modbusTask.h`, `src/domain/station/modbus/modbusPacket.h`
- Why fragile: Packet size, byte count, and sensor count are trusted after minimal checks, then copied into stack arrays and sensor slots.
- Safe modification: Add hard validation for minimum frame length, expected byte count, and sensor count before any `memcpy` or index access.
- Test coverage: `test/system/rs485.h` verifies only a happy-path packet and does not cover malformed frames.

**Runtime/router wiring is centralized in one large registration file:**
- Files: `src/combineResolvers.h`
- Why fragile: Query and mutation registration is hand-maintained across a 284-line file; a missing include or registration silently hides an endpoint.
- Safe modification: Add new endpoints in the same edit as resolver creation and registration, then verify with request examples under `docs/`.
- Test coverage: No router-level test enumerates or validates the full endpoint map.

## Scaling Limits

**Notification capacity is fixed at 12 entries:**
- Current capacity: `NotificationTray::MAX_TRAY = 12`.
- Limit: New notifications evict old ones once the tray is full, and the eviction path currently leaks memory.
- Scaling path: Introduce bounded storage with explicit destruction on eviction and support paged retrieval if alert volume grows.

**Station configuration is capped by compile-time arrays:**
- Current capacity: `StationSchema::stations[8]` with `sensors[10]` per station.
- Limit: Payload writes and runtime structures assume these limits and will not scale safely without additional validation.
- Scaling path: Move to length-tracked containers with validated bounds and rebuild the runtime station pool from persisted config changes.

**Channel and precondition counts are hard-coded throughout control logic:**
- Current capacity: 8 channels and 3 preconditions per channel/sequence path.
- Limit: Many loops, validations, and array writes are compiled around these exact sizes.
- Scaling path: Consolidate capacities into shared constants and update models, permissions, and serializers together before increasing limits.

## Dependencies at Risk

**TaskScheduler-driven object lifetime model:**
- Risk: The project depends on `TaskScheduler@3`, but task objects are tightly coupled to scheduler callback timing and rely on self-deletion semantics.
- Impact: Any scheduler behavior change can surface latent use-after-free or missed-cleanup bugs in sequence and GPIO execution.
- Migration plan: Encapsulate scheduler interactions behind core/task managers before upgrading scheduler behavior or swapping libraries.

**ArduinoJson heap usage on constrained firmware:**
- Risk: `ArduinoJson@6.21.5` is used pervasively with heap-backed `DynamicJsonDocument` allocations.
- Impact: Fragmentation or reduced free heap can destabilize command handling and telemetry generation under sustained traffic.
- Migration plan: Audit actual document sizes, convert stable shapes to static allocations, and add low-heap tests around heavy request paths.

## Missing Critical Features

**No runtime reload after persistent configuration changes:**
- Problem: Several mutations save EEPROM-backed state but do not consistently rebuild the live objects that actually enforce control and station behavior.
- Blocks: Reliable remote reconfiguration without manual restart, especially for station topology and related background tasks.

**No overflow/health protection around serial command ingestion:**
- Problem: Request readers do not enforce safe frame sizes, queue limits, or transport-level throttling.
- Blocks: Safe operation on noisy serial links or in environments with untrusted command sources.

## Test Coverage Gaps

**Request parsing and malformed-input handling:**
- What's not tested: Oversized JSON frames, truncated packets, invalid Modbus CRC flows, and malformed date/time payloads.
- Files: `src/deviceEndpoint.h`, `src/loraEndpoint.h`, `src/domain/station/modbus/modbusTask.h`, `src/domain/rtc/resolvers.h`
- Risk: Crash-prone input paths can regress silently and only fail on physical hardware.
- Priority: High

**Scheduler-driven control lifecycles:**
- What's not tested: Channel activation/deactivation, sequence restarts, GPIO task cleanup, and race-prone self-destruction paths.
- Files: `src/domain/channel/core.h`, `src/domain/sequence/core.h`, `src/domain/gpio/core.h`, `src/domain/gpio/util/gpioTask.h`
- Risk: Timing-sensitive bugs will appear only after deployment and are difficult to reproduce from logs alone.
- Priority: High

**Station configuration mutations and persistence consistency:**
- What's not tested: Adding/removing stations, updating sensor lists, and verifying that runtime station polling matches EEPROM contents after mutations.
- Files: `src/domain/station/resolvers.h`, `src/domain/station/core.h`, `src/domain/model.h`
- Risk: Field devices can report stale stations or corrupt saved configuration without any automated detection.
- Priority: High

**Overall test suite breadth:**
- What's not tested: Nearly all domain resolvers and core classes; the checked-in suite runs a single hardware-oriented RS485 test from `test/test_main.cpp`.
- Files: `test/test_main.cpp`, `test/system/rs485.h`
- Risk: Refactors to routing, permissions, EEPROM models, and control logic have no host-level safety net.
- Priority: High

---

*Concerns audit: 2026-04-15*
