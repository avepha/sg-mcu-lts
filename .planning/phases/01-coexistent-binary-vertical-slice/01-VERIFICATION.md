---
phase: 01-coexistent-binary-vertical-slice
verified: 2026-04-16T05:42:47Z
status: gaps_found
score: 7/8 must-haves verified
overrides_applied: 0
gaps:
  - truth: "The current JSON request/response communication remains fully operational and behaviorally unchanged while the new protocol path is enabled."
    status: failed
    reason: "`src/main.cpp` can drop a pending JSON request when the other serial port handles binary traffic in the same loop iteration."
    artifacts:
      - path: "src/main.cpp"
        issue: "Lines 99-125 `continue` immediately after `handleBinaryTraffic(...)`, so JSON already read on the other port never reaches the JSON dispatch block at lines 127-178."
      - path: "test/end-to-end/test_binary_coexistence.cpp"
        issue: "Coexistence tests only check byte classification and malformed frames; they do not cover simultaneous dual-port JSON+binary traffic."
    missing:
      - "Process both ports before continuing the loop, or preserve any already-read JSON request before servicing binary traffic."
      - "Add a host-side regression for one-loop dual-port arbitration where one port has JSON pending and the other has binary pending."
---

# Phase 1: Coexistent Binary Vertical Slice Verification Report

**Phase Goal:** Firmware ships a fully working new protocol path for one production-relevant operation while the existing JSON communication continues to work unchanged.
**Verified:** 2026-04-16T05:42:47Z
**Status:** gaps_found
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
| --- | --- | --- | --- |
| 1 | A backend client can send one real framed binary request, the device can execute it, and the client can decode the expected typed response successfully. | ✓ VERIFIED | `test/end-to-end/test_binary_protocol.cpp:146-198` builds a real `info.query` frame, routes it through `binaryRouteFrame`, decodes the response payload, and asserts `version`, `model`, and `device_environment`; `src/main.cpp:57-69` wires live binary traffic to `binaryRouteFrame(requestFrame, context)`. |
| 2 | The current JSON request/response communication remains fully operational and behaviorally unchanged while the new protocol path is enabled. | ✗ FAILED | `src/main.cpp:99-125` exits the loop immediately after binary handling on either port. If one port already read JSON at `104-106` or `118-120` and the other port hits the binary branch, execution never reaches the JSON response block at `127-178`. |
| 3 | Corrupted, oversize, and malformed binary traffic is rejected safely without crashing the firmware or corrupting runtime state. | ✓ VERIFIED | `src/protocol/binaryFrame.h:304-365` rejects bad sync, oversize length, CRC mismatch, unsupported version, unknown operation, and payload-length mismatch; `test/end-to-end/test_binary_protocol.cpp:88-140` and `test/end-to-end/test_binary_coexistence.cpp:88-110` cover CRC, oversize, and unknown-op failures; `rtk pio test -e native_protocol` passed. |
| 4 | The new protocol path includes explicit framing, bounded payload handling, correlation, versioning, routing, and canonical failure reporting for the delivered operation. | ✓ VERIFIED | `src/protocol/binaryProtocol.h:8-53` defines sync bytes, version, operation ID, status codes, envelope offsets, and size ceilings; `src/protocol/binaryFrame.h:281-365` encodes/decodes framed messages with `requestId`, status, and bounded payload length. |
| 5 | Backend implementers have a defined timeout and retry contract for request handling on the wire. | ✓ VERIFIED | `docs/protocol/binary-phase1-info.md:7-10` specifies `250 ms`, `1 retry only` on silence, and `no retry` on explicit protocol failures. |
| 6 | The first binary operation has one canonical envelope and schema contract. | ✓ VERIFIED | `schema/binary/phase1_info.proto:1-25` is the single typed schema source for `InfoQueryRequest`/`InfoQueryResponse`; `src/protocol/binaryProtocol.h:8-53` is the single envelope/status constant header consumed by frame and test code. |
| 7 | Fast automated tests exist for the protocol contract and coexistence path. | ✓ VERIFIED | `platformio.ini:146-152` defines `native_protocol`; `test/test_main.cpp:31-52` registers 16 native tests across protocol and coexistence suites; `rtk pio test -e native_protocol` passed. |
| 8 | The schema source can evolve additively without field-number reuse. | ✓ VERIFIED | `schema/binary/phase1_info.proto:6-7` and `18-20` reserve future ranges and retired names; request field `client_capabilities` uses `optional` at line 9 for presence semantics. |

