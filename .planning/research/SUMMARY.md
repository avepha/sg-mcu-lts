# Project Research Summary

**Project:** SG MCU LTS Protocol Rework
**Domain:** Embedded firmware binary protocol redesign for ESP32 + backend
**Researched:** 2026-04-15
**Confidence:** HIGH

## Executive Summary

This project is a brownfield ESP32 firmware protocol rewrite: replace newline-delimited JSON over serial with a compact, schema-driven binary request/response protocol that is cheaper to parse, more robust on noisy links, and safer to evolve over time. The research strongly converges on the same expert pattern: keep transport framing separate from payload schema, use a small fixed envelope for routing and correlation, and use Protocol Buffers with Nanopb for typed payloads instead of inventing ad hoc per-command binary layouts.

The recommended approach is opinionated: use **protobuf + Nanopb + Buf** for schema discipline, put a **fixed binary header outside the protobuf payload**, add **explicit framing plus CRC**, and integrate the new path as a **parallel protocol boundary** in front of the existing resolver/domain system. Do not rewrite domain logic first. Prove the design with one production-relevant vertical slice, then expand operation families once framing, schema rules, and firmware safety limits are stable.

The main risks are not about wire size; they are about protocol correctness and migration discipline. The biggest failure modes are unstable field/opcode numbering, underspecified frame behavior, unsafe MCU decode limits, and trying to replace too much at once. Mitigation is clear: freeze schema IDs early, define frame invariants before handlers, enforce bounded payloads and malformed-input tests, keep one canonical schema source of truth, and defer advanced features like handshake, streaming, and async events until the base request/response loop is proven.

## Key Findings

### Recommended Stack

The stack research is the strongest area: official protobuf, Nanopb, and Buf documentation all align with the project goals of compact payloads, safe evolution, and future multi-language support. The best fit is a split architecture where protobuf defines business payloads while transport metadata stays in a tiny fixed header for deterministic firmware parsing.

**Core technologies:**
- **Protocol Buffers (`proto3`)**: payload schema/IDL — best cross-language contract for compact, evolvable typed messages.
- **Nanopb 0.4.x**: firmware encode/decode — designed for 32-bit MCUs with static allocation and bounded memory.
- **Buf v2 workflow**: linting, breaking checks, codegen orchestration — enforces schema discipline from the start.
- **Custom fixed binary header**: version, kind/flags, opcode, request ID, payload length, status — enables cheap routing before payload decode.
- **COBS framing + CRC16/CRC32**: serial framing and corruption detection — makes packet boundaries and resynchronization explicit.
- **`protobuf-es` / official Go runtime / official Rust runtime**: future backend and SDK generation path — supports TypeScript, Go, and Rust consumers from the same schema.

**Critical version/tooling notes:**
- Use `proto3` syntax and versioned packages from day one.
- Keep firmware-facing fields bounded with Nanopb options.
- Reserve retired field numbers and enum values permanently.

### Expected Features

Research across features and pitfalls agrees that the MVP is not “binary everything,” but a disciplined request/response foundation. The launch bar is a reliable framed transport, stable routing envelope, typed payloads, predictable errors, and explicit evolution rules.

**Must have (table stakes):**
- Explicit frame boundaries plus CRC/FCS.
- Stable envelope with protocol version, opcode/message kind, request ID, payload length, and status/error code.
- Deterministic numeric command routing.
- Bounded payload sizes and early oversize rejection.
- Timeout/retry semantics with duplicate-safe correlation behavior.
- Forward-compatible schema rules with optional/presence support.
- Structured typed payloads instead of per-command raw byte blobs.

**Should have (competitive):**
- Schema-first definitions with optional code generation.
- Introspectable error details.
- Capability negotiation/hello handshake once multiple client versions exist.
- Event/notification multiplexing on the same wire format.
- Per-operation idempotency metadata for safer retries.

**Defer (v2+):**
- Block-wise or streaming transfer for large bodies.
- Separate transport ack from business response.
- Mandatory multi-language SDK delivery in milestone 1.
- Broad transport abstraction or protocol-layer security redesign unless a concrete threat model forces it.

### Architecture Approach

The architecture research is clear: add the new protocol as a layered boundary in front of existing firmware resolvers. Separate **framing**, **envelope**, **payload codec**, and **routing adapter** into different components, then bridge decoded operations into current domain logic through thin adapters. This minimizes blast radius and preserves brownfield momentum.

