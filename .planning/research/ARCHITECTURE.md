# Architecture Patterns

**Domain:** Embedded firmware communication protocol redesign
**Researched:** 2026-04-15
**Confidence:** HIGH for firmware integration, MEDIUM for backend-side shape

## Recommended Architecture

Build the new protocol as a strict layered boundary in front of the existing resolver/domain system, not as a rewrite of domain logic.

**Recommended stack of layers:**

```text
Host / Backend Service
  -> schema-aware client SDK or hand-written client
  -> transport frame writer/reader
  -> request/response codec
  -> serial bridge

ESP32 Firmware
  -> transport frame reader/writer
  -> envelope parser + integrity checks
  -> schema codec (message <-> typed structs)
  -> protocol router adapter
  -> existing resolver/domain layer
  -> models / cores / schedulers / hardware
```

The key design choice is to separate **framing**, **message envelope**, **schema payload**, and **business routing** into different components. The current code mixes transport, parsing, validation, and resolver dispatch in `src/main.cpp`; the new protocol should split those concerns so the binary protocol can evolve without touching domain logic.

## Component Boundaries

| Component | Responsibility | Communicates With |
|-----------|---------------|-------------------|
| `ProtocolSchema` | Canonical operation IDs, field IDs, enums, error codes, version rules | Firmware codec, backend codec, optional generators |
| `TransportFrame` | Byte framing: sync/magic, length, flags, correlation ID, CRC | Serial/UART on both sides |
| `EnvelopeCodec` | Encode/decode request, response, event, and error envelopes | TransportFrame, MessageCodec |
| `MessageCodec` | Encode/decode typed payload structs from schema definitions | EnvelopeCodec, RouteAdapter |
| `RouteAdapter` (firmware) | Map operation ID + payload struct into current resolver calls / typed handlers | MessageCodec, existing resolvers/domains |
| `BinaryEndpoint` (firmware) | Non-blocking serial byte intake and frame emission | `main.cpp` runtime loop, TransportFrame |
| `BackendProtocolGateway` | Serial session management, retries/timeouts, device connection state | Backend application services, EnvelopeCodec |
| `Domain Handlers` | Execute actual device logic | RouteAdapter, existing `CombineContext` / domain cores |

## Where Each Part Should Live

### Firmware

```text
src/protocol/
  schema/
    protocol_schema.h        # operation IDs, field IDs, versions, limits, enums
    generated/               # optional later; hand-written first is acceptable
  framing/
    frame.h                  # frame header/trailer definitions
    frame_codec.h            # parse/serialize bytes + CRC
  codec/
    envelope.h               # request/response/error/event envelope structs
    request_codec.h
    response_codec.h
    payload_codecs/*.h       # per-operation binary payload codecs
  routing/
    protocol_router.h        # dispatch by operation ID
    resolver_adapter.h       # bridge into existing query/mutation model
    operations/*.h           # one file per operation family
  transport/
    binary_endpoint.h        # replaces JSON newline endpoint for new path
```

### Backend service

```text
backend/protocol/
  schema/                    # same source schema or generated artifacts
  framing/                   # exact frame implementation match
  codec/                     # request/response/event codecs
  client/                    # firmware client / session abstraction
  gateway/                   # retries, correlation tracking, timeouts
```

**Recommendation:** keep the schema source-of-truth outside firmware-specific code if possible (for example `protocol-schema/` at repo root or shared repo), but keep firmware-specific generated or hand-written C++ structs under `src/protocol/`.

## Data Flow

### Request flow

1. Backend application calls `BackendProtocolGateway.send(operation, payload)`.
2. Backend `MessageCodec` serializes typed payload according to `ProtocolSchema`.
3. Backend `EnvelopeCodec` wraps payload with operation ID, request ID/correlation ID, and protocol version.
4. `TransportFrame` adds frame marker, payload length, flags, and CRC.
5. Frame bytes cross UART/serial to ESP32.
6. Firmware `BinaryEndpoint` accumulates bytes and extracts complete frames.
7. Firmware `frame_codec` validates frame shape, max length, and CRC before any schema decode.
8. Firmware `EnvelopeCodec` decodes envelope metadata.
9. Firmware `MessageCodec` decodes payload into a typed request struct.
10. Firmware `RouteAdapter` maps the operation to a handler.
11. Handler either:
    - calls a thin adapter into existing `CombineResolvers`, or
    - directly invokes a domain core for new binary-native handlers.
