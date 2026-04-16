---
phase: 01-coexistent-binary-vertical-slice
reviewed: 2026-04-16T05:39:08Z
depth: standard
files_reviewed: 14
files_reviewed_list:
  - schema/binary/phase1_info.proto
  - schema/binary/phase1_info.options
  - src/protocol/binaryProtocol.h
  - src/protocol/binaryFrame.h
  - src/protocol/binaryEndpoint.h
  - src/protocol/binaryRouter.h
  - src/protocol/schema/phase1_info.pb.h
  - src/protocol/schema/phase1_info.pb.c
  - src/main.cpp
  - docs/protocol/binary-phase1-info.md
  - docs/protocol/examples/phase1-info.md
  - test/end-to-end/test_binary_protocol.cpp
  - test/end-to-end/test_binary_coexistence.cpp
  - test/test_main.cpp
findings:
  critical: 0
  warning: 1
  info: 1
  total: 2
status: issues_found
---

# Phase 01: Code Review Report

**Reviewed:** 2026-04-16T05:39:08Z
**Depth:** standard
**Files Reviewed:** 14
**Status:** issues_found

## Summary

Reviewed the phase 01 binary protocol slice across schema, protocol helpers, `src/main.cpp`, docs, and native tests. The host suite and `v2_dev` build both pass, but there is one real coexistence regression in the main loop plus a test gap that failed to cover it.

## Warnings

### WR-01: Binary handling can drop a JSON request from the other port

**File:** `src/main.cpp:99-125`
**Issue:** The loop `continue`s immediately after handling binary traffic on either port. If one port already consumed a JSON request into `requestString` and the other port then has a binary frame, the binary branch exits the iteration before the JSON request is validated and answered. That breaks the stated coexistence guarantee under simultaneous traffic.
**Fix:** Process both ports for the iteration before `continue`, or defer the binary-path `continue` until after any already-read JSON request is dispatched.

```cpp
bool handledBinary = false;

if (rpiTransport == SG_BINARY_TRANSPORT_BINARY) {
  handledBinary = handleBinaryTraffic(rpiBinaryEndpoint) || handledBinary;
} else if (rpiTransport == SG_BINARY_TRANSPORT_JSON) {
  isDeviceDataComing = handleJsonTraffic(rpiEndpoint, &requestString);
}

if (serialTransport == SG_BINARY_TRANSPORT_BINARY) {
  handledBinary = handleBinaryTraffic(serialBinaryEndpoint) || handledBinary;
} else if (serialTransport == SG_BINARY_TRANSPORT_JSON) {
  isEndpointDataComing = handleJsonTraffic(serialEndpoint, &requestString);
}

if (isEndpointDataComing || isDeviceDataComing) {
  // existing JSON handling
}
if (handledBinary) {
  continue;
}
```

## Info

### IN-01: Native coexistence tests do not cover simultaneous dual-port traffic

**File:** `test/end-to-end/test_binary_protocol.cpp:142-210`, `test/end-to-end/test_binary_coexistence.cpp:80-135`
**Issue:** Current tests validate classification and helper routing, but they never exercise the `src/main.cpp` control-flow case where one port has JSON pending and the other has binary pending in the same loop iteration. That gap allowed the coexistence regression above to pass.
**Fix:** Extract one-loop transport arbitration into a host-testable helper and add a regression asserting that a pending JSON request is still processed when the other port delivers a binary frame in the same iteration.

---

_Reviewed: 2026-04-16T05:39:08Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
