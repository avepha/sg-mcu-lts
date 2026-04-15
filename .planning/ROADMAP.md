# Roadmap: SG MCU LTS Protocol Rework

## Overview

This roadmap introduces a compact binary request/response protocol alongside the existing newline-delimited JSON path. Phase 1 must deliver a fully working new protocol implementation for one real backend-to-firmware operation while keeping the current JSON communication fully operational and untouched until replacement is explicitly approved.

## Phases

**Phase Numbering:**
- Integer phases (1, 2, 3): Planned milestone work
- Decimal phases (2.1, 2.2): Urgent insertions (marked with INSERTED)

Decimal phases appear between their surrounding integers in numeric order.

- [ ] **Phase 1: Coexistent Binary Vertical Slice** - Deliver one fully working binary request/response operation end to end while keeping the current JSON communication fully operational.
- [ ] **Phase 2: Request/Response Envelope** - Standardize the compact envelope used to route, correlate, and report binary operations.
- [ ] **Phase 3: Typed Schema Contract** - Define the canonical payload schema and evolution rules for future-safe protocol growth.
- [ ] **Phase 4: Broader Protocol Rollout** - Expand the new protocol beyond the first proven operation once coexistence is stable and replacement remains explicitly controlled.

## Phase Details

### Phase 1: Coexistent Binary Vertical Slice
**Goal**: Firmware ships a fully working new protocol path for one production-relevant operation while the existing JSON communication continues to work unchanged.
**Depends on**: Nothing (first phase)
**Requirements**: TRAN-01, TRAN-02, TRAN-03, ENVP-01, ENVP-02, ENVP-03, ENVP-04, SCMA-01, SCMA-02, SCMA-03, SCMA-04, RUNT-01, RUNT-02, COMP-01, SLCE-01, SLCE-02
**Success Criteria** (what must be TRUE):
  1. A backend client can send one real framed binary request, the device can execute it, and the client can decode the expected typed response successfully.
  2. The current JSON request/response communication remains fully operational and behaviorally unchanged while the new protocol path is enabled.
  3. Corrupted, oversize, and malformed binary traffic is rejected safely without crashing the firmware or corrupting runtime state.
  4. The new protocol path includes explicit framing, bounded payload handling, correlation, versioning, routing, and canonical failure reporting for the delivered operation.
  5. Backend implementers have a defined timeout and retry contract for request handling on the wire.
**Plans**: 3 plans

Plans:
- [ ] 01-01-PLAN.md — Define the binary envelope/schema contract and native protocol test harness
- [ ] 01-02-PLAN.md — Implement framed binary transport and route operation `0x01` through existing info logic
- [ ] 01-03-PLAN.md — Publish backend wire docs and finish coexistence/malformed regression coverage

### Phase 2: Request/Response Envelope
**Goal**: Firmware and backend share a stable compact envelope for versioning, routing, correlation, and canonical failure reporting.
**Depends on**: Phase 1
**Requirements**: ENVP-01, ENVP-02, ENVP-03, ENVP-04
**Success Criteria** (what must be TRUE):
  1. Every binary request and response exposes a protocol version that can be checked before payload decoding.
  2. The device can route requests through compact numeric operation identifiers rather than JSON topic and method strings.
  3. A backend client can match each response to its originating request using the protocol request ID.
  4. Protocol and operation failures return canonical machine-readable status and error codes instead of ad hoc text handling.
**Plans**: TBD

### Phase 3: Typed Schema Contract
**Goal**: Payloads are defined by one typed, evolvable schema source that fits MCU constraints now and future code generation later.
**Depends on**: Phase 2
**Requirements**: SCMA-01, SCMA-02, SCMA-03, SCMA-04
**Success Criteria** (what must be TRUE):
  1. Request and response payloads are encoded from typed binary schemas instead of per-command custom byte layouts.
  2. Optional fields can be omitted distinctly from sending zero, false, or empty values.
  3. Additive schema updates can be introduced without reusing retired field or enum identifiers.
  4. The canonical schema source is organized so Rust, Go, and TypeScript SDK generation can be added later without redefining the protocol contract.
**Plans**: TBD

### Phase 4: Broader Protocol Rollout
**Goal**: Expand the new protocol beyond the first proven operation while preserving the coexistence requirement with the current JSON path.
**Depends on**: Phase 3
**Requirements**: PROT-03
**Success Criteria** (what must be TRUE):
  1. The team has a defined path to expand the new protocol across the major existing JSON operation families after the first coexistent implementation is proven.
  2. Additional rollout work preserves backward compatibility with the current JSON communication until replacement is explicitly approved.
  3. Broader operation coverage builds on the same framing, envelope, and schema rules proven in earlier phases.
**Plans**: TBD

## Progress

**Execution Order:**
Phases execute in numeric order: 1 → 2 → 3 → 4

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. Coexistent Binary Vertical Slice | 0/TBD | Not started | - |
| 2. Request/Response Envelope | 0/TBD | Not started | - |
| 3. Typed Schema Contract | 0/TBD | Not started | - |
| 4. Broader Protocol Rollout | 0/TBD | Not started | - |
