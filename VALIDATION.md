# Validation Authority

Reviewed: 2026-09-22

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Source evidence baseline: `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6 fix: preserve TWAI lifecycle and alerts`; Stage 4T TX acceptance/backpressure correction was implemented at `0c699d6`.
- Stage 4R lifecycle / alert observability correction, Stage 4T TX contract correction, Stage 5 evidence consolidation, Phase 2A host-testable ISO-TP / `DiagnosticTransport` core, Phase 3A `ReadOnlyGuard` safety gate, Phase 3B Generic OBD-II read-only host service core, Phase 4A UDS `0x22` guard extension, Phase 4B Generic UDS `ReadDataByIdentifier` host service core and Phase 5A normalized VehicleData sample/value semantics are complete. Phase 4B implementation is at `59a10b0f6453c07bc65f370bd6186b0f46e4a997`; it covers single-DID positive matching, terminal negative response and bounded NRC `0x78` only. Phase 5A implementation is at `68864ba`; it covers only generic single-sample representation and invariants. Store/Cache, Scheduler, registry and profile mapping remain future work. Physical runtime behavior remains unverified.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock, TWAI backend, Classic CAN ISO-TP / `DiagnosticTransport` core, `ReadOnlyGuard` OBD/UDS safety gates and Generic OBD-II/UDS host service cores are present in the repository | Software evidence exists; this does not prove physical behavior, broader UDS service/session/DTC behavior or application-facing diagnostic TX |
| Host Test | PASS — Phase 5A current | `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp`; executed successfully for `68864ba` | Covers deterministic ISO-TP, OBD and guard regressions, UDS `0x22` semantics and normalized VehicleData sample/value invariants; no UDS runtime, TWAI correctness or physical behavior inference |
| ESP32 Compile | PASS — Phase 1 evidence | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11` / ESP-IDF `5.5.5`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --build-path C:\Users\user\AppData\Local\Temp\esp32-vag-stage4t-build-final --fqbn esp32:esp32:esp32s3 --warnings all --verbose src`; `esp32_twai_can.cpp.o` appears in the build log; tested Stage 4T implementation commit `0c699d6` | Preserved Phase 1 backend participation evidence; not re-run for Phase 2A, Phase 3A, Phase 3B, Phase 4A, Phase 4B or Phase 5A because ESP32-facing source participation / platform boundary did not change |
| CI | PASS — Phase 2A evidence | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `35688710299` at head `43a550de1e835fffad22e63f29e03943b6dea6c5`; compile and test steps succeeded | Existing CI evidence covers Phase 2A; Phase 3A local host-test evidence is recorded separately. The workflow only compiles/executes host tests and is not ESP32, TWAI runtime, Bench, Hardware or Vehicle evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Stage 2 build-layout and backend-participation validation is complete. Stage 4T required revalidation after the material TX mapping change; Phase 1 host and ESP32 compile evidence at `0c699d6` remains current for that implementation scope. Phase 2A host compile/tests and CI run `35688710299` remain recorded at `43a550de`; Phase 3A host compile/tests passed at `5eefb44`; Phase 3B host compile/tests passed at `5e038fc`; Phase 4A host compile/tests passed at `4b86093`; Phase 4B host compile/tests passed at `59a10b0`; Phase 5A host compile/tests passed at `68864ba`. No ESP32 compile was required because the Generic Core guard/service/sample headers did not change ESP32-facing source participation or the platform boundary. All evidence remains scope-qualified and does not infer physical behavior, broader UDS service/session/DTC behavior, VehicleData Store/Cache, Scheduler or application-facing diagnostic TX.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
