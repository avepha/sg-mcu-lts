# Technology Stack

**Project:** SG MCU LTS Protocol Rework
**Research scope:** Embedded-friendly custom schema-driven binary protocol for ESP32 firmware + new backend
**Researched:** 2026-04-15
**Overall confidence:** HIGH

## Recommended Stack

### Core protocol stack

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Protocol Buffers (`proto3` syntax, bounded subset) | Current stable | Schema/IDL for request and response payloads | Best fit for “compact + evolvable + multi-language.” Official protobuf docs still position it as the standard binary schema language for structured cross-language data, and it has the strongest backend tooling story. Use `proto3` for maximum generator compatibility across firmware and backend tools. | HIGH |
| Nanopb | 0.4.x | Firmware-side encode/decode on ESP32 | Nanopb is explicitly designed for 32-bit MCUs, pure C, small code size, small RAM usage, and static allocation without `malloc`. That matches this brownfield Arduino/ESP32 firmware far better than full protobuf runtimes. | HIGH |
| Buf CLI | v2 config | Schema linting, breaking-change checks, codegen orchestration | Buf is the standard 2025-era protobuf workflow tool. Use it to keep `.proto` files disciplined from day one: `buf lint`, `buf breaking`, `buf generate`. That matters more here because protocol evolution is a primary project goal. | HIGH |

### Wire format and transport

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Custom fixed binary header + protobuf payload | Project-defined | Fast routing and correlation before payload decode | Do **not** put transport metadata inside a fully generic protobuf envelope. A small fixed header lets firmware validate version, opcode, request ID, flags, and payload length before invoking Nanopb. That keeps the hot path deterministic and cheap. | HIGH |
| COBS framing | Minimal in-house implementation | Packet framing over serial byte stream | Binary protobuf bytes need framing. COBS is a proven embedded serial framing pattern that tolerates arbitrary payload bytes and resynchronizes better than newline-delimited text. Implement it directly or with a tiny local helper; keep framing ownership in-repo. | MEDIUM |
| CRC16-CCITT or CRC32 | Reuse existing `FastCRC` where practical | Corruption detection per frame | The protocol needs frame integrity independent of protobuf parsing. The repo already uses `FastCRC`; reusing that keeps dependencies small and implementation familiar. Prefer CRC16 for short serial frames, CRC32 if payloads grow materially. | HIGH |

### Backend/tooling stack

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| `protobuf-es` | 2.x | TypeScript runtime + codegen for backend or tooling | If the new backend or CLI tooling is in TypeScript/Node, use `protobuf-es`. Its own docs claim full required conformance coverage, Editions 2024 support, standard plugin flow, and plain TypeScript output. It is the cleanest TS protobuf stack right now. | HIGH |
| `protoc-gen-go` + `google.golang.org/protobuf` | Current stable | Go backend/client generation | If the backend is Go, use the official protobuf generator/runtime. It is the default, well-documented path and integrates cleanly with Buf/protoc. | HIGH |
| Official Protobuf Rust runtime/codegen | Current stable | Future Rust SDK/backend support | Official Rust protobuf support now exists and is documented. Use this for future Rust SDK generation if Rust becomes a first-class consumer; validate concrete build ergonomics during that phase. | MEDIUM |

### Schema design rules

| Rule | Use | Why | Confidence |
|------|-----|-----|------------|
| One request message + one response message per operation | Yes | Keeps firmware routing explicit and keeps generated types small and readable. | HIGH |
| Numeric operation IDs / opcodes | Yes | Existing firmware routes by `topic` + `method`; replace that with compact numeric IDs to remove string parsing overhead. | HIGH |
| `optional`, `oneof`, enums with `*_UNSPECIFIED = 0` | Yes | Matches protobuf evolution best practices and keeps future schema changes safe. | HIGH |
| Reserve deleted field numbers and enum values | Mandatory | Official protobuf best practices are explicit here; this is essential for safe evolution. | HIGH |
| Bounded strings/bytes/repeated fields via Nanopb options | Mandatory on firmware-facing messages | Prevents heap pressure and unbounded parse cost on ESP32. | HIGH |
| `google.protobuf.Any` | Avoid | Too dynamic for an MCU-facing protocol and protobuf best practices prefer alternatives where possible. | HIGH |

## Recommended implementation approach

### 1. Keep protobuf for payload schema, not full transport

Use protobuf messages for request/response bodies, but keep a tiny transport header outside protobuf.

Recommended frame layout:

```text
COBS(
  header {
    u8  version;
    u8  flags;
    u16 opcode;
    u32 request_id;
    u16 payload_len;
    u16 status_or_reserved;
  }
  payload_bytes[protobuf]
  crc16
) + 0x00
```

Why this split:
- firmware can reject bad version/opcode/length before decode
- request/response correlation is fixed-cost
- protobuf payload stays language-neutral and codegen-friendly
- transport can evolve independently from message schema

### 2. Use a protobuf subset that is explicitly MCU-safe

Use:
- scalars
- enums
- `optional`
- `oneof`
- short repeated fields with hard upper bounds
- short strings/bytes with hard upper bounds

Avoid on firmware-facing messages:
- deeply nested message trees
- unbounded repeated fields
- maps unless clearly needed
- `Any`
- services/RPC frameworks on the device wire

### 3. Generate, do not hand-maintain, payload structs

Recommended source of truth:

