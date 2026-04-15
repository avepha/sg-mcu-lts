# Feature Landscape

**Domain:** Compact binary request/response protocol for embedded firmware
**Researched:** 2026-04-15

## Table Stakes

Features users expect. Missing = protocol feels fragile or incomplete.

| Feature | Why Expected | Complexity | Notes |
|---------|--------------|------------|-------|
| Explicit frame boundaries plus corruption check | Serial links need unambiguous packet start/end and a way to reject damaged frames. | Med | Use a length-delimited or escaped frame with CRC/FCS. This is foundational; every other feature depends on it. |
| Request/response correlation ID | Backend must match replies to in-flight requests and detect stale/duplicate traffic. | Low | Small numeric `reqId`/token field is enough. Distinct from transport-level sequence/retry state. |
| Fixed message envelope | Every packet needs stable top-level fields such as protocol version, message kind, opcode/topic ID, request ID, payload length, and status/error code. | Low | Replaces JSON's implicit self-description with a compact, predictable header. |
| Canonical success/error response model | Operators need machine-readable failures, not ad hoc parse errors. | Low | Include bounded error codes like `BAD_FRAME`, `UNKNOWN_OPCODE`, `INVALID_ARGUMENT`, `BUSY`, `INTERNAL`, `UNSUPPORTED_VERSION`. |
| Bounded payload sizes | Embedded protocols must fail predictably instead of overrunning buffers. | Low | Declare max request/response sizes per transport and reject oversize frames early. |
| Timeout and retry semantics | Request/response protocols are incomplete without a defined lost-response story. | Med | Define who retries, retry budget, duplicate handling, and which operations are safe/idempotent to retry. |
| Forward-compatible schema rules | The protocol needs room to evolve without rewrites. | Med | Reserve field/opcode ranges, allow unknown-field skipping if using TLV/proto-style payloads, and never reuse retired IDs. |
| Presence/optional field semantics | Firmware/admin/config APIs quickly need to distinguish omitted, zero, false, and empty values. | Med | Especially important for partial updates and future generated SDKs. |
| Deterministic command routing | Brownfield firmware already routes by topic/method; the binary protocol needs an equally clear dispatch key. | Low | Use compact numeric service/opcode IDs that map cleanly onto existing resolver families. |
| Capability to send structured binary payloads, not raw byte blobs | Without typed payload conventions, the protocol becomes custom parsing per command and loses evolvability. | Med | Prefer schema-defined fields over ad hoc byte offsets for most operations. |

## Differentiators

Features that set product apart. Not expected, but valued.

| Feature | Value Proposition | Complexity | Notes |
|---------|-------------------|------------|-------|
| Schema-first message definitions with optional code generation | Gives the project a path to Rust/Go/TypeScript SDKs without forcing generation in milestone 1. | Med | Strong fit for project requirements. Nanopb/protobuf-style discipline is more valuable than inventing message layouts by hand. |
| Capability negotiation / hello handshake | Lets firmware and backend agree on protocol version, max frame size, supported opcodes, and optional features at session start. | Med | Valuable once more than one backend/client version exists; not required for the first end-to-end proof. |
| Streaming or block-wise transfer for large bodies | Prevents one-off large operations from forcing huge buffers or transport fragmentation. | High | Useful later for firmware/admin blobs, bulk config export, or diagnostics. Defer unless first operation needs it. |
| Separate transport ack from business response | Improves robustness for long-running commands by allowing “accepted/processing” before final result. | Med | Useful when command execution can outlast serial timing assumptions. |
| Introspectable error details | Makes backend debugging and fleet support much easier than opaque numeric failure only. | Low | Keep core error code compact, add optional short detail fields for validation location or subcode. |
| Event/notification multiplexing on same wire format | Enables logs, health, and async device events without inventing a second protocol later. | Med | Good design win if represented as a separate message kind, but initial rollout can stay request/response only. |
| Per-operation idempotency metadata | Lets the backend safely retry some commands automatically. | Low | Particularly useful for unreliable links or reboot recovery. |

## Anti-Features

Features to explicitly NOT build.

