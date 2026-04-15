# Domain Pitfalls

**Domain:** Embedded firmware + backend migration from JSON to a custom schema-driven binary request/response protocol
**Researched:** 2026-04-15

## Critical Pitfalls

Mistakes here commonly force protocol breaks, firmware rewrites, or field failures.

### Pitfall 1: Treating schema numbers and message layout as refactorable
**What goes wrong:** Field/tag numbers, enum values, or message meanings get renumbered or reused during iteration because the protocol is still “internal.”
**Why it happens:** Teams port JSON field names into binary, then later try to “clean up” numbering once the wire format starts feeling permanent.
**Consequences:** Old firmware and backend builds silently misread packets; stored payloads, logs, retries, or replayed commands decode incorrectly.
**Warning signs:**
- Schema reviews talk about “renumbering for neatness” or reusing deleted field IDs.
- No `reserved`/retired ID policy exists.
- The team assumes simultaneous rollout will prevent skew.
**Prevention:**
- Freeze field/tag numbers the first time any payload leaves a developer machine.
- Reserve deleted field numbers and enum values permanently.
- Add schema evolution rules to the repo: additive changes only, no tag reuse, no semantic reuse.
- Keep a changelog of message and enum compatibility decisions.
**Phase to address:** Phase 1 - protocol schema foundation

### Pitfall 2: Designing the payload but not the frame
**What goes wrong:** Teams replace JSON with binary payloads but leave transport framing underspecified: no max length, weak resynchronization, no checksum/CRC, no message type envelope, or no correlation ID.
**Why it happens:** JSON newline framing previously hid message boundaries and human readability masked transport defects.
**Consequences:** Serial noise, truncation, partial reads, or concatenated frames become decoder crashes, deadlocks, or phantom commands.
**Warning signs:**
- Discussions focus on message fields, not frame format.
- The decoder assumes a complete buffer already exists.
- There is no explicit oversize-frame policy.
- Error handling does not distinguish bad CRC, bad length, unsupported type, and decode failure.
**Prevention:**
- Specify transport envelope separately from schema: sync/resync strategy, length prefix, message kind, correlation ID, flags, checksum/CRC, and max frame size.
- Make oversize/truncated/corrupt frame handling explicit and testable.
- Use a bounded streaming/state-machine decoder instead of assembling unbounded payloads first.
- Define exact timeout and retry behavior at the frame layer.
**Phase to address:** Phase 1 - transport/framing design

### Pitfall 3: Assuming binary automatically fixes MCU resource risk
**What goes wrong:** The new protocol still allocates dynamically, trusts sender-provided lengths, or decodes into oversized temporary buffers.
**Why it happens:** Teams focus on smaller wire size and forget that parsing strategy, not just format, drives heap churn and crash risk.
**Consequences:** The firmware keeps the same crash class it already has today: buffer overruns, heap fragmentation, watchdog stalls, or stack exhaustion.
**Warning signs:**
- Decoder entrypoints still copy whole frames into fixed arrays without bounds checks.
- Generated or hand-written decode paths allocate variable-size buffers on heap/stack.
- Maximum string/bytes/repeated lengths are not defined in schema options or protocol docs.
**Prevention:**
- Define hard per-message and per-field limits before implementation.
- Prefer bounded/static decode targets on firmware.
- Reject frames before decode when declared length exceeds policy.
- Add malformed-input tests for oversize, truncated, duplicated, and interleaved fields.
**Phase to address:** Phase 1 - decoder safety and limits

### Pitfall 4: Losing request/response semantics during the JSON-to-binary rewrite
**What goes wrong:** The binary protocol captures data shape but drops operational semantics the JSON API already relies on: request IDs, method/topic routing, typed errors, idempotency expectations, and admin vs normal command classes.
**Why it happens:** Teams think “binary protocol” means only serialization, not API contract redesign.
**Consequences:** Backends cannot correlate responses, retries become unsafe, router logic becomes ad hoc, and debugging behavior regresses.
**Warning signs:**
- The schema has payload messages but no standard envelope.
- Error responses are described as “just disconnect” or “generic failure code.”
- Command classes with different risk levels share the same unrestricted path.
**Prevention:**
- Define a stable envelope for all exchanges: request ID, operation ID, status/error code, optional diagnostics, and capability/version metadata.
- Preserve or explicitly redesign routing semantics instead of letting them emerge from implementation.
- Separate mutating/admin operations from safe query flows in the protocol itself.
**Phase to address:** Phase 1 - API contract design