```text
proto/
  sg/device/v1/common.proto
  sg/device/v1/channel_set.proto
  sg/device/v1/status_get.proto
```

Recommended generation flow:
- Buf owns lint/breaking/generate workflow
- Nanopb generator emits firmware C structs/descriptors
- backend generator emits TS/Go/Rust types as needed

### 4. Add a thin firmware translation layer

Do **not** let generated Nanopb structs leak across the whole firmware.

Use this boundary instead:
- transport/frame parser
- opcode router
- generated payload decode/encode
- adapter layer that maps generated messages to existing domain resolvers

This fits the current architecture: `main.cpp` / endpoint / resolver routing stays intact while the protocol boundary changes.

### 5. Start with one operation and freeze the pattern

For the first milestone:
- choose one real command with non-trivial request + response
- define its `.proto`
- define fixed opcode
- implement frame parse + CRC + decode + route + encode + response
- build one backend integration test and one MCU test fixture around raw bytes

Do this before designing the whole protocol catalog.

## Prescriptive choices

### Use this

1. **Protobuf + Nanopb + Buf** as the core schema toolchain.
2. **COBS-framed serial packets with CRC** for transport robustness.
3. **Fixed header outside protobuf** for cheap routing and deterministic parsing.
4. **Generated backend types** from the same `.proto` files used by firmware.
5. **Proto evolution discipline from day one**: linting, reserved fields, versioned packages.

### Do not use this

| Do not use | Why not |
|------------|---------|
| JSON or CBOR as the new primary device protocol | Better than JSON in some cases, but still not as strong as protobuf for schema evolution + codegen across firmware/backend languages. JSON also fails the payload-size/parsing-cost goal directly. |
| FlatBuffers | Great for zero-copy desktop/server workloads, but the main win is avoiding parse/unpack. That is less important than bounded memory, simple encode/decode, and mature embedded C support in this project. Nanopb is the safer brownfield fit. |
| Cap’n Proto | Fast, but a worse fit for a constrained serial request/response redesign than protobuf + Nanopb. Tooling and embedded integration are not as straightforward here. |
| Hand-rolled packed C structs as the schema | Small at first, but they become brittle immediately when fields evolve, languages diverge, or SDK generation is needed. This project explicitly wants future Rust/Go/TS support. |
| `required` fields | Official protobuf best practices explicitly say not to add required fields. They are hostile to long-lived compatibility. |
| `google.protobuf.Any` as a generic command envelope | Too dynamic, hard to bound on firmware, and protobuf best practices prefer extensions/explicit types over `Any` where possible. |
| PacketSerial as a core long-term dependency | Useful as prior art, but the project should own a tiny framing implementation instead of depending on an older Arduino-specific framing library for a critical protocol boundary. |

## Minimal initial file/tool layout

```text
proto/
  buf.yaml
  buf.gen.yaml
  sg/device/v1/common.proto
  sg/device/v1/<operation>.proto

tools/
  generate-proto.sh

src/protocol/
  frame.h
  frame.cpp
  crc.h
  codec.h
  codec.cpp
  router.h
  router.cpp
  generated/   # nanopb output or vendored generated files
```

## Installation / toolchain notes

```bash
# schema tooling
brew install bufbuild/buf/buf
brew install protobuf

# firmware codegen support
python -m pip install nanopb protobuf grpcio-tools

# TypeScript backend/tooling
npm install @bufbuild/protobuf
npm install -D @bufbuild/protoc-gen-es
```

## Confidence assessment by recommendation

| Recommendation | Confidence | Notes |
|----------------|------------|-------|
| Protobuf as schema language | HIGH | Strong official documentation and strongest cross-language ecosystem. |
| Nanopb for ESP32 firmware | HIGH | Official docs explicitly target 32-bit MCUs with static allocation and small footprint. |
| Buf for schema workflow | HIGH | Current official docs show v2 configs, linting, codegen, and breaking checks as the modern workflow. |
| Fixed header + protobuf payload | HIGH | Strong implementation fit for current brownfield router and embedded cost model. |
| COBS framing + CRC | MEDIUM | Strong embedded practice and compatible tooling evidence; less direct official standardization than protobuf/Buf/Nanopb. |
| `protobuf-es` for TS backend | HIGH | Current project docs are strong and conformance claims are explicit. |
| Official Rust protobuf for future SDKs | MEDIUM | Officially supported now, but concrete team ergonomics should be validated when Rust becomes active scope. |

## Sources

- Protocol Buffers Overview — https://protobuf.dev/overview/
- Protocol Buffers Best Practices — https://protobuf.dev/best-practices/dos-donts/
- Nanopb Overview — https://jpa.kapsi.fi/nanopb/docs/
- Buf Docs — https://buf.build/docs/
- Buf Lint Quickstart — https://buf.build/docs/lint/quickstart/
- Buf Breaking Quickstart — https://buf.build/docs/breaking/quickstart/
- Protobuf-ES README — https://github.com/bufbuild/protobuf-es
- Go generated protobuf docs — https://protobuf.dev/reference/go/go-generated/
- Rust generated protobuf docs — https://protobuf.dev/reference/rust/rust-generated/
- FlatBuffers overview — https://flatbuffers.dev/
- Cap’n Proto introduction — https://capnproto.org/
- PacketSerial README (COBS/SLIP framing prior art) — https://github.com/bakercp/PacketSerial
