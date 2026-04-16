---
phase: 1
slug: coexistent-binary-vertical-slice
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-04-16
---

# Phase 1 — Validation Strategy

> Per-phase validation contract for feedback sampling during execution.

---

## Test Infrastructure

| Property | Value |
|----------|-------|
| **Framework** | Host-side standard C++ `end-to-end` tests via PlatformIO native env + firmware compile verification |
| **Config file** | `platformio.ini` |
| **Quick run command** | `rtk pio test -e native_protocol` |
| **Full suite command** | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` |
| **Estimated runtime** | ~35 seconds |

---

## Sampling Rate

- **After every task commit:** Run `rtk pio test -e native_protocol`
- **After every plan wave:** Run `rtk pio test -e native_protocol && rtk pio run -e v2_dev`
- **Before `/gsd-verify-work`:** Full suite must be green
- **Primary developer path:** host-side tests and firmware compile only; board upload/flash is not part of the normal loop
- **Max feedback latency:** 45 seconds

---

## Per-Task Verification Map

| Task ID | Plan | Wave | Requirement | Threat Ref | Secure Behavior | Test Type | Automated Command | File Exists | Status |
|---------|------|------|-------------|------------|-----------------|-----------|-------------------|-------------|--------|
| 1-01-01 | 01 | 1 | SCMA-01, SCMA-02, SCMA-03, SCMA-04, ENVP-01, ENVP-02, ENVP-03, ENVP-04 | T-01-01 | Canonical schema/envelope contract is versioned and bounded | native end-to-end/unit | `rtk pio test -e native_protocol` | ❌ W0 | ⬜ pending |
| 1-01-02 | 01 | 1 | TRAN-02, TRAN-03 | T-01-02 | Fast tests reject CRC and oversize inputs before firmware integration | native end-to-end/unit | `rtk pio test -e native_protocol` | ❌ W0 | ⬜ pending |
| 1-02-01 | 02 | 2 | TRAN-01, TRAN-02, TRAN-03, RUNT-02 | T-01-03 | Binary frame parser accepts valid frames and safely rejects malformed traffic | host native end-to-end/unit + build | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |
| 1-02-02 | 02 | 2 | ENVP-01, ENVP-02, ENVP-03, ENVP-04, COMP-01, SLCE-01 | T-01-04 | Binary transport coexists with JSON and routes `info.query` through existing domain logic | host native end-to-end/integration + build | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |
| 1-03-01 | 03 | 3 | RUNT-01, SLCE-02 | T-01-05 | Backend docs encode timeout/retry and wire examples unambiguously | docs + native end-to-end fixture | `rtk pio test -e native_protocol` | ✅ | ⬜ pending |
| 1-03-02 | 03 | 3 | COMP-01, RUNT-02 | T-01-06 | Regression suite proves JSON discrimination and malformed frame handling remain safe | host native end-to-end/integration + build | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |

*Status: ⬜ pending · ✅ green · ❌ red · ⚠️ flaky*

---

## Wave 0 Requirements

- [ ] `test/end-to-end/test_binary_protocol.cpp` — end-to-end frame/schema regression scaffold
- [ ] `platformio.ini` — `native_protocol` test environment

---

## Manual-Only Verifications

| Behavior | Requirement | Why Manual | Test Instructions |
|----------|-------------|------------|-------------------|
| Uploading/flashing firmware to the physical ESP32 is a manual user-owned checkpoint and not part of the normal developer loop | COMP-01, RUNT-02 | The device requires a physical button-push sequence during upload, so flashing cannot be assumed or automated in routine execution/validation | When explicit hardware validation is requested, the user performs the button-push upload flow for `v2_dev`, then run one JSON request and one binary request from the backend harness and confirm both respond on the same transport boundary |
| Serial monitor sanity check on real ESP32 hardware is optional follow-up after a manual flash and not part of the main developer path | COMP-01 | Firmware build proves compile safety, so attached-port smoke testing is reserved for explicit follow-up validation only after the user-owned flash checkpoint | After the manual `v2_dev` flash, send one JSON request and one binary request from the backend harness, then confirm both respond on the same transport boundary |

---

## Validation Sign-Off

- [x] All tasks have `<automated>` verify or Wave 0 dependencies
- [x] Sampling continuity: no 3 consecutive tasks without automated verify
- [x] Wave 0 covers all MISSING references
- [x] No watch-mode flags
- [x] Feedback latency < 45s
- [x] `nyquist_compliant: true` set in frontmatter

**Approval:** pending
