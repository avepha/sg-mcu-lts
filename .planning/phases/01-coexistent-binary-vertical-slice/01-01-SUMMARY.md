---
phase: 01-coexistent-binary-vertical-slice
plan: 01
subsystem: protocol
tags: [protobuf, nanopb, crc16, platformio, unity, native-tests]
requires: []
provides:
  - Canonical Phase 1 `info.query` protobuf contract and nanopb bounds
  - Shared binary envelope constants, status codes, and host-safe CRC/frame helpers
  - Native PlatformIO protocol regression suite for CRC, oversize, and fixture-size checks
affects: [phase-01-plan-02, phase-01-plan-03, binary-protocol, backend-contract]
tech-stack:
  added: [PlatformIO native test environment]
  patterns: [single protobuf schema source, bounded payload constants, host-safe contract tests]
key-files:
  created:
    - schema/binary/phase1_info.proto
    - schema/binary/phase1_info.options
    - src/protocol/binaryProtocol.h
    - test/end-to-end/test_binary_protocol.cpp
  modified:
    - platformio.ini
    - test/test_main.cpp
key-decisions:
  - "Operation 0x01 is the canonical binary `info.query` slice backed by one checked-in proto schema."
  - "The binary contract is locked first with a native Unity suite so framing and size drift are caught before firmware transport wiring."
patterns-established:
  - "Protocol constants live in `src/protocol/binaryProtocol.h` and are reused by host-side regression tests."
  - "Native protocol tests run through `native_protocol` without Arduino bootstrap or device access."
requirements-completed: [ENVP-01, ENVP-02, ENVP-03, ENVP-04, SCMA-01, SCMA-02, SCMA-03, SCMA-04]
duration: 4min
completed: 2026-04-16
---

# Phase 01 Plan 01: Define the binary envelope/schema contract and native protocol test harness Summary

**Phase 1 `info.query` protobuf contract, bounded binary envelope constants, and native CRC/oversize regression tests.**

## Performance

- **Duration:** 4 min
- **Started:** 2026-04-16T05:14:59Z
- **Completed:** 2026-04-16T05:18:33Z
- **Tasks:** 2
- **Files modified:** 6

## Accomplishments
- Added the canonical Phase 1 protobuf schema and nanopb options for binary operation `0x01`.
- Defined shared sync bytes, versioning, operation IDs, status codes, size ceilings, and host-safe CRC/frame helpers.
- Added a fast `native_protocol` Unity suite that guards CRC mismatch, oversize rejection, and bounded fixture sizes without Arduino runtime.

## Task Commits

Each task was committed atomically:

1. **Task 1: Write the binary slice contract** - `e922548` (feat)
2. **Task 2: Add native protocol test harness and initial regression cases** - `c75a5d9` (test), `d559adb` (feat)

## Files Created/Modified
- `schema/binary/phase1_info.proto` - canonical proto3 request/response schema for `info.query` with reserved future identifiers.
- `schema/binary/phase1_info.options` - nanopb string bounds for static response allocation.
- `src/protocol/binaryProtocol.h` - binary envelope constants, status codes, and host-safe CRC/frame helper functions.
- `platformio.ini` - dedicated `native_protocol` test environment plus explicit per-environment PlatformIO settings.
- `test/end-to-end/test_binary_protocol.cpp` - contract regression tests for frame constants, CRC rejection, oversize handling, and fixture sizes.
- `test/test_main.cpp` - shared test entrypoint that keeps MCU bootstrap on Arduino while running host tests natively without `bootstrap()`.

## Decisions Made
- Used one checked-in `.proto` file plus `.options` as the only Phase 1 payload contract source.
- Kept response field bounds explicit at the schema/options layer and mirrored payload limits in `binaryProtocol.h` for early parser rejection.
- Ran native protocol checks through the shared test entrypoint so the host suite stays hardware-free while existing Arduino tests remain available.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Removed inherited ESP32-only test assumptions from the native suite path**
- **Found during:** Task 2 (Add native protocol test harness and initial regression cases)
- **Issue:** `native_protocol` initially inherited MCU-oriented PlatformIO and test entrypoint assumptions, causing board/bootstrap failures before contract tests could run.
- **Fix:** Made environment settings explicit in `platformio.ini` and gated `test/test_main.cpp` so native runs skip Arduino bootstrap while still executing the new host-side tests.
- **Files modified:** `platformio.ini`, `test/test_main.cpp`
- **Verification:** `rtk pio test -e native_protocol`
- **Committed in:** `c75a5d9`

---

**Total deviations:** 1 auto-fixed (1 blocking)
**Impact on plan:** Necessary to make the planned native test harness runnable; no product scope change.

## Issues Encountered
- PlatformIO test discovery compiled the existing MCU test entrypoint for the native environment; resolving that required explicitly isolating the host path before the RED/GREEN cycle could proceed.

## User Setup Required
None - no external service configuration required.

## Next Phase Readiness
- The contract and regression guardrails for operation `0x01` are in place for binary transport wiring.
- Phase 01 Plan 02 can build the framed parser/router against a fixed schema, status space, and tested size/CRC assumptions.

## Self-Check: PASSED