12. Domain returns a typed result or typed protocol error.
13. Firmware encodes response payload, wraps it in response envelope, frames it, and sends bytes back.
14. Backend matches the response by correlation ID and returns typed data to application code.

### Event/log flow

1. Firmware domain/runtime code emits log/event/notification internally.
2. Protocol event publisher maps those to **unsolicited protocol messages** with distinct message kind values.
3. Messages are framed exactly like responses but marked as `event`, `log`, or `notification`.
4. Backend gateway routes them to observers without pretending they are responses.

## Protocol Structure Recommendation

### 1. Transport framing

Use a small fixed header plus variable payload:

```text
| magic(2) | version(1) | kind(1) | flags(1) | headerLen(1) |
| bodyLen(2 or 4) | correlationId(4) | operationId(2) | crc(2 or 4) | body(N) |
```

**Why:**
- framing must be recoverable from arbitrary serial byte streams
- length-delimited parsing is cheaper than delimiter escaping
- CRC belongs to the frame layer, not the business payload
- correlation ID enables concurrent requests and clean backend timeout handling

**Recommendation:** use a fixed max frame size for the first milestone and reject oversized frames early.

### 2. Schema definition

Define schema by **numeric operation IDs and numeric field tags**, not string keys.

Example logical shape:

```text
Operation 0x0101: channel.get
  Request fields:
    1 -> channel_id : u8
  Response fields:
    1 -> enabled : bool
    2 -> control_type : enum
    3 -> state : enum
```

**Rules:**
- operation IDs are stable forever once shipped
- field tags are never reused
- optional fields are preferred over breaking changes
- reserve field/tag ranges for future expansion
- schema version at frame/envelope level, not per field blob

### 3. Codec layer

The codec should convert between:

```text
bytes <-> envelope structs <-> typed operation structs
```

The codec should **not** know about EEPROM, schedulers, GPIO, or `CombineContext`.

### 4. Routing layer

The routing layer should convert:

```text
typed request struct -> domain call -> typed response struct
```

For brownfield integration, the first implementation should use a **resolver adapter**:

- binary request -> typed struct
- typed struct -> small JSON-like intermediary or direct resolver call parameters
- existing resolver/domain logic executes
- output mapped into typed binary response

This minimizes milestone risk. After the first proven operation, hot paths can be migrated from resolver-adapter routing to direct typed handlers.

## Recommended Firmware Integration Strategy

### Near-term: adapter architecture

Do **not** replace `CombineResolvers` first. Instead:

1. keep current domain resolvers and contexts intact
2. add a parallel `ProtocolRouter`
3. implement one `OperationHandler` for the first real request/response operation
4. inside that handler, call existing domain logic through a thin adapter

This creates a parallel protocol boundary without destabilizing the entire firmware.

### Medium-term: hybrid routing

After 1-3 operations are proven:

- keep read-heavy/simple operations on resolver adapters
- move performance-sensitive or complex operations to direct typed handlers
- keep operation registry numeric and centralized in `src/protocol/schema/protocol_schema.h`

### Long-term: binary-native service layer

If rollout succeeds broadly, the system can evolve toward:

```text
ProtocolRouter -> typed service handlers -> domain cores/models
```

At that point `CombineResolvers` becomes a legacy compatibility surface rather than the main contract.

## Suggested Build Order

1. **Define protocol invariants first**
   - frame format
   - request/response/error/event kinds
   - correlation ID rules
   - size limits
   - versioning rules

2. **Define the initial schema for one production operation**
   - choose one operation that touches real domain logic but has bounded payload shape
   - freeze operation ID and field tags before writing handlers

3. **Build shared codecs before routing**
   - frame parser/serializer
   - CRC validation
   - envelope encode/decode
   - typed payload codec

4. **Add firmware `BinaryEndpoint` and parser loop**
   - integrate in `main.cpp` beside existing JSON path
   - keep binary path feature-flagged during bring-up

5. **Add `ProtocolRouter` and first `OperationHandler`**
   - route decoded operation ID to a handler
   - adapt into existing resolver/domain logic

