# SG MCU LTS Protocol Rework

## What This Is

This project is an ESP32 firmware codebase for a controller device that currently exposes a JSON request/response protocol over its serial transport boundary. The next project milestone is to design and implement a more efficient custom binary protocol for communication with a new in-house backend, starting with one end-to-end request/response operation and expanding from there.

## Core Value

The firmware must support a compact, reliable, evolvable request/response protocol that is materially better suited to embedded constraints than the current JSON API.

## Requirements

### Validated

- ✓ Firmware boots on ESP32, initializes hardware/runtime state, and runs scheduler-driven tasks — existing
- ✓ Device accepts request/response commands through the current serial transport boundary and routes them by topic and method — existing
- ✓ Firmware supports channel control, configuration/state persistence, sequence control, station polling, and status-style query flows through domain resolvers — existing

### Active

- [ ] Replace the current JSON-centric protocol direction with a custom compact binary request/response protocol designed for embedded system constraints
- [ ] Prove the new protocol with at least one real end-to-end request/response operation between firmware and a new in-house backend service
- [ ] Design the protocol to improve payload efficiency, MCU parsing cost, reliability/framing, and future schema evolution compared with JSON
- [ ] Define a schema model that can eventually support generated types or SDKs for languages such as Rust, Go, and TypeScript, while keeping SDK generation optional for the first release
- [ ] Create a path to expand the new protocol across the major existing JSON operation families: channel control, configuration reads/writes, sequence operations, status/health queries, and firmware/admin-style commands

### Out of Scope

- Maintaining backward compatibility with the current JSON backend contract — a new backend will be built, so compatibility is not required
- Full protocol rollout across every existing operation in the first release — the first milestone only needs to prove one real operation end to end
- Mandatory SDK/code generation delivery in the first release — valuable later, but optional for the initial protocol milestone

## Context

The codebase is a brownfield embedded firmware project built with PlatformIO, Arduino, and ESP32-specific runtime APIs. The current transport boundary is centered in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h`, where newline-delimited JSON requests are parsed, validated, routed, and returned as JSON responses. Domain behavior already exists for channel control, sequence execution, EEPROM-backed configuration and models, station polling over Modbus, and runtime logging/event flows.

The protocol change is driven by embedded-system constraints rather than a single issue. The goal is to reduce message overhead, reduce serialization/deserialization cost on the MCU, improve robustness of message framing and parsing, and create a protocol surface that can evolve safely over time. Because the new backend service will also be developed in-house, the team has flexibility to make a clean protocol break instead of carrying JSON compatibility forward.

## Constraints

- **Tech stack**: Must fit the current ESP32 firmware environment in `platformio.ini` and the existing C++/Arduino codebase under `src/` — the protocol design must work within embedded runtime and memory constraints
- **Brownfield architecture**: The new protocol should integrate with the existing request routing and domain model structure centered in `src/main.cpp`, `src/deviceEndpoint.h`, and `src/combineResolvers.h` — the firmware is not being rewritten from scratch
- **Initial milestone**: First release only needs at least one production-relevant request/response operation implemented end to end — this is a proving milestone, not a full replacement
- **Interoperability**: The protocol should be designed with a schema definition approach that can later support generated types for Rust, Go, and TypeScript — even if code generation is deferred initially
- **Compatibility**: No backward compatibility requirement with the old JSON backend — this allows protocol and backend decisions to optimize for the new design

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Build a custom compact binary protocol instead of continuing with JSON | JSON is not a good fit for embedded payload size, parsing cost, framing robustness, and long-term protocol evolution goals | — Pending |
| Pair the firmware change with a new in-house backend service | A new backend removes migration pressure and allows protocol changes without breakage concerns | — Pending |
| Start with one end-to-end request/response operation before broader rollout | The first milestone should prove the protocol design in real firmware/backend usage before replacing every operation | — Pending |
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
*Last updated: 2026-04-15 after initialization*
