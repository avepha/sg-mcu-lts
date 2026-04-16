---
phase: 01-coexistent-binary-vertical-slice
plan: 02
subsystem: protocol
tags: [binary-protocol, serial, protobuf, crc16, platformio, unity]
requires:
  - phase: 01-01
    provides: Canonical envelope constants, schema contract, and native protocol test harness
provides:
  - Checked-in Phase 1 schema binding files and bounded binary frame helpers
  - Binary serial endpoint routing that coexists with the legacy JSON transport on the same ports
  - Operation `0x01` responses encoded from existing `query_info` data through a binary router adapter
affects: [phase-01-plan-03, binary-protocol, firmware-transport, backend-contract]
tech-stack:
  added: []
  patterns: [first-byte transport discrimination, framed binary endpoint helper, query_info adapter for binary replies]
key-files:
  created:
    - src/protocol/schema/phase1_info.pb.h
    - src/protocol/schema/phase1_info.pb.c
    - src/protocol/binaryFrame.h
    - src/protocol/binaryEndpoint.h
    - src/protocol/binaryRouter.h
  modified:
    - src/main.cpp
    - test/end-to-end/test_binary_protocol.cpp
    - test/test_main.cpp
key-decisions:
  - "Transport selection now peeks the first byte so `{` keeps the legacy JSON path and `0xA5` enters the binary frame path."
  - "Binary operation `0x01` reuses `query_info` output through a narrow snapshot adapter instead of duplicating firmware info values."
patterns-established:
  - "Binary framing, payload encoding, and canonical status handling stay in host-testable helpers under `src/protocol/`."
  - "Main-loop serial coexistence is enforced by classifying the first incoming byte before any endpoint consumes transport bytes."
requirements-completed: [TRAN-01, TRAN-02, TRAN-03, ENVP-01, ENVP-02, ENVP-03, ENVP-04, RUNT-02, COMP-01, SLCE-01]
duration: 9min
completed: 2026-04-16
---

# Phase 01 Plan 02: Implement framed binary transport and route operation `0x01` through existing info logic Summary

**Coexistent serial routing for framed binary `info.query` requests with CRC-checked envelopes and responses sourced from existing firmware info logic.**

## Performance

- **Duration:** 9 min
- **Started:** 2026-04-16T05:21:01Z
- **Completed:** 2026-04-16T05:30:14Z
- **Tasks:** 2
- **Files modified:** 8

## Accomplishments
- Added checked-in schema binding files and a bounded binary frame codec with canonical rejection paths for CRC, oversize, version, and operation failures.
- Added binary endpoint and router helpers that keep frame parsing and `info.query` response shaping host-testable.
- Updated `src/main.cpp` to discriminate `{` vs `0xA5` on the shared serial ports so JSON stays on the legacy path while binary requests get framed responses on the originating port.

## Task Commits

Each task was committed atomically:

1. **Task 1: Implement generated schema bindings and bounded frame codec** - `3e7ccf6` (test), `4e0bd72` (feat)
2. **Task 2: Wire binary serial handling and route operation 0x01 through existing info logic** - `a9ebf0c` (test), `412c503` (feat)

## Files Created/Modified
- `src/protocol/schema/phase1_info.pb.h` - checked-in Phase 1 info message bindings used by the frame/router helpers.
- `src/protocol/schema/phase1_info.pb.c` - companion schema binding source kept with the checked-in payload contract.
- `src/protocol/binaryFrame.h` - host-safe frame envelope, CRC, payload encode/decode, and canonical validation helpers.
- `src/protocol/binaryEndpoint.h` - transport classifier plus binary serial endpoint read/write support.
- `src/protocol/binaryRouter.h` - operation `0x01` routing and `query_info` snapshot adapter for binary responses.
- `src/main.cpp` - first-byte transport discrimination and binary endpoint wiring on `Serial` / `entryPort`.
- `test/end-to-end/test_binary_protocol.cpp` - regression coverage for frame decode, routing, coexistence, and malformed traffic.
- `test/test_main.cpp` - native registration for the new binary routing regression cases.

## Decisions Made
- Classified shared-port traffic before any endpoint reads bytes so malformed binary input cannot silently fall into the JSON path.
- Kept binary business data aligned with JSON by adapting `query_info` output instead of duplicating version/model/environment composition.
- Returned canonical framed error responses from the binary router for malformed and unsupported requests rather than dropping them silently.

## Deviations from Plan

None - plan executed exactly as written.

## Issues Encountered
- The native protocol suite does not compile `src/` C files directly with `test_build_src = no`, so the checked-in binding defaults were kept header-local while the companion `.c` file remains available for firmware-side integration.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness
- Backend-facing wire docs can now describe a real framed request/response flow for operation `0x01`.
- Phase 01 Plan 03 can expand malformed/coexistence regression coverage and publish the backend contract against a working firmware path.

## Self-Check: PASSED

---
*Phase: 01-coexistent-binary-vertical-slice*
*Completed: 2026-04-16*
