# Requirements: SG MCU LTS Protocol Rework

**Defined:** 2026-04-15
**Core Value:** The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.

## v1 Requirements

Requirements for the initial protocol milestone.

### Transport

- [x] **TRAN-01**: Device accepts binary requests using explicit packet framing with unambiguous frame boundaries
- [x] **TRAN-02**: Device validates each frame with CRC/FCS and rejects corrupted packets safely
- [x] **TRAN-03**: Device enforces bounded request and response frame size limits and fails oversize traffic predictably

### Envelope

- [x] **ENVP-01**: Every request and response includes a protocol version field
- [x] **ENVP-02**: Device routes requests using compact numeric operation identifiers instead of JSON topic/method strings
- [x] **ENVP-03**: Every request and response includes a small correlation/request ID for matching replies to requests
- [x] **ENVP-04**: Device returns canonical machine-readable status and error codes for protocol and operation failures

### Schema

- [x] **SCMA-01**: Request and response payloads use typed binary schemas instead of ad hoc byte layouts
- [x] **SCMA-02**: The protocol defines additive-only field evolution rules and reserves retired identifiers from reuse
- [x] **SCMA-03**: The schema can represent optional/presence semantics where omitted differs from zero, false, or empty
- [x] **SCMA-04**: The schema source is structured so Rust, Go, and TypeScript SDK/code generation can be added later

### Runtime

- [x] **RUNT-01**: Firmware defines timeout behavior for request handling and documents retry expectations for the backend
- [x] **RUNT-02**: Firmware safely rejects malformed frames and malformed payloads without crashing or corrupting runtime state

### Compatibility

- [x] **COMP-01**: The new protocol coexists with the current JSON communication, and enabling the new path does not break or change existing JSON behavior before explicit replacement approval

### Vertical Slice

- [x] **SLCE-01**: The system implements one real production-relevant request/response operation end to end using the new protocol
- [x] **SLCE-02**: The new in-house backend can encode a request, send it to firmware, decode the response, and handle protocol-level failures for that first operation

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
| Removing or changing the current JSON communication in the first milestone | The new protocol must ship alongside the existing JSON path for backward compatibility until replacement is explicitly approved |
| Full replacement of all existing JSON operations in the first milestone | Initial scope is a proving milestone for one real operation, not a complete rollout |
| Mandatory multi-language SDK generation in the first milestone | The schema should be codegen-ready now, but full SDK delivery would slow the initial protocol proof |
| Protocol-layer authentication or crypto redesign | Important only if a later threat model requires it; not core to this milestone |

## Traceability

Which phases cover which requirements. Updated during roadmap creation.

| Requirement | Phase | Status |
|-------------|-------|--------|
| TRAN-01 | Phase 1 | Complete |
| TRAN-02 | Phase 1 | Complete |
| TRAN-03 | Phase 1 | Complete |
| ENVP-01 | Phase 1 | Complete |
| ENVP-02 | Phase 1 | Complete |
| ENVP-03 | Phase 1 | Complete |
| ENVP-04 | Phase 1 | Complete |
| SCMA-01 | Phase 1 | Complete |
| SCMA-02 | Phase 1 | Complete |
| SCMA-03 | Phase 1 | Complete |
| SCMA-04 | Phase 1 | Complete |
| RUNT-01 | Phase 1 | Complete |
| RUNT-02 | Phase 1 | Complete |
| COMP-01 | Phase 1 | Complete |
| SLCE-01 | Phase 1 | Complete |
| SLCE-02 | Phase 1 | Complete |

**Coverage:**
- v1 requirements: 16 total
- Mapped to phases: 16
- Unmapped: 0 ✓

---
*Requirements defined: 2026-04-15*
*Last updated: 2026-04-16 after Phase 1 planning coverage was finalized*
