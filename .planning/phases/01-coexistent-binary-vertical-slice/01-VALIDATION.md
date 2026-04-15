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
| **Framework** | PlatformIO Unity + native C++ protocol test env |
| **Config file** | `platformio.ini` |
| **Quick run command** | `rtk pio test -e native_protocol` |
| **Full suite command** | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` |
| **Estimated runtime** | ~35 seconds |

---

## Sampling Rate

- **After every task commit:** Run `rtk pio test -e native_protocol`
- **After every plan wave:** Run `rtk pio test -e native_protocol && rtk pio run -e v2_dev`
- **Before `/gsd-verify-work`:** Full suite must be green
- **Max feedback latency:** 45 seconds

---

## Per-Task Verification Map

| Task ID | Plan | Wave | Requirement | Threat Ref | Secure Behavior | Test Type | Automated Command | File Exists | Status |
|---------|------|------|-------------|------------|-----------------|-----------|-------------------|-------------|--------|
| 1-01-01 | 01 | 1 | SCMA-01, SCMA-02, SCMA-03, SCMA-04, ENVP-01, ENVP-02, ENVP-03, ENVP-04 | T-01-01 | Canonical schema/envelope contract is versioned and bounded | native/unit | `rtk pio test -e native_protocol` | ❌ W0 | ⬜ pending |
| 1-01-02 | 01 | 1 | TRAN-02, TRAN-03 | T-01-02 | Fast tests reject CRC and oversize inputs before firmware integration | native/unit | `rtk pio test -e native_protocol` | ❌ W0 | ⬜ pending |
| 1-02-01 | 02 | 2 | TRAN-01, TRAN-02, TRAN-03, RUNT-02 | T-01-03 | Binary frame parser accepts valid frames and safely rejects malformed traffic | native/unit | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |
| 1-02-02 | 02 | 2 | ENVP-01, ENVP-02, ENVP-03, ENVP-04, COMP-01, SLCE-01 | T-01-04 | Binary transport coexists with JSON and routes `info.query` through existing domain logic | native/integration + build | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |
| 1-03-01 | 03 | 3 | RUNT-01, SLCE-02 | T-01-05 | Backend docs encode timeout/retry and wire examples unambiguously | docs + native fixture | `rtk pio test -e native_protocol` | ✅ | ⬜ pending |
| 1-03-02 | 03 | 3 | COMP-01, RUNT-02 | T-01-06 | Regression suite proves JSON discrimination and malformed frame handling remain safe | native/integration + build | `rtk pio test -e native_protocol && rtk pio run -e v2_dev` | ✅ | ⬜ pending |

*Status: ⬜ pending · ✅ green · ❌ red · ⚠️ flaky*

---

## Wave 0 Requirements

- [ ] `test/protocol/test_binary_protocol.cpp` — protocol/frame/schema regression scaffold
- [ ] `platformio.ini` — `native_protocol` test environment

---

## Manual-Only Verifications

| Behavior | Requirement | Why Manual | Test Instructions |
|----------|-------------|------------|-------------------|
| Serial monitor sanity check on real ESP32 hardware is optional follow-up | COMP-01 | Firmware build proves compile safety, but attached-port smoke test still benefits from human hardware access | Flash `v2_dev`, send one JSON request and one binary request from the backend harness, confirm both respond on the same transport boundary |

---

## Validation Sign-Off

- [x] All tasks have `<automated>` verify or Wave 0 dependencies
- [x] Sampling continuity: no 3 consecutive tasks without automated verify
- [x] Wave 0 covers all MISSING references
- [x] No watch-mode flags
- [x] Feedback latency < 45s
- [x] `nyquist_compliant: true` set in frontmatter

**Approval:** pending
