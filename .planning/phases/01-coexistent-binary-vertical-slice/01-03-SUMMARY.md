---
phase: 01-coexistent-binary-vertical-slice
plan: 03
subsystem: protocol
tags: [binary-protocol, docs, testing, crc16, platformio, unity]
requires:
  - phase: 01-02
    provides: Working binary transport, framed responses, and operation `0x01` routing
provides:
  - Backend-facing Phase 1 wire contract with explicit timeout, retry, status, and size rules
  - Copy-paste hex fixtures for `info.query` request, success, and failure frames
  - Native regression coverage for coexistence, malformed traffic, and docs-to-implementation drift
affects: [backend-contract, binary-protocol, native-tests, phase-01-complete]
tech-stack:
  added: []
  patterns: [markdown wire fixtures verified by native tests, explicit timeout-and-retry contract for binary requests]
key-files:
  created:
    - docs/protocol/binary-phase1-info.md
    - docs/protocol/examples/phase1-info.md
    - test/end-to-end/test_binary_coexistence.cpp
  modified:
    - test/test_main.cpp
key-decisions:
  - "Published Phase 1 runtime behavior as a backend contract with 250 ms timeout, one retry on silence only, and no retries on explicit protocol failures."
  - "Loaded markdown wire examples directly in the native suite so backend fixtures cannot drift from firmware helpers silently."
patterns-established:
  - "Backend-facing binary docs should include exact hex fixtures plus envelope/status tables so integrators do not need to inspect firmware code."
  - "Protocol docs in `docs/protocol/` are treated as executable fixtures by host-side regression tests."
requirements-completed: [RUNT-01, RUNT-02, COMP-01, SLCE-02]
duration: 2min
completed: 2026-04-16
---

# Phase 01 Plan 03: Publish backend wire docs and finish coexistence/malformed regression coverage Summary

**Backend-ready `info.query` wire docs with tested hex fixtures and native regressions that lock JSON coexistence plus malformed-frame handling.**

## Performance

- **Duration:** 2 min
- **Started:** 2026-04-16T05:34:07Z
- **Completed:** 2026-04-16T05:35:50Z
- **Tasks:** 2
- **Files modified:** 4

## Accomplishments
- Published a backend-facing Phase 1 binary contract that makes framing, status codes, size ceilings, timeout, and retry semantics explicit.
- Added copy-paste wire examples for request, success, failure, and corrupted-CRC cases for operation `0x01`.
- Added a native coexistence suite that verifies JSON-vs-binary discrimination, malformed-frame safety, and drift-free documentation fixtures.

## Task Commits

Each task was committed atomically:

1. **Task 1: Publish the backend wire contract for operation 0x01** - `f894faa` (docs)
2. **Task 2: Add final malformed/coexistence regression coverage** - `d735b9e` (test), `c6bdf84` (feat)

## Files Created/Modified
- `docs/protocol/binary-phase1-info.md` - backend contract for the Phase 1 binary envelope, statuses, size limits, and retry policy.
- `docs/protocol/examples/phase1-info.md` - concrete request, success, failure, and CRC-corruption hex fixtures for `info.query`.
- `test/end-to-end/test_binary_coexistence.cpp` - host-side regressions that parse markdown fixtures and verify coexistence/malformed handling.
- `test/test_main.cpp` - native Unity registration for the new coexistence suite.

## Decisions Made
- Locked the backend runtime contract in docs rather than leaving timeout/retry behavior implicit in firmware source.
- Verified markdown examples from disk in native tests so documentation becomes part of the regression surface.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing Critical] Registered the new coexistence suite in `test/test_main.cpp`**
- **Found during:** Task 2 (Add final malformed/coexistence regression coverage)
- **Issue:** The plan listed the new test file, but repository test rules require new tests to be registered in `test/test_main.cpp` or they would never run.
- **Fix:** Included `test/end-to-end/test_binary_coexistence.cpp` and added `RUN_TEST(...)` entries for the new regression cases.
- **Files modified:** `test/test_main.cpp`
- **Verification:** `rtk pio test -e native_protocol && rtk pio run -e v2_dev`
- **Committed in:** `d735b9e` (part of Task 2 commit)

---

**Total deviations:** 1 auto-fixed (1 missing critical)
**Impact on plan:** Required for correctness because unregistered tests would not have enforced the planned regression coverage.

## Issues Encountered
- TDD red phase failed at link time because fixture-loading helpers were intentionally absent; the green commit added markdown parsing helpers and request builders inside the host-side suite.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness
- Phase 1 now has an explicit backend handoff for operation `0x01` with reproducible examples and runtime expectations.
- Native regressions cover coexistence and malformed traffic, so Phase 1 can be marked complete without requiring extra protocol clarification.

## Self-Check: PASSED

---
*Phase: 01-coexistent-binary-vertical-slice*
*Completed: 2026-04-16*
