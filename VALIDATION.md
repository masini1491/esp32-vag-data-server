# Validation Authority

Reviewed: 2026-09-22

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Source evidence baseline: `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6 fix: preserve TWAI lifecycle and alerts`; Stage 4T TX acceptance/backpressure correction was implemented at `0c699d6`.
- Stage 4R lifecycle / alert observability correction, Stage 4T TX contract correction, Stage 5 evidence consolidation, Phase 2A host-testable ISO-TP / `DiagnosticTransport` core, Phase 3A `ReadOnlyGuard` safety gate and Phase 3B Generic OBD-II read-only host service core are complete. Phase 3B implementation is at `5e038fc114b5ea5b1bab6c1b517023f1e43e35c9`; it covers one outstanding request, Mode `0x01` raw data / supported-PID blocks and Mode `0x09` PID `0x02` VIN response validation only. Physical runtime behavior remains unverified.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock, TWAI backend, Classic CAN ISO-TP / `DiagnosticTransport` core, `ReadOnlyGuard` safety gate and Generic OBD-II host service core are present in the repository | Software evidence exists; this does not prove physical behavior, DTC/UDS semantics or application-facing diagnostic TX |
| Host Test | PASS — Phase 3B current | `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp`; executed successfully for implementation `5e038fc114b5ea5b1bab6c1b517023f1e43e35c9` | Covers deterministic ISO-TP and `ReadOnlyGuard` regression plus OBD response matching, one-request Busy, Clock timeout, lower-status propagation, supported-PID bitmap and VIN validation matrix; no TWAI runtime correctness inference |
| ESP32 Compile | PASS — Phase 1 evidence | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11` / ESP-IDF `5.5.5`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --build-path C:\Users\user\AppData\Local\Temp\esp32-vag-stage4t-build-final --fqbn esp32:esp32:esp32s3 --warnings all --verbose src`; `esp32_twai_can.cpp.o` appears in the build log; tested Stage 4T implementation commit `0c699d6` | Preserved Phase 1 backend participation evidence; not re-run for Phase 2A, Phase 3A or Phase 3B because ESP32-facing source participation / platform boundary did not change |
| CI | PASS — Phase 2A evidence | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `35688710299` at head `43a550de1e835fffad22e63f29e03943b6dea6c5`; compile and test steps succeeded | Existing CI evidence covers Phase 2A; Phase 3A local host-test evidence is recorded separately. The workflow only compiles/executes host tests and is not ESP32, TWAI runtime, Bench, Hardware or Vehicle evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Stage 2 build-layout and backend-participation validation is complete. Stage 4T required revalidation after the material TX mapping change; Phase 1 host and ESP32 compile evidence at `0c699d6` remains current for that implementation scope. Phase 2A host compile/tests and CI run `35688710299` remain recorded at `43a550de`. Phase 3A host compile/tests passed at `5eefb44`; Phase 3B host compile/tests passed at `5e038fc`. No ESP32 compile was required because the Generic Core guard/service headers did not change ESP32-facing source participation or the platform boundary. All evidence remains scope-qualified and does not infer physical behavior, DTC/UDS semantics or broader diagnostic service completion.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
