# Testing Patterns

**Analysis Date:** 2026-04-15

## Test Framework

**Runner:**
- PlatformIO Unity test runner
- Config: `platformio.ini`

**Assertion Library:**
- Unity via `#include <unity.h>` in `test/system/rs485.h`

**Run Commands:**
```bash
rtk pio test              # Run PlatformIO Unity tests in this workspace
rtk pio test -e v2_dev    # Run the default development environment tests
rtk pio run -e v2_dev     # Build firmware when verifying non-test changes
```

## Test File Organization

**Location:**
- Tests live under `test/`.
- The active entrypoint is `test/test_main.cpp`.
- Hardware-oriented helpers and suites currently live in `test/system/rs485.h`.

**Naming:**
- Use descriptive function names for test cases, such as `rs485BasicCommand` in `test/system/rs485.h`.
- Current suite files are headers included by `test/test_main.cpp` rather than separate `.cpp` compilation units.

**Structure:**
```text
test/
├── test_main.cpp
└── system/
    └── rs485.h
```

## Test Structure

**Suite Organization:**
```cpp
// `test/test_main.cpp`
void setup() {
  delay(500);
  bootstrap();

  UNITY_BEGIN();
  RUN_TEST(rs485BasicCommand);
  UNITY_END();
}

void loop() {
}
```

**Patterns:**
- Initialize firmware dependencies before tests by calling `bootstrap()` in `test/test_main.cpp`.
- Register tests explicitly with `RUN_TEST(...)` in `test/test_main.cpp`; adding a new test requires both a function and a registration line.
- Use direct assertion macros inside plain functions rather than fixture classes. `test/system/rs485.h` uses `TEST_ASSERT_MESSAGE(...)`.
- Keep the test runner inside Arduino `setup()` / `loop()` to match MCU execution constraints.

## Mocking

**Framework:**
- Not detected.

**Patterns:**
```cpp
// No mocking layer is present.
// `test/system/rs485.h` exercises real serial I/O paths:
sendDataToStation(packet.getVectorPacket().data(), packet.getVectorPacket().size());

while (!stationPort.available() && millis() - ts <= 1000) {}

TEST_ASSERT_MESSAGE(ModbusPacket::verifyPacket(vByte), "Modbus packet must be verified");
```

**What to Mock:**
- Not established in the current codebase.
- If host-isolated tests are added later, hardware serial endpoints from `src/bootstrap.h` and station communication routines used in `test/system/rs485.h` are the first candidates for seams.

**What NOT to Mock:**
- Current practice favors real hardware interactions for RS485/device communication. Keep hardware-path tests real when validating serial framing, station response timing, or Modbus packet integrity.

## Fixtures and Factories

**Test Data:**
```cpp
// `test/system/rs485.h`
byte data[4] = {0, 0, 0, 0};
ModbusPacket packet(1, 4, data, sizeof(data));
```

**Location:**
- Inline test data currently lives inside the test function in `test/system/rs485.h`.
- JSON request examples in `docs/` act as manual API fixtures for firmware behavior, but they are not wired into automated test execution.

## Coverage

**Requirements:**
- No automated coverage target or threshold is configured.
- No coverage tooling or report generation config was detected in the repository.

**View Coverage:**
```bash
# Not configured in this repository
```

## Test Types

**Unit Tests:**
- Minimal. The current suite does not isolate small classes or functions under `src/`; it boots firmware context and interacts with hardware-connected serial resources.

**Integration Tests:**
- This is the dominant testing style. `test/system/rs485.h` sends a `ModbusPacket`, waits for `stationPort`, and validates the received packet using `ModbusPacket::verifyPacket(...)` from `src/domain/station/modbus/modbusPacket.h`.

**E2E Tests:**
- No separate E2E framework detected.
- Manual end-to-end verification is implied through `README.md` commands for flashing firmware and checking serial monitor output.

## Common Patterns

**Async Testing:**
```cpp
// `test/system/rs485.h`
uint32_t ts = millis();
while (!stationPort.available() && millis() - ts <= 1000) {}

if (millis() - ts >= 1000) {
  TEST_ASSERT_MESSAGE(false, "RS485 timeout");
}
```

**Error Testing:**
```cpp
// Current pattern is assertion-on-timeout rather than exception-based testing.
if (millis() - ts >= 1000) {
  TEST_ASSERT_MESSAGE(false, "RS485 timeout");
}
```

## Build and Verification Signals

- `platformio.ini` defines the testable PlatformIO environments, with `v2_dev` as the default environment.
- `README.md` documents `pio test -e v2_dev` and explicitly warns that tests are hardware-oriented, perform an upload step, and may fail without a connected target board and upload port.
- No CI workflow files were detected under `.github/`; test execution and build verification are currently local, command-driven steps.

## Current Gaps to Preserve Awareness Of

- Only one registered automated test was detected: `RUN_TEST(rs485BasicCommand)` in `test/test_main.cpp`.
- No dedicated tests were detected for request validation in `src/domain/**/permission.h`, routing in `src/combineResolvers.h`, boot behavior in `src/bootstrap.h`, or JSON serialization/error formatting in `src/validationError.h`.
- No fake transport, fixture library, or reusable setup helpers were detected beyond direct use of `bootstrap()` and inline packet construction.
- Because tests depend on live MCU/serial behavior, regressions in pure logic under files such as `src/domain/channel/resolvers.h` can ship without host-only feedback.

---

*Testing analysis: 2026-04-15*