### Pitfall 5: Presence/default-value ambiguity corrupting partial updates
**What goes wrong:** “Field absent” and “field set to zero/false/empty” are treated the same, especially in configuration writes.
**Why it happens:** Binary formats often optimize away default values; teams port JSON patch-like behavior without explicitly modeling presence.
**Consequences:** Legitimate zero values cannot be set, EEPROM-backed config gets unintentionally cleared, and firmware/backend patch semantics drift.
**Warning signs:**
- Update messages reuse the same schema as read models.
- Booleans and numeric zero values are used as implicit “not provided.”
- Teams rely on default comparisons instead of explicit presence bits or field masks.
**Prevention:**
- Model patch/update messages separately from full state messages.
- Use explicit presence semantics (`optional`, oneof, field mask, or equivalent) for mutable config.
- Write tests specifically for setting values to 0, false, empty string, and empty list.
- Document merge semantics for each mutation.
**Phase to address:** Phase 1 - mutation schema design

### Pitfall 6: Backend and firmware drifting because the schema is not the source of truth
**What goes wrong:** Firmware structs, backend structs, and wire docs evolve separately; generated code is optional, but disciplined compatibility checks are also missing.
**Why it happens:** Teams want to move fast with hand-written codecs during the first milestone.
**Consequences:** One side accepts fields the other ignores, enum handling diverges, and bugs only appear during cross-version integration.
**Warning signs:**
- There is no single schema repo or compatibility gate.
- Sample payloads are maintained by hand.
- Integration tests cover only same-commit firmware and backend builds.
**Prevention:**
- Keep one canonical schema definition, even if code generation is deferred.
- Generate at least descriptors/test fixtures/golden payloads from that schema.
- Add compatibility tests across firmware/backend versions and malformed payload corpora.
- Version the schema independently from implementation releases.
**Phase to address:** Phase 2 - tooling and CI

### Pitfall 7: Using non-canonical serialization as an integrity or equality primitive
**What goes wrong:** Teams hash, sign, diff, cache-key, or deduplicate messages based on raw serialized bytes.
**Why it happens:** Binary payloads look compact and deterministic enough to compare directly.
**Consequences:** Different valid serializations of the same logical message break signatures, dedupe, replay detection, or cache behavior.
**Warning signs:**
- Specs say “compare payload bytes” or “CRC the serialized command object” without defining canonicalization.
- Cross-language encoders are expected to emit identical bytes for logically equivalent messages.
**Prevention:**
- If integrity is needed, checksum/sign the transport frame actually sent, not an abstract message reconstructed elsewhere.
- If logical equality is needed, compare normalized semantic fields, not serializer output.
- Only depend on byte stability if the protocol explicitly defines canonical encoding and tests it.
**Phase to address:** Phase 1 - integrity and interoperability rules

### Pitfall 8: Cutting over too broadly instead of proving one vertical slice
**What goes wrong:** Teams try to replace every JSON operation family at once.
**Why it happens:** Once a binary protocol exists, broad migration feels cheaper than maintaining two paths temporarily.
**Consequences:** Too many unknowns move at once: framing, schema, backend, router integration, tooling, ops, and runtime behavior. Failures become hard to localize.
**Warning signs:**
- Early milestones include channel control, configuration, sequence, status, and admin commands together.
- There is no “one production-relevant operation” proving path.
- Success criteria are phrased as “JSON replacement” instead of “validated slice.”
**Prevention:**
- Pick one high-value round trip first.
- Validate decode safety, end-to-end correlation, retries, observability, and rollback on that single path.
- Expand only after the schema and frame rules survive real traffic.
**Phase to address:** Phase 2 - first end-to-end rollout

### Pitfall 9: Binary-by-obscurity replacing real security
**What goes wrong:** Teams assume a custom binary protocol is “harder to abuse” than JSON and therefore delay authentication, authorization, and dangerous-command gating.
**Why it happens:** Readability drops, so risk feels lower.
**Consequences:** The same unauthenticated control surface remains, but with worse observability and potentially more dangerous parser bugs.
**Warning signs:**
- Security requirements are deferred because the backend is in-house.
- Destructive commands remain reachable over the same transport.
- Threat modeling stops at “not human-readable anymore.”
**Prevention:**
- Treat every frame as untrusted input.
- Add authentication or a trusted physical/service-mode boundary before enabling destructive mutations in production.
- Separate admin operations in the protocol and gate them independently.
**Phase to address:** Phase 1 for threat model, Phase 3 for production hardening

## Moderate Pitfalls