**Major components:**
1. **TransportFrame / frame codec** — accumulates bytes, extracts frames, validates shape, length, and CRC.
2. **EnvelopeCodec** — decodes fixed metadata like version, kind, opcode, correlation ID, and status.
3. **MessageCodec** — converts payload bytes into typed request/response structs from the canonical schema.
4. **ProtocolRouter / RouteAdapter** — dispatches numeric operations and bridges them into existing resolver/domain calls.
5. **BinaryEndpoint** — feature-flagged serial endpoint running beside the current JSON path during bring-up.
6. **BackendProtocolGateway** — session management, retries, timeouts, and response correlation on the backend side.

**Patterns to follow:**
- Layered binary boundary, not mixed parsing/business logic.
- Stable numeric contract from first shipment.
- Thin brownfield adapters before direct typed handlers.
- Build order of invariants -> codecs -> endpoint integration -> first operation -> backend hardening -> async expansion.

### Critical Pitfalls

1. **Reusing or renumbering schema IDs** — freeze tags/opcodes immediately, reserve retired IDs forever, and adopt additive-only evolution rules.
2. **Designing payloads before the frame** — specify sync/resync, length, kind, request ID, CRC, max size, timeout, and retry rules before writing handlers.
3. **Assuming binary solves MCU safety automatically** — enforce hard message/field limits, bounded decode targets, and malformed-input tests.
4. **Dropping request/response semantics during rewrite** — keep a standard envelope with correlation, typed errors, and operation classes.
5. **Trying to migrate all JSON operations at once** — prove one production-relevant vertical slice first, then expand after real traffic validation.

## Implications for Roadmap

Based on research, suggested phase structure:

### Phase 1: Wire Contract Foundation
**Rationale:** Every other decision depends on stable frame and schema invariants; changing these later churns both firmware and backend.
**Delivers:** Protocol spec for frame layout, envelope fields, error model, size limits, retry semantics, schema conventions, numeric opcode allocation, and first operation selection.
**Addresses:** Framing, request correlation, fixed envelope, bounded payloads, forward-compatible schema rules, optional/presence semantics.
**Avoids:** Tag reuse, bad framing, unsafe decode limits, lost request/response semantics.

### Phase 2: Firmware Protocol Core
**Rationale:** Once the contract is frozen, firmware needs the transport and codec substrate before any business handler is useful.
**Delivers:** `src/protocol/` framing, CRC validation, envelope codec, payload codec, `BinaryEndpoint`, feature flagging, golden frame fixtures, and malformed-input MCU tests.
**Uses:** Nanopb, fixed header, CRC, bounded decode rules.
**Implements:** TransportFrame, EnvelopeCodec, MessageCodec, BinaryEndpoint.
**Avoids:** Decoder crashes, mixed concerns, debuggability collapse.

### Phase 3: First Vertical Slice via Adapter
**Rationale:** The project goal is to prove one real end-to-end operation, not broad replacement. This is the milestone that validates the architecture.
**Delivers:** One production-relevant request/response operation wired from backend to firmware and back, through a thin resolver adapter, with typed errors and integration tests.
**Features included:** Deterministic routing, typed payloads, timeout/retry handling, canonical success/error response model.
**Pitfalls to avoid:** Scope creep across multiple operation families, runtime-vs-persisted-state drift, default/presence ambiguity on mutations.

### Phase 4: Backend Gateway and Tooling Hardening
**Rationale:** After the first slice works, backend ergonomics and operational tooling become the leverage points for safe expansion.
**Delivers:** BackendProtocolGateway, correlation tracking, timeout policy, CLI/frame inspector, generated fixtures or descriptor artifacts, compatibility tests across versions.
**Features included:** Optional codegen path, introspectable errors, better observability, drift prevention.
**Pitfalls to avoid:** Firmware/backend schema drift, poor field debugging, incorrect byte-level equality assumptions.

### Phase 5: Expansion and Advanced Protocol Features
**Rationale:** Only after the protocol proves stable should the team expand operation families and optional capabilities.
**Delivers:** Additional operation groups (channel, config, sequence, status, admin), optional capability negotiation, event/log multiplexing, later block-wise transfer if needed.
**Features included:** Differentiators and v2+ capabilities.
**Pitfalls to avoid:** Enum/capability growth surprises, binary-by-obscurity security mistakes, premature optimization for a few saved bytes.

