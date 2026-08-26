# Validation Authority

Reviewed: 2026-08-26

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Baseline commit: `4e77bf0 fix: harden ESP32 TWAI runtime boundary`
- Stage 4 produced host and ESP32 compile evidence after TWAI runtime boundary hardening.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock and TWAI backend are present in the repository | Software foundation exists; this does not prove physical behavior |
| Host Test | PASS — local revalidation | `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp`; executed host tests successfully after Stage 4; tested commit `4e77bf0` | Host regression passes locally; GitHub Actions evidence remains historical until Stage 5 refresh |
| ESP32 Compile | PASS | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --build-path C:\Users\user\AppData\Local\Temp\esp32-stage4-final-build --fqbn esp32:esp32:esp32s3 --warnings all src`; output compiled final sketch successfully; tested commit `4e77bf0` | Standard Arduino sketch build compiles the hardened TWAI backend; this does not prove physical behavior |
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