**Score:** 7/8 truths verified

### Required Artifacts

| Artifact | Expected | Status | Details |
| --- | --- | --- | --- |
| `schema/binary/phase1_info.proto` | Canonical typed request/response schema for operation `0x01` | ✓ VERIFIED | Exists, substantive, and defines `InfoQueryRequest`, `InfoQueryResponse`, reserved identifiers, and optional presence semantics. |
| `schema/binary/phase1_info.options` | Nanopb string bounds for response payload | ✓ VERIFIED | Exists and bounds `version` and `model` to 32 chars. |
| `src/protocol/binaryProtocol.h` | Binary envelope/status constants and size limits | ✓ VERIFIED | Exists, substantive, and reused by frame helpers and tests. |
| `src/protocol/binaryFrame.h` | Bounded frame encode/decode and payload codec | ✓ VERIFIED | Exists, substantive, wired by `binaryRouter.h` and tests, and enforces CRC/size/version/op validation. |
| `src/protocol/binaryEndpoint.h` | Byte-oriented serial frame reader/writer and transport classifier | ✓ VERIFIED | Exists, substantive, wired by `src/main.cpp:11,20,43-44,57-69,99-125`. |
| `src/protocol/binaryRouter.h` | Operation `0x01` router reusing existing info data | ✓ VERIFIED | Exists, substantive, wired by `src/main.cpp:12,63` and `query_info` adapter at `104-119`. |
| `src/main.cpp` | Coexistent JSON/binary routing on shared ports | ⚠️ PARTIAL | Exists and is wired, but loop control at `99-125` can suppress pending JSON work when binary traffic arrives on the other port in the same iteration. |
| `docs/protocol/binary-phase1-info.md` | Backend-facing wire contract and runtime policy | ✓ VERIFIED | Exists, substantive, and documents framing, status, size, timeout, and retry contract. |
| `docs/protocol/examples/phase1-info.md` | Copy-paste request/response/failure fixtures | ✓ VERIFIED | Exists, substantive, and fixtures are checked by native tests. |
| `test/end-to-end/test_binary_protocol.cpp` | Host-side contract regression coverage | ✓ VERIFIED | Exists, substantive, registered in `test/test_main.cpp`, and passed in `native_protocol`. |
| `test/end-to-end/test_binary_coexistence.cpp` | Host-side coexistence/malformed regression coverage | ⚠️ PARTIAL | Exists and runs, but only validates classification/helpers, not the full `src/main.cpp` dual-port control flow that currently regresses coexistence. |

### Key Link Verification

| From | To | Via | Status | Details |
| --- | --- | --- | --- | --- |
| `schema/binary/phase1_info.proto` | `src/protocol/binaryProtocol.h` | operation and status definitions | ✓ WIRED | Both describe operation `0x01`, version `1`, and the Phase 1 contract; gsd-tools verified this link. |
| `src/protocol/binaryProtocol.h` | `test/end-to-end/test_binary_protocol.cpp` | native end-to-end assertions | ✓ WIRED | Tests import protocol constants and assert sync bytes, sizes, CRC, and fixture bounds. |
| `src/main.cpp` | `src/deviceEndpoint.h` | first-byte JSON routing | ✓ WIRED | `binarySelectTransport(... '{' ...)` selects JSON, then `handleJsonTraffic(rpiEndpoint/serialEndpoint, &requestString)` uses the legacy `DeviceEndpoint`. |
| `src/main.cpp` | `src/protocol/binaryEndpoint.h` | sync-byte binary routing | ✓ WIRED | `src/main.cpp:93-115` routes `binarySelectTransport(...peek...)` results to `handleBinaryTraffic(...)`; `binarySelectTransport` maps `SG_BINARY_SYNC_1` to the binary path in `src/protocol/binaryEndpoint.h:18-29`. |
| `src/protocol/binaryRouter.h` | `src/domain/info/resolvers.h` | existing info resolver adapter | ✓ WIRED | `binaryInfoSnapshotFromContext()` constructs `query_info` and reuses `resolve(...)` output instead of duplicating info payload composition. |
| `docs/protocol/binary-phase1-info.md` | `docs/protocol/examples/phase1-info.md` | wire examples and status table | ✓ WIRED | Contract doc points readers to examples and both share `request_id`, statuses, and framing details. |
| `test/end-to-end/test_binary_coexistence.cpp` | `src/main.cpp` | transport discrimination expectations | ⚠️ PARTIAL | Tests cover transport classification helpers and fixture drift, but not the live `main.cpp` one-loop arbitration that drops pending JSON when binary is handled on the other port. |