### Pitfall 1: Debuggability collapses after the switch
**What goes wrong:** Engineers lose the ability to inspect traffic quickly, and field debugging slows dramatically.
**Warning signs:**
- No CLI decoder, frame inspector, or annotated hexdump tool exists.
- Logs only say “decode failed.”
**Prevention:**
- Ship a developer decoder/encoder tool with the schema.
- Log frame-level reason codes and bounded metadata.
- Keep golden request/response examples in the repo.
**Phase to address:** Phase 2 - developer tooling

### Pitfall 2: Runtime state and persisted state diverge after binary mutations
**What goes wrong:** A binary config mutation updates EEPROM/schema state but not the live scheduler/core objects.
**Warning signs:**
- Success responses are returned before runtime reload/rebuild is complete.
- Reboot is required to observe some config writes.
**Prevention:**
- Define for each mutation whether it is immediate, deferred, or reboot-required.
- Keep apply/validate/persist/reload stages explicit.
- Add tests that verify live runtime state after mutation, not just persisted bytes.
**Phase to address:** Phase 2 - domain integration

### Pitfall 3: Enum and capability growth is not planned
**What goes wrong:** Backends assume exhaustive enums or fixed feature sets; old firmware mishandles new values.
**Warning signs:**
- No `UNSPECIFIED`/unknown enum policy.
- Backend switch statements treat unknown enum as impossible.
**Prevention:**
- Reserve enum evolution rules now.
- Add explicit capability/version negotiation in the envelope.
- Make unknown enum handling non-fatal where possible.
**Phase to address:** Phase 1 - schema conventions

## Minor Pitfalls

### Pitfall 1: Over-optimizing for minimum bytes too early
**What goes wrong:** Teams choose awkward encodings or hyper-packed fields that save a few bytes but make evolution and debugging much worse.
**Prevention:**
- Optimize the frame and hot-path fields first.
- Prefer clear schema boundaries over bit-level cleverness in milestone one.
**Phase to address:** Phase 1 - design review discipline

### Pitfall 2: Reusing the same message for wire API and long-term storage
**What goes wrong:** Transport concerns and persistence concerns get coupled; later changes to one break the other.
**Prevention:**
- Keep wire messages separate from EEPROM/internal storage models.
- Add an explicit translation layer at the firmware boundary.
**Phase to address:** Phase 2 - integration architecture

## Phase-Specific Warnings

| Phase Topic | Likely Pitfall | Mitigation |
|-------------|---------------|------------|
| Protocol framing/spec | Payload defined before frame behavior | Write transport envelope spec first: max size, sync, CRC, type, reqId, timeout, retry |
| Schema conventions | Tag reuse, enum reuse, poor presence semantics | Adopt additive-only schema rules, reserved IDs, explicit presence for mutations |
| Firmware decoder | Oversize/truncated inputs still crash MCU | Implement bounded streaming decode, size guards, malformed-input tests |
| First vertical slice | Too many operations migrate at once | Prove one end-to-end command first with golden fixtures and real backend integration |
| Backend integration | Firmware/backend drift | One canonical schema, generated fixtures, version compatibility tests |
| Domain rollout | Persisted config changes do not update live runtime | Define apply/reload semantics per command and test runtime effects |
| Production hardening | Binary mistaken for security | Add auth/gating, observability, and negative testing before enabling admin mutations |

## Sources

- HIGH: Protobuf proto3 guide - schema evolution, unknown fields, wire safety: https://protobuf.dev/programming-guides/proto3/
- HIGH: Protobuf encoding guide - TLV structure, packed fields, last-one-wins, field order: https://protobuf.dev/programming-guides/encoding/
- HIGH: Protobuf field presence note - explicit vs implicit presence and patch semantics: https://protobuf.dev/programming-guides/field_presence/
- HIGH: Protobuf best practices - tag reuse, enum rules, storage-vs-API split, serialization caveats: https://protobuf.dev/best-practices/dos-donts/
- HIGH: Protobuf serialization-not-canonical note - raw byte instability: https://protobuf.dev/programming-guides/serialization-not-canonical/
- HIGH: Nanopb concepts - framing responsibility, max_size/max_count constraints, callback caveats: https://jpa.kapsi.fi/nanopb/docs/concepts.html
- HIGH: Nanopb security model - untrusted input handling, max-size and stream limits: https://jpa.kapsi.fi/nanopb/docs/security.html
- MEDIUM: Project context from `.planning/PROJECT.md`, `.planning/codebase/CONCERNS.md`, `.planning/codebase/ARCHITECTURE.md` informing brownfield-specific warnings about serial framing, bounded parsing, router semantics, EEPROM-backed configuration, and runtime reload risk.
