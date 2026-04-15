# Phase 1 Research — Coexistent Binary Vertical Slice

**Date:** 2026-04-16
**Scope:** Research the first binary request/response slice that can coexist with the current JSON serial API.

## Executive Summary

Phase 1 should deliver a **read-only `info` query vertical slice** over a new binary protocol that runs on the same `Serial` / `entryPort` boundary as the current JSON API without changing JSON behavior. The firmware should:

1. keep JSON parsing exactly as-is for frames that start with `{`
2. add a separate binary frame parser for frames that start with a reserved sync prefix
3. use a **fixed binary envelope** for version / operation / request ID / status
4. use a **protobuf schema with nanopb-generated C bindings** for the typed request/response payload
5. reject bad CRC, oversize frames, unsupported versions, and unknown operations before domain logic runs

This is the best Phase 1 slice because `query_info` is already implemented, small, side-effect free, and useful for backend/device capability discovery.

## Recommended Slice

### Selected operation

**Use `query_info` as operation ID `0x01`.**

Why this slice is the best fit:

- already exists in `src/domain/info/resolvers.h`
- returns stable production metadata (`version`, `model`, `device_env`)
- no EEPROM writes, scheduler mutations, or station side effects
- easy to compare against the existing JSON path for coexistence checks
- small enough to validate framing, CRC, schema, correlation, and canonical errors without dragging in unrelated subsystem complexity

### Secondary candidate considered

`query_configuration` is also viable, but it adds model reads and a slightly larger payload without improving the transport proof materially for Phase 1.

## Existing Codebase Signals

### Current transport boundary

- `src/main.cpp` reads JSON lines from `DeviceEndpoint` instances on `Serial` and `entryPort`
- `src/deviceEndpoint.h` assumes JSON frames begin with `{` and terminate with `\n`
- `src/loraEndpoint.h` already demonstrates a byte-oriented framed transport with CRC validation on `Serial2`
- `src/domain/station/modbus/modbusPacket.h` already uses `FastCRC16` for CRC validation

### Current resolver contract

- `src/combineResolvers.h` routes JSON `{topic, method, reqId, data}` to query/mutation handlers
- `src/domain/info/resolvers.h` exposes `query_info` and `query_device_status`
- `ValidationError` types in `src/validationError.h` already define machine-readable failure codes that can inspire canonical binary status mapping

## Recommended Technical Stack

### Framing

Use a **custom fixed frame** on `Serial` / `entryPort`:

`[sync1][sync2][body_len_le_u16][body_bytes...][crc16_le_u16]`

Recommended constants:

- `sync1 = 0xA5`
- `sync2 = 0x5A`
- `max_body_len = 128`

Why this framing:

- unambiguous frame boundaries (`TRAN-01`)
- bounded payload length before allocation/work (`TRAN-03`)
- simple resynchronization after malformed bytes
- CRC16 can reuse the existing `FastCRC16` dependency (`TRAN-02`)
- cheaper to parse on MCU than JSON or delimiter escaping schemes

### Envelope

Use a **fixed envelope inside `body_bytes`**:

`[version_u8][message_type_u8][operation_id_u8][request_id_le_u16][status_u8][payload_len_le_u16][payload...]`

Recommended semantics:

- `version`: protocol version, Phase 1 starts at `1`
- `message_type`: request = `1`, response = `2`
- `operation_id`: `0x01` = `info.query`
- `request_id`: small correlation ID echoed back (`ENVP-03`)
- `status`: canonical status enum (`OK`, `BAD_FRAME`, `CRC_FAIL`, `OVERSIZE`, `UNSUPPORTED_VERSION`, `UNKNOWN_OPERATION`, `DECODE_FAIL`, `INTERNAL_ERROR`)
- `payload_len`: bounded typed payload byte count

Why fixed envelope + typed payload:

- envelope fields are always present and cheap to read before payload decode
- payload remains schema-driven rather than ad hoc (`SCMA-01`)
- later phases can expand operations without redesigning the parser

### Typed schema

Use **proto3 `.proto` files as canonical schema source** plus **nanopb** for firmware bindings.

Recommended layout:

- `schema/binary/phase1_info.proto` — canonical message contract
- `schema/binary/phase1_info.options` — nanopb max sizes / static allocation rules
- checked-in generated files under `src/protocol/schema/`

Why protobuf + nanopb:

- one canonical schema source supports future Rust / Go / TypeScript generation (`SCMA-04`)
- additive field evolution rules are established by protobuf field numbering and `reserved` declarations (`SCMA-02`)
- proto3 `optional` supports explicit presence where needed (`SCMA-03`)
- nanopb is specifically designed for embedded C/C++ with bounded static fields

## External Documentation Findings

### Protobuf guidance

From protobuf docs:

- proto3 **recommends `optional`** for basic scalar fields when presence matters
- field numbers **must never be reused**; removed fields should be **reserved**
- adding new fields is wire-safe; old clients ignore unknown fields

Implication for this phase:

- define the Phase 1 request/response messages in `.proto`
- use `reserved` numbers/names if fields are retired later
- keep lower field numbers for frequently-present fields

### Nanopb guidance

From nanopb docs:

- `.proto` remains the portable interface description
- `.options` files let the project cap string/bytes sizes for static allocation
- framing is intentionally the application's responsibility
- runtime decode fails cleanly on oversized fields or bytes-left exhaustion

Implication for this phase:

- set max sizes in `.options` instead of heap-allocating variable payloads
- keep message framing separate from protobuf payload encoding
- commit generated files so firmware builds do not depend on protoc at build time

## Architecture Recommendation

### Do not replace `DeviceEndpoint`