### Phase Ordering Rationale

- Framing and schema invariants come first because both firmware and backend depend on them.
- Firmware substrate comes before handlers because handler tests are low-value if the wire contract is unstable.
- One vertical slice comes before broad rollout because it localizes risk and proves the brownfield adapter strategy.
- Tooling and compatibility gates should be added before multi-operation expansion to prevent cross-version drift.
- Advanced capabilities are last because they depend on a stable request/response foundation and otherwise over-scope the milestone.

### Research Flags

Phases likely needing deeper research during planning:
- **Phase 1:** Final framing choice details (COBS vs exact length-delimited variant), CRC width, and timeout/retry policy should be validated against expected transport behavior.
- **Phase 3:** First operation selection needs domain-specific validation so the slice is meaningful but bounded, especially if it touches mutable config.
- **Phase 5:** Capability negotiation, async event multiplexing, and large-payload transfer need targeted research once real expansion requirements are known.

Phases with standard patterns (skip research-phase):
- **Phase 2:** Layered framing/codec integration and bounded decoder testing are well-documented patterns.
- **Phase 4:** Schema tooling, compatibility checks, and decoder/inspector tooling follow established protobuf/Buf workflows.

## Confidence Assessment

| Area | Confidence | Notes |
|------|------------|-------|
| Stack | HIGH | Strong official-source support for protobuf, Nanopb, Buf, and protobuf runtime choices. |
| Features | HIGH | Table stakes are consistent across embedded protocol norms and align tightly with project scope. |
| Architecture | HIGH | Very strong fit to the current brownfield firmware shape; backend layout is slightly more inferential but still solid. |
| Pitfalls | HIGH | Risks are well-supported by official protobuf/Nanopb guidance and match the repo’s migration context closely. |

**Overall confidence:** HIGH

### Gaps to Address

- **Exact frame format choice:** Research supports explicit framing strongly, but the final on-wire shape still needs implementation-level selection and fixture validation.
- **First operation selection:** The summary recommends one vertical slice, but the specific command should be chosen for bounded payload shape and real business value.
- **Mutation semantics:** If the first operation is a write/config path, presence semantics and apply/reload behavior need explicit planning.
- **Security boundary:** Protocol-layer security redesign is out of scope for MVP, but admin/destructive operations still need threat-model-driven gating before production rollout.
- **Backend language commitment:** TS/Go/Rust are all viable from the schema, but the initial backend/tooling language should be chosen during roadmap planning to narrow implementation tasks.

## Sources

### Primary (HIGH confidence)
- `.planning/research/STACK.md` — protobuf, Nanopb, Buf, transport split, backend runtime recommendations
- `.planning/research/FEATURES.md` — table stakes, differentiators, anti-features, MVP priorities
- `.planning/research/ARCHITECTURE.md` — layering, component boundaries, build order, roadmap split
- `.planning/research/PITFALLS.md` — migration failure modes, prevention strategies, phase warnings
- `.planning/PROJECT.md` — project scope, constraints, brownfield context, milestone boundaries
- https://protobuf.dev/overview/ — protobuf overview and ecosystem direction
- https://protobuf.dev/best-practices/dos-donts/ — evolution rules, tag reuse, API/storage guidance
- https://protobuf.dev/programming-guides/encoding/ — payload encoding and field numbering behavior
- https://protobuf.dev/programming-guides/field_presence/ — explicit presence semantics for updates
- https://jpa.kapsi.fi/nanopb/docs/ — MCU-safe protobuf implementation guidance
- https://jpa.kapsi.fi/nanopb/docs/security.html — untrusted input and size-limit guidance
- https://buf.build/docs/ — linting, breaking checks, and codegen workflow

### Secondary (MEDIUM confidence)
- https://github.com/bufbuild/protobuf-es — TypeScript runtime/codegen direction
- https://protobuf.dev/reference/go/go-generated/ — official Go generation path
- https://protobuf.dev/reference/rust/rust-generated/ — official Rust generation path
- https://github.com/bakercp/PacketSerial — framing prior art for COBS over serial

### Tertiary (LOW confidence)
- None material; the main remaining uncertainty is implementation selection, not source quality.

---
*Research completed: 2026-04-15*
*Ready for roadmap: yes*
