# Requirements: SG MCU LTS Protocol Rework

**Defined:** 2026-04-15
**Core Value:** The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.

## v1 Requirements

Requirements for the initial protocol milestone.

### Transport

- [ ] **TRAN-01**: Device accepts binary requests using explicit packet framing with unambiguous frame boundaries
- [ ] **TRAN-02**: Device validates each frame with CRC/FCS and rejects corrupted packets safely
- [ ] **TRAN-03**: Device enforces bounded request and response frame size limits and fails oversize traffic predictably

### Envelope

- [ ] **ENVP-01**: Every request and response includes a protocol version field
- [ ] **ENVP-02**: Device routes requests using compact numeric operation identifiers instead of JSON topic/method strings
- [ ] **ENVP-03**: Every request and response includes a small correlation/request ID for matching replies to requests
- [ ] **ENVP-04**: Device returns canonical machine-readable status and error codes for protocol and operation failures

### Schema

- [ ] **SCMA-01**: Request and response payloads use typed binary schemas instead of ad hoc byte layouts
- [ ] **SCMA-02**: The protocol defines additive-only field evolution rules and reserves retired identifiers from reuse
- [ ] **SCMA-03**: The schema can represent optional/presence semantics where omitted differs from zero, false, or empty
- [ ] **SCMA-04**: The schema source is structured so Rust, Go, and TypeScript SDK/code generation can be added later

### Runtime

- [ ] **RUNT-01**: Firmware defines timeout behavior for request handling and documents retry expectations for the backend
- [ ] **RUNT-02**: Firmware safely rejects malformed frames and malformed payloads without crashing or corrupting runtime state

### Vertical Slice

- [ ] **SLCE-01**: The system implements one real production-relevant request/response operation end to end using the new protocol
- [ ] **SLCE-02**: The new in-house backend can encode a request, send it to firmware, decode the response, and handle protocol-level failures for that first operation

## v2 Requirements

### Protocol Expansion

- **PROT-01**: Device and backend support capability negotiation or hello-handshake semantics for versioned feature discovery
- **PROT-02**: Protocol supports asynchronous events or notifications on the same wire format as request/response traffic
- **PROT-03**: The new protocol expands to cover the major existing JSON operation families across channel, configuration, sequence, status, and admin flows

### Tooling and Transfer

- **TOOL-01**: The project ships generated SDKs or typed client libraries for Rust, Go, and TypeScript from the canonical schema
- **TOOL-02**: Protocol supports block-wise or streaming transfer for large request or response bodies
- **TOOL-03**: Protocol can distinguish transport acknowledgment from business response for long-running operations

## Out of Scope

| Feature | Reason |
|---------|--------|
| Backward compatibility with the current JSON backend | A new in-house backend will be built, so migration compatibility is not required |
| Full replacement of all existing JSON operations in the first milestone | Initial scope is a proving milestone for one real operation, not a complete rollout |
| Mandatory multi-language SDK generation in the first milestone | The schema should be codegen-ready now, but full SDK delivery would slow the initial protocol proof |
| Protocol-layer authentication or crypto redesign | Important only if a later threat model requires it; not core to this milestone |

## Traceability

Which phases cover which requirements. Updated during roadmap creation.

| Requirement | Phase | Status |
|-------------|-------|--------|
| TRAN-01 | Phase 1 | Pending |
| TRAN-02 | Phase 1 | Pending |
| TRAN-03 | Phase 1 | Pending |
| ENVP-01 | Phase 2 | Pending |
| ENVP-02 | Phase 2 | Pending |
| ENVP-03 | Phase 2 | Pending |
| ENVP-04 | Phase 2 | Pending |
| SCMA-01 | Phase 3 | Pending |
| SCMA-02 | Phase 3 | Pending |
| SCMA-03 | Phase 3 | Pending |
| SCMA-04 | Phase 3 | Pending |
| RUNT-01 | Phase 1 | Pending |
| RUNT-02 | Phase 1 | Pending |
| SLCE-01 | Phase 4 | Pending |
| SLCE-02 | Phase 4 | Pending |

**Coverage:**
- v1 requirements: 15 total
- Mapped to phases: 15
- Unmapped: 0 ✓

---
*Requirements defined: 2026-04-15*
*Last updated: 2026-04-15 after roadmap creation*
