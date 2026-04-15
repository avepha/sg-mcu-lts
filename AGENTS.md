# Repository Guidelines

## Project Structure & Module Organization
This repository is an ESP32 firmware project built with PlatformIO and Arduino. Application code lives in `src/`, with most features grouped by domain under paths such as `src/domain/channel-control/`, `src/domain/sequence-control/`, and `src/domain/station/`. Shared bootstrapping and device entrypoints are in files like `src/main.cpp`, `src/bootstrap.h`, and `src/*Endpoint.h`.

Public headers and library placeholders live in `include/` and `lib/`. Protocol examples and request payload fixtures are stored in `docs/` as JSON files. Tests live under `test/`, with the current Unity-based MCU test entrypoint in `test/test_main.cpp`.

## Build, Test, and Development Commands
Use the local `rtk` wrapper for shell commands in this workspace.

- `rtk pio run -e v2_dev`: build the default development firmware for ESP32.
- `rtk pio run -e v2`: build the LTS production variant without development flags.
- `rtk pio test`: run PlatformIO Unity tests from `test/`.
- `rtk pio device monitor -b 345600`: open the serial monitor with the configured baud rate.

Run commands from the repository root. Do not commit `.pio/` build artifacts.

## Coding Style & Naming Conventions
Follow the existing C++ style: 2-space indentation, opening braces on the same line, and short inline comments only where logic is not obvious. Keep feature code scoped to its domain directory instead of adding cross-cutting utilities to unrelated modules.

Match current naming patterns:
- Types and classes: `PascalCase`
- Functions and variables: `camelCase`
- Macros and compile flags: `UPPER_SNAKE_CASE`
- Test helpers and fixtures: descriptive names such as `rs485BasicCommand`

No formatter or linter is checked in, so consistency with nearby files is the standard.

## Testing Guidelines
Tests use PlatformIO's Unity support. Add new MCU tests under `test/` and register them in `test/test_main.cpp` with `RUN_TEST(...)`. Prefer small hardware-focused tests that validate packet parsing, transport behavior, and domain rules independently where possible.

## Commit & Pull Request Guidelines
Recent history follows short, imperative messages with conventional prefixes when useful, for example `fix(espressif): ...`, `fix: ...`, and `chore: ...`. Keep commits focused on one change.

Pull requests should describe the target environment (`v1`, `v2`, `v2_dev`, etc.), summarize firmware behavior changes, list test/build commands run, and include serial logs or payload examples when the change affects device I/O or protocol handling.
