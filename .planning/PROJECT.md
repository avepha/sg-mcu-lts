# SG MCU LTS Protocol Rework

## What This Is

This project is an ESP32 firmware codebase for a controller device that currently exposes a JSON request/response protocol over its serial transport boundary. The next project milestone is to introduce a more efficient custom binary protocol alongside the existing JSON path, starting with one end-to-end request/response operation while keeping the current JSON communication fully operational until replacement is explicitly approved.

## Core Value

The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.

## Requirements

### Validated

- ✓ Firmware boots on ESP32, initializes hardware/runtime state, and runs scheduler-driven tasks — existing
- ✓ Device accepts request/response commands through the current serial transport boundary and routes them by topic and method — existing
- ✓ Firmware supports channel control, configuration/state persistence, sequence control, station polling, and status-style query flows through domain resolvers — existing

### Active

- [ ] Introduce a custom compact binary request/response protocol alongside the current JSON protocol without changing existing JSON behavior
- [ ] Deliver Phase 1 as a fully working new protocol implementation for at least one real end-to-end request/response operation between firmware and a new in-house backend service
- [ ] Design the protocol to improve payload efficiency, MCU parsing cost, reliability/framing, and future schema evolution compared with JSON
- [ ] Define a schema model that can eventually support generated types or SDKs for languages such as Rust, Go, and TypeScript, while keeping SDK generation optional for the first release
- [ ] Create a path to expand the new protocol across the major existing JSON operation families: channel control, configuration reads/writes, sequence operations, status/health queries, and firmware/admin-style commands

### Out of Scope

- Removing, degrading, or changing the current JSON protocol before explicit replacement approval — the existing communication path must remain fully operational during the new protocol rollout
- Full protocol rollout across every existing operation in the first release — the first milestone only needs to prove one real operation end to end
- Mandatory SDK/code generation delivery in the first release — valuable later, but optional for the initial protocol milestone

## Context

The codebase is a brownfield embedded firmware project built with PlatformIO, Arduino, and ESP32-specific runtime APIs. The current transport boundary is centered in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h`, where newline-delimited JSON requests are parsed, validated, routed, and returned as JSON responses. Domain behavior already exists for channel control, sequence execution, EEPROM-backed configuration and models, station polling over Modbus, and runtime logging/event flows.

The protocol change is driven by embedded-system constraints rather than a single issue. The goal is to reduce message overhead, reduce serialization/deserialization cost on the MCU, improve robustness of message framing and parsing, and create a protocol surface that can evolve safely over time. The new backend service will also be developed in-house, but the rollout direction is to run the new protocol in parallel with the existing JSON path until the team explicitly approves replacement.

## Constraints

- **Tech stack**: Must fit the current ESP32 firmware environment in `platformio.ini` and the existing C++/Arduino codebase under `src/` — the protocol design must work within embedded runtime and memory constraints
- **Brownfield architecture**: The new protocol should integrate with the existing request routing and domain model structure centered in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h` — the firmware is not being rewritten from scratch
- **Initial milestone**: Phase 1 must deliver a 100% working new protocol implementation for at least one production-relevant request/response operation while the existing JSON communication remains fully operational and untouched
- **Interoperability**: The protocol should be designed with a schema definition approach that can later support generated types for Rust, Go, and TypeScript — even if code generation is deferred initially
- **Compatibility**: The new protocol must coexist with the current JSON communication for backward compatibility until replacement is explicitly approved

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Introduce a custom compact binary protocol alongside the current JSON path | JSON is not a good fit for embedded payload size, parsing cost, framing robustness, and long-term protocol evolution goals, but the existing JSON interface must stay operational during rollout | — Pending |
| Pair the firmware change with a new in-house backend service | A new backend can adopt the new protocol early while the installed JSON path remains available for backward compatibility | — Pending |
| Start with one fully working end-to-end request/response operation in Phase 1 before broader rollout | The first milestone should prove the protocol design in real firmware/backend usage while demonstrating safe coexistence with the current JSON path | — Pending |
| Keep generated SDK support optional for the first release | Schema and evolution design matter now, but mandatory multi-language SDK generation would slow the initial proving milestone | — Pending |

## Evolution

This document evolves at phase transitions and milestone boundaries.

**After each phase transition** (via `/gsd-transition`):
1. Requirements invalidated? -> Move to Out of Scope with reason
2. Requirements validated? -> Move to Validated with phase reference
3. New requirements emerged? -> Add to Active
4. Decisions to log? -> Add to Key Decisions
5. "What This Is" still accurate? -> Update if drifted

**After each milestone** (via `/gsd-complete-milestone`):
1. Full review of all sections
2. Core Value check -> still the right priority?
3. Audit Out of Scope -> reasons still valid?
4. Update Context with current state

---
*Last updated: 2026-04-16 after coexistence direction update*
