# Validation Authority

Reviewed: 2026-08-27

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Baseline commit: `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6 fix: preserve TWAI lifecycle and alerts`
- Stage 4R lifecycle / alert observability correction has current host and ESP32 compile evidence. Its physical runtime behavior remains unverified.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock and TWAI backend are present in the repository | Software foundation exists; this does not prove physical behavior |
| Host Test | PASS — local revalidation | `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp`; executed successfully; tested commit `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6` | Host regression only; no TWAI runtime correctness inference |
| ESP32 Compile | PASS | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11` / ESP-IDF `5.5.5`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --build-path C:\Users\user\AppData\Local\Temp\esp32-vag-stage4r-build --fqbn esp32:esp32:esp32s3 --warnings all --verbose src`; `esp32_twai_can.cpp.o` appears in the build log; tested commit `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6` | Standard Arduino sketch build compiles the corrected TWAI backend; this does not prove physical behavior |
| CI | PASS — host baseline | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `32941791913` at head `800ff97e7027bca32ab65efa0711747a13e85636` | Workflow only compiles/executes host tests; it is not current TWAI backend, runtime, Bench, Hardware or Vehicle evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Stage 2 build-layout and backend-participation validation is complete. Stage 4R affected host and ESP32 compile evidence has been revalidated at `2e4ec1d`; Stage 5 must consolidate current host CI / remaining Phase 1 evidence without inferring physical behavior.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