Keep the current JSON path untouched for compatibility (`COMP-01`).

Instead:

1. keep `DeviceEndpoint` for `{...}\n` traffic
2. add a new `BinaryEndpoint` for `0xA5 0x5A ... CRC16` frames
3. let `src/main.cpp` dispatch by peeking the first incoming byte

This minimizes coexistence risk because the current JSON parser remains the source of truth for existing behavior.

### Reuse existing business logic

Do **not** duplicate the `info` business response in a second source file. The binary router should call the existing `query_info` resolver logic (or a tiny adapter around it) so JSON and binary return the same underlying data.

### Keep transport code separate from domain code

Recommended responsibility split:

| Area | Responsibility |
|------|----------------|
| `src/main.cpp` | transport selection and endpoint wiring |
| `src/protocol/binary*.h` | framing, CRC, envelope, parse/encode, status mapping |
| `src/protocol/schema/*` | generated typed payload bindings |
| `src/domain/info/resolvers.h` | business data source for the Phase 1 operation |
| `docs/protocol/*` | backend contract and wire examples |

## Canonical Failure Contract

Binary responses should never return free-form failure strings as the primary machine contract. Use a compact status enum in the envelope and optionally include a short debug string only when development mode needs it.

Recommended Phase 1 statuses:

- `0x00 OK`
- `0x01 BAD_FRAME`
- `0x02 CRC_FAIL`
- `0x03 OVERSIZE`
- `0x04 UNSUPPORTED_VERSION`
- `0x05 UNKNOWN_OPERATION`
- `0x06 DECODE_FAIL`
- `0x07 INTERNAL_ERROR`

## Timeout / Retry Contract

For Phase 1 `info.query`:

- firmware should finish frame assembly quickly and process synchronously in the current main loop
- backend timeout recommendation: **250 ms per request**
- retry recommendation: **1 retry only for timeout / transport silence**, because `info.query` is idempotent
- do **not** retry on explicit protocol failures such as `UNSUPPORTED_VERSION`, `UNKNOWN_OPERATION`, `DECODE_FAIL`, or `OVERSIZE`

This satisfies `RUNT-01` without pretending the protocol supports long-running operations yet.

## Testing Strategy

Current `test/` coverage is hardware-oriented. Phase 1 needs fast protocol tests that do not depend on attached hardware.

Recommended approach:

1. add a **native PlatformIO test environment** for protocol-only tests
2. keep frame parsing / encoding in platform-light files that compile under native C++
3. run native tests for parser, CRC, oversize, malformed, and schema round-trip cases
4. still run at least one firmware build (`rtk pio run -e v2_dev`) to verify ESP32 compatibility

## Don’t Hand-Roll

- don’t hand-roll a custom typed payload byte layout
- don’t replace or rewrite the JSON transport path
- don’t use unbounded `String` accumulation for binary payload storage
- don’t allocate payload buffers based on attacker-controlled lengths before max checks
- don’t use newline-delimited binary frames

## Common Pitfalls

1. **Shared serial reader conflicts** — two endpoint readers on the same port will race unless `main.cpp` peeks and routes before reading.
2. **Schema drift** — if `.proto` and generated C files diverge, backend and firmware contracts will drift; keep generated files checked in and update together.
3. **Missing oversize gate** — size must be rejected before CRC/payload decode work to avoid waste and memory pressure.
4. **Binary path bypassing existing business logic** — returning ad hoc info values from transport code will cause JSON/binary divergence.
5. **Native tests pulling Arduino-only headers** — keep core framing/schema helpers isolated so host tests stay fast.

## Architectural Responsibility Map

| Tier | Files | What belongs here |
|------|-------|-------------------|
| Transport orchestration | `src/main.cpp` | choose JSON vs binary path based on first byte, send response on originating serial port |
| Binary protocol boundary | `src/protocol/binaryProtocol.h`, `src/protocol/binaryFrame.h`, `src/protocol/binaryEndpoint.h`, `src/protocol/binaryRouter.h` | frame validation, CRC, envelope parsing, status mapping, request/response encoding |
| Typed contract | `schema/binary/phase1_info.proto`, `schema/binary/phase1_info.options`, `src/protocol/schema/*` | request/response payload schema and generated bindings |
| Domain logic | `src/domain/info/resolvers.h` | actual `info` query data production |
| Verification/docs | `test/protocol/*`, `docs/protocol/*` | automated checks and backend-facing contract docs |

## Validation Architecture

Recommended validation contract for execution:

- quick command: `rtk pio test -e native_protocol`
- full phase command: `rtk pio test -e native_protocol && rtk pio run -e v2_dev`
- required automated cases:
  - valid frame encodes/decodes
  - CRC failure rejected safely
  - oversize rejected safely
  - unsupported version rejected canonically
  - unknown operation rejected canonically
  - `info.query` payload round-trips through schema bindings
  - JSON discriminator still routes `{` traffic to the legacy path

## Planning Implications

The phase should be split into three plans:

1. **Contracts + fast test harness** — schema, envelope constants, native test environment
2. **Firmware binary transport + router** — frame parser, binary endpoint, `info.query` dispatch, coexistence wiring
3. **Protocol docs + regression coverage** — backend contract, timeout/retry guidance, malformed/coexistence regression tests

## Confidence

**High confidence**

- custom fixed frame + CRC16 is a good fit for this codebase
- `query_info` is the safest first vertical slice
- keeping JSON untouched is the lowest-risk coexistence strategy

**Medium confidence**

- exact nanopb dependency wiring in PlatformIO may need one implementation pass
- exact frame/body max sizes may be tuned slightly during execution once generated payload size is known
