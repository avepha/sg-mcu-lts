# Roadmap: SG MCU LTS Protocol Rework

## Overview

This roadmap moves the firmware from newline-delimited JSON to a compact binary request/response protocol by locking the wire contract first, making the serial boundary safe under malformed traffic, defining a canonical typed schema, and then proving the design with one real backend-to-firmware operation.

## Phases

**Phase Numbering:**
- Integer phases (1, 2, 3): Planned milestone work
- Decimal phases (2.1, 2.2): Urgent insertions (marked with INSERTED)

Decimal phases appear between their surrounding integers in numeric order.

- [ ] **Phase 1: Framing & Runtime Safety** - Establish a safe binary transport boundary with explicit framing, validation, bounds, and retry rules.
- [ ] **Phase 2: Request/Response Envelope** - Standardize the compact envelope used to route, correlate, and report binary operations.
- [ ] **Phase 3: Typed Schema Contract** - Define the canonical payload schema and evolution rules for future-safe protocol growth.
- [ ] **Phase 4: First End-to-End Binary Operation** - Prove the protocol with one real backend-to-firmware request/response flow.

## Phase Details

### Phase 1: Framing & Runtime Safety
**Goal**: Firmware can safely receive and reject binary traffic over the existing serial boundary without ambiguity or runtime instability.
**Depends on**: Nothing (first phase)
**Requirements**: TRAN-01, TRAN-02, TRAN-03, RUNT-01, RUNT-02
**Success Criteria** (what must be TRUE):
  1. A backend client can send framed binary requests and the device can detect exact frame boundaries without relying on newline-delimited JSON.
  2. Corrupted packets are rejected with integrity failure handling instead of reaching operation handlers.
  3. Oversize requests or responses fail predictably at defined size limits rather than consuming unbounded memory.
  4. Malformed frames or payloads are rejected without crashing the firmware or corrupting runtime state.
  5. Backend implementers have a defined timeout and retry contract for request handling on the wire.
**Plans**: TBD

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

### Phase 4: First End-to-End Binary Operation
**Goal**: One production-relevant operation works end to end between the new backend and firmware using the new protocol.
**Depends on**: Phase 3
**Requirements**: SLCE-01, SLCE-02
**Success Criteria** (what must be TRUE):
  1. The backend can encode one real production-relevant request, send it to firmware, and decode the typed response successfully.
  2. The firmware can execute that operation through the new protocol path and return the expected success response.
  3. The backend can detect and handle protocol-level failures for that operation using the new binary contract.
**Plans**: TBD

## Progress

**Execution Order:**
Phases execute in numeric order: 1 → 2 → 3 → 4

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. Framing & Runtime Safety | 0/TBD | Not started | - |
| 2. Request/Response Envelope | 0/TBD | Not started | - |
| 3. Typed Schema Contract | 0/TBD | Not started | - |
| 4. First End-to-End Binary Operation | 0/TBD | Not started | - |