### Data-Flow Trace (Level 4)

| Artifact | Data Variable | Source | Produces Real Data | Status |
| --- | --- | --- | --- | --- |
| `src/protocol/binaryRouter.h` | `snapshot.version`, `snapshot.model`, `snapshot.deviceEnvironment` | `binaryInfoSnapshotFromContext()` → `query_info.resolve(...)` in `src/domain/info/resolvers.h:15-24` | Yes — response fields are populated from the existing firmware info resolver and then encoded by `binaryEncodeInfoQueryResponsePayload(...)` | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
| --- | --- | --- | --- |
| Native protocol contract and coexistence suite passes | `rtk pio test -e native_protocol` | 16/16 tests passed | ✓ PASS |
| Firmware still builds with new binary path enabled | `rtk pio run -e v2_dev` | Build succeeded for `v2_dev` | ✓ PASS |

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
| --- | --- | --- | --- | --- |
| `TRAN-01` | `01-02` | Device accepts binary requests using explicit packet framing with unambiguous frame boundaries | ✓ SATISFIED | `src/protocol/binaryFrame.h:281-365` and `src/protocol/binaryEndpoint.h:41-88` implement framed reads/writes with sync bytes and length-prefixed bodies. |
| `TRAN-02` | `01-02` | Device validates each frame with CRC/FCS and rejects corrupted packets safely | ✓ SATISFIED | `binaryFrameCrc16` / CRC check at `src/protocol/binaryFrame.h:336-340`; CRC-fail tests at `test/end-to-end/test_binary_protocol.cpp:88-104` and `test/end-to-end/test_binary_coexistence.cpp:94-97`. |
| `TRAN-03` | `01-02` | Device enforces bounded request and response frame size limits and fails oversize traffic predictably | ✓ SATISFIED | `SG_BINARY_MAX_PAYLOAD_SIZE`, `SG_BINARY_MAX_BODY_SIZE`, and oversize rejection at `src/protocol/binaryProtocol.h:16-19` and `src/protocol/binaryFrame.h:319-321`. |
| `ENVP-01` | `01-01`, `01-02` | Every request and response includes a protocol version field | ✓ SATISFIED | Version constant in `src/protocol/binaryProtocol.h:10`; envelope includes version at `src/protocol/binaryFrame.h:21-28,256-265`. |
| `ENVP-02` | `01-01`, `01-02` | Device routes requests using compact numeric operation identifiers | ✓ SATISFIED | `SG_BINARY_OPERATION_INFO_QUERY = 0x01` in `src/protocol/binaryProtocol.h:31-33`; route validation at `src/protocol/binaryFrame.h:272-274`. |
| `ENVP-03` | `01-01`, `01-03` | Every request and response includes a small correlation/request ID | ✓ SATISFIED | `requestId` is encoded in the envelope and echoed in docs/examples: `src/protocol/binaryFrame.h:21-28,262`, `docs/protocol/binary-phase1-info.md:10,41`. |
| `ENVP-04` | `01-01`, `01-02`, `01-03` | Device returns canonical machine-readable status and error codes | ✓ SATISFIED | Status enum in `src/protocol/binaryProtocol.h:35-44`; status responses built in `src/protocol/binaryRouter.h:32-47`. |
| `SCMA-01` | `01-01` | Request and response payloads use typed binary schemas | ✓ SATISFIED | Canonical schema at `schema/binary/phase1_info.proto` and checked-in bindings at `src/protocol/schema/phase1_info.pb.h`; frame helpers encode/decode the typed request/response payloads. |
| `SCMA-02` | `01-01` | Protocol defines additive-only field evolution rules and reserves retired identifiers | ✓ SATISFIED | Reserved ranges and names in `schema/binary/phase1_info.proto:6-7,19-20`. |
| `SCMA-03` | `01-01` | Schema can represent optional/presence semantics | ✓ SATISFIED | `optional uint32 client_capabilities = 1;` in `schema/binary/phase1_info.proto:9`; presence stored as `has_client_capabilities` in `src/protocol/schema/phase1_info.pb.h:18-21`. |
| `SCMA-04` | `01-01` | Schema source is structured for future Rust/Go/TypeScript generation | ✓ SATISFIED | Single `.proto` source plus `.options` file under `schema/binary/` is codegen-ready and not duplicated elsewhere. |
| `RUNT-01` | `01-03` | Firmware defines timeout behavior and documents retry expectations for the backend | ✓ SATISFIED | `docs/protocol/binary-phase1-info.md:7-10` makes timeout/retry policy explicit. |
| `RUNT-02` | `01-02`, `01-03` | Firmware safely rejects malformed frames and malformed payloads without crashing or corrupting runtime state | ✓ SATISFIED | Rejection paths in `src/protocol/binaryFrame.h:304-365` and `src/protocol/binaryRouter.h:52-64`; native malformed-frame tests pass. |
| `COMP-01` | `01-02`, `01-03` | New protocol coexists with current JSON communication without breaking existing JSON behavior | ✗ BLOCKED | `src/main.cpp:99-125` can skip JSON dispatch for one port when the other port handles binary traffic in the same loop iteration. |
| `SLCE-01` | `01-02` | One real production-relevant request/response operation is implemented end to end | ✓ SATISFIED | Operation `0x01` routes through `query_info` and returns typed framed responses; host tests and firmware build both pass. |
| `SLCE-02` | `01-03` | Backend can encode, send, decode, and handle protocol-level failures for the first operation | ✓ SATISFIED | Backend-facing contract/docs plus verified request/success/failure fixtures in `docs/protocol/examples/phase1-info.md`, tested by `test/end-to-end/test_binary_coexistence.cpp:112-135`. |

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
| --- | --- | --- | --- | --- |
| `src/main.cpp` | 99-125 | Early `continue` after binary/discard handling on one port | 🛑 Blocker | Breaks coexistence guarantee by dropping already-read JSON traffic from the other port in the same loop iteration. |
| `test/end-to-end/test_binary_protocol.cpp` / `test/end-to-end/test_binary_coexistence.cpp` | 142-210 / 80-135 | Coexistence tests only validate classification/helpers, not live dual-port arbitration | ⚠️ Warning | A real `src/main.cpp` coexistence regression escaped despite the native suite passing. |
| `src/domain/info/resolvers.h` | 17 | `//TODO: fix this` on `version` field | ℹ️ Info | Does not block the phase goal, but indicates the info source still has unresolved cleanup. |

### Gaps Summary

The binary vertical slice is mostly real: the protocol contract exists, native regressions pass, the firmware builds, the `info.query` operation is framed and routed, and backend docs/examples are present. The phase still misses its goal because coexistence is not fully achieved. `src/main.cpp` can drop a JSON request when the other serial port handles binary traffic in the same loop iteration, so the legacy JSON path is not behaviorally unchanged under mixed simultaneous traffic. That blocks `COMP-01` and the roadmap success criterion requiring unchanged JSON communication while the binary path is enabled.

---

_Verified: 2026-04-16T05:42:47Z_
_Verifier: the agent (gsd-verifier)_