| Anti-Feature | Why Avoid | What to Do Instead |
|--------------|-----------|-------------------|
| Full transport-agnostic protocol abstraction from day one | The project is serial-bound today; over-generalizing early adds complexity without proving value. | Design clean framing and envelope layers so another transport can be added later if needed. |
| Human-readable/self-describing payloads | Recreates JSON overhead and parsing cost, which is the problem being removed. | Keep the wire format compact and numeric; rely on schema/docs/tools for readability. |
| Dynamic field-name dictionaries or reflection-heavy runtime parsing | Expensive in flash/RAM and undermines predictable embedded parsing. | Use generated or table-driven descriptors with numeric field IDs. |
| Mandatory multi-language SDK/codegen delivery in milestone 1 | Slows the proving milestone and is explicitly out of scope. | Make the schema codegen-friendly now; ship hand-written backend/client bindings first if faster. |
| Rich query language / generic RPC interpreter | Creates a second embedded platform rather than a protocol. | Expose explicit opcodes for real firmware operations. |
| Compression in the initial rollout | Small binary frames usually do not justify code size, CPU cost, and debug pain. | Win first on compact encoding and bounded payloads. Revisit only for proven large-body traffic. |
| In-protocol authentication/crypto redesign in the first milestone | Important in some deployments, but not the core problem this milestone is solving and easy to over-scope. | Treat security as transport/deployment work unless a concrete threat model requires protocol-layer auth now. |
| Concurrent multi-part transactions | Hard to test, easy to deadlock, and unnecessary for proving one end-to-end operation. | Keep one request -> one response semantics first; add block-wise upload/download later if needed. |
| Ad hoc per-command binary layouts | Fast initially, but causes long-term parser sprawl and breaks SDK ambitions. | Keep one shared envelope and one shared schema discipline across all opcodes. |

## Feature Dependencies

```text
Frame boundaries + CRC/FCS -> safe packet parsing
safe packet parsing -> request/response correlation
fixed envelope -> deterministic command routing
fixed envelope -> canonical success/error model
schema discipline -> forward-compatible evolution
schema discipline -> presence/optional field semantics
schema discipline -> optional SDK/codegen path
request/response correlation + timeout/retry semantics -> reliable backend behavior
bounded payload sizes -> safe retries and predictable memory use
capability negotiation -> optional features like block-wise transfer or async events
block-wise transfer -> large admin/firmware/diagnostic operations
separate transport ack from business response -> long-running operations
```

## MVP Recommendation

Prioritize:
1. Explicit framing with CRC/FCS and bounded frame size
2. Stable envelope with version, opcode/topic ID, request ID, and status/error code
3. Schema-defined typed payloads with forward-compatible field numbering

Defer:
- Capability negotiation: useful once version skew exists, not necessary for the first proof.
- Block-wise transfer: high value for large payloads, but unnecessary unless the first real operation exceeds practical frame limits.
- Async event multiplexing: design for it in the header, but do not require it for the initial request/response milestone.

## Opinionated Recommendation

For this firmware, table stakes should look like: **small fixed envelope + typed binary payload + explicit framing + CRC + request ID + bounded errors + versioned schema rules**.

Do **not** ship a "clever" custom protocol that is only compact because every opcode has its own hand-written byte layout. That saves a little space now but makes brownfield expansion across channel control, configuration, sequence control, status, and admin commands much harder. The better differentiator is a compact protocol that is still schema-driven and evolvable.

## Sources

- Project context: `.planning/PROJECT.md`, `.planning/codebase/ARCHITECTURE.md`
- Protocol Buffers encoding guide — compact tag/value encoding, length-delimited fields, unknown-field skipping behavior, field numbering discipline: https://protobuf.dev/programming-guides/encoding/ _(HIGH)_
- Protocol Buffers field presence note — explicit presence matters for default values and patch/update semantics: https://protobuf.dev/programming-guides/field_presence/ _(HIGH)_
- Nanopb overview — embedded-oriented protobuf implementation, static allocation, bounded field sizing, callback support for larger-than-RAM messages: https://jpa.kapsi.fi/nanopb/docs/ _(MEDIUM)_
- RFC 7252 (CoAP) — constrained request/response design, tokens/message IDs, compact headers, confirmable vs non-confirmable exchanges, response codes: https://datatracker.ietf.org/doc/html/rfc7252 _(HIGH)_
- RFC 1662 (PPP in HDLC-like Framing) — framed serial transport, escaping, invalid frame handling, and FCS for error detection: https://datatracker.ietf.org/doc/html/rfc1662 _(HIGH)_
- RFC 7959 (CoAP block-wise transfer) — large-payload transfers should be explicit and block-wise, not left to fragmentation: https://datatracker.ietf.org/doc/html/rfc7959 _(HIGH)_