6. **Build backend gateway only after the wire contract is frozen**
   - backend session manager
   - correlation tracking
   - timeout/retry policy

7. **Add unsolicited event/log support last**
   - request/response correctness matters first
   - events require subscription and backpressure decisions

## Why This Build Order

- If framing is unstable, every later component churns.
- If schema IDs change after backend work starts, both sides churn.
- If routing is built before codecs, handler tests are low-value because the wire contract is still unproven.
- If events/logs are built too early, the milestone gets distracted from proving one solid request/response loop.

## Patterns to Follow

### Pattern 1: Layered binary boundary
**What:** keep frame parsing, schema decoding, and business dispatch separate.
**When:** always.
**Example:**

```typescript
frameBytes -> parseFrame() -> decodeEnvelope() -> decodePayload() -> handleOperation()
```

### Pattern 2: Stable numeric contract
**What:** numeric operation IDs and field tags are the compatibility surface.
**When:** from the first shipped version.
**Example:**

```typescript
const OP_CHANNEL_GET = 0x0101
const FIELD_CHANNEL_ID = 1
```

### Pattern 3: Thin brownfield adapters
**What:** bridge binary protocol handlers into existing resolver/domain code instead of rewriting everything.
**When:** first milestone and early rollout.
**Example:**

```typescript
BinaryRequest -> OperationHandler -> ResolverAdapter -> existing domain logic
```

## Anti-Patterns to Avoid

### Anti-Pattern 1: Mixing framing and schema logic
**What:** CRC, length parsing, field decoding, and business validation in one class.
**Why bad:** hard to test, hard to recover from partial frames, high rewrite risk.
**Instead:** separate `frame_codec`, `envelope_codec`, and `operation_handler`.

### Anti-Pattern 2: Stringly-typed binary routing
**What:** decode binary payloads only to recreate `topic`/`method` strings everywhere.
**Why bad:** keeps JSON-era overhead and weak typing.
**Instead:** use numeric operation IDs, with adapters only at the brownfield edge.

### Anti-Pattern 3: Schema-owned-by-codec implementation
**What:** field numbers and operation IDs scattered through firmware and backend code.
**Why bad:** guarantees drift.
**Instead:** centralize schema definitions and generate constants where possible.

### Anti-Pattern 4: Big-bang JSON replacement
**What:** convert all operations before the first end-to-end proof.
**Why bad:** too much blast radius for a serial protocol rewrite.
**Instead:** parallel path, one real operation first.

## Scalability Considerations

| Concern | At 100 users/devices | At 10K users/devices | At 1M users/devices |
|---------|----------------------|----------------------|---------------------|
| Firmware RAM pressure | Hand-written structs/codecs are fine | Need stricter frame size caps and zero-copy decode where possible | Generated codecs and aggressive buffer discipline matter |
| Backend connection handling | Single device session logic is enough | Need pooled serial/session supervision and observability | Need durable protocol gateway/service split |
| Schema evolution | Manual review is enough | Need compatibility policy and schema registry discipline | Need automated contract tests and generated SDKs |
| Operation rollout | Per-operation manual implementation is fine | Need operation family grouping and deprecation policy | Need version negotiation and formal compatibility matrix |

## Roadmap Implications

- **Phase 1 should be wire-contract design**, not handler implementation.
- **Phase 2 should be firmware framing/codec integration** with a feature-flagged parallel endpoint.
- **Phase 3 should be one end-to-end operation via resolver adapter**.
- **Phase 4 should be backend gateway hardening and event/log expansion**.

The architecture naturally splits the roadmap into protocol foundation -> firmware integration -> first business operation -> expansion.

## Sources

- Internal project scope: `/Users/farhan/Documents/sg-mcu-lts/.planning/PROJECT.md`
- Current firmware architecture: `/Users/farhan/Documents/sg-mcu-lts/.planning/codebase/ARCHITECTURE.md`
- Current integration surface: `/Users/farhan/Documents/sg-mcu-lts/.planning/codebase/INTEGRATIONS.md`
- Request loop and endpoint wiring: `src/main.cpp`
- Current serial boundary: `src/deviceEndpoint.h`
- Current request router: `src/combineResolvers.h`
