# Project State

## Project Reference

See: .planning/PROJECT.md (updated 2026-04-16)

**Core value:** The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.
**Current focus:** Phase 1 - Coexistent Binary Vertical Slice

## Current Position

Phase: 1 of 4 (Coexistent Binary Vertical Slice)
Plan: 0 of 3 in current phase
Status: Planned
Last activity: 2026-04-16 — Refined Phase 1 plans with task read-first and acceptance criteria details for execution readiness

Progress: [░░░░░░░░░░] 0%

## Performance Metrics

**Velocity:**
- Total plans completed: 0
- Average duration: 0 min
- Total execution time: 0.0 hours

**By Phase:**

| Phase | Plans | Total | Avg/Plan |
|-------|-------|-------|----------|
| - | - | - | - |

**Recent Trend:**
- Last 5 plans: none
- Trend: Stable

## Accumulated Context

### Decisions

Decisions are logged in PROJECT.md Key Decisions table.
Recent decisions affecting current work:

- [Phase 1]: Deliver one fully working binary operation that safely coexists with the current JSON communication.
- [Phase 2]: Keep routing, versioning, correlation, and status in a compact fixed envelope.
- [Phase 3]: Use one canonical typed schema source with additive-only evolution discipline.
- [Phase 4]: Expand the protocol after the coexistent Phase 1 implementation proves stable and replacement is explicitly approved.
- [Workflow]: Uploading/flashing firmware to the physical ESP32 is a manual user-owned checkpoint because it requires device button presses, so normal developer execution and validation must not assume or automate board flashing.

### Pending Todos

None yet.

### Blockers/Concerns

- First operation selection still needs bounded business validation for the Phase 1 coexistent delivery milestone.
- Final framing choice and CRC width need confirmation during Phase 1 planning.
- Real-device upload/flash validation is available only as an explicit manual checkpoint and is outside the normal automated developer loop.

## Deferred Items

| Category | Item | Status | Deferred At |
|----------|------|--------|-------------|
| Protocol Expansion | Handshake, async events, streaming transfer, and broad operation-family rollout | Deferred to v2 | 2026-04-15 |

## Session Continuity

Last session: 2026-04-16 00:00
Stopped at: Manual hardware flashing constraint recorded in state and Phase 1 validation guidance
Resume file: None
