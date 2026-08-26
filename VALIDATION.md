# Validation Authority

Reviewed: 2026-08-26

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Baseline commit: `5b50343 build: include TWAI backend in Arduino sketch`
- Stage 2 produced reproducible Arduino-ESP32 compile evidence after the sketch layout fix.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock and TWAI backend are present in the repository | Software foundation exists; this does not prove physical behavior |
| Host Test | PASS — historical | GitHub Actions run `32558147322`; workflow compiles `tests/host/main.cpp` with `g++ -std=c++17 -Wall -Wextra -pedantic -I.` and executes the binary | Existing host evidence; re-run after relevant hardening changes |
| ESP32 Compile | PASS | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --fqbn esp32:esp32:esp32s3 --warnings all src`; verbose output compiled `src/esp32_twai_can.cpp`; tested commit `5b50343` | Standard Arduino sketch build now compiles the ESP32 TWAI backend; this does not prove physical behavior |
| CI | PASS — historical host CI | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `32558147322` | Host CI exists; no ESP32 backend CI evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Stage 2 build-layout and backend-participation validation is complete. Stage 5 must still consolidate fresh host and ESP32 evidence after remaining Phase 1 hardening.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
