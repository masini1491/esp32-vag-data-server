# Validation Authority

Reviewed: 2026-10-05

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

EV-1's [Kamiq Read-only Evidence Acquisition Protocol](docs/KAMIQ_READ_ONLY_EVIDENCE.md) is documentation only. No capture or physical observation occurred; it creates no validation PASS and Bench / Hardware / Vehicle remain Pending.

- Source evidence baseline: `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6 fix: preserve TWAI lifecycle and alerts`; Stage 4T TX acceptance/backpressure correction was implemented at `0c699d6`.
- Stage 4R lifecycle / alert observability correction, Stage 4T TX contract correction, Stage 5 evidence consolidation, Phase 2A host-testable ISO-TP / `DiagnosticTransport` core, Phase 3A `ReadOnlyGuard` safety gate, Phase 3B Generic OBD-II read-only host service core, Phase 4A UDS `0x22` guard extension, Phase 4B Generic UDS `ReadDataByIdentifier` host service core, Phase 5A normalized VehicleData sample/value semantics and Phase 5B bounded VehicleData Store/Cache are complete. Phase 4B implementation is at `59a10b0f6453c07bc65f370bd6186b0f46e4a997`; it covers single-DID positive matching, terminal negative response and bounded NRC `0x78` only. Phase 5A implementation is at `68864ba`; it covers only generic single-sample representation and invariants. Phase 5B implementation is at `d7b8552`; it covers fixed-capacity latest-state ownership, timestamp ordering, whole-sample replacement and copy-based read/snapshot only. Eviction, TTL/automatic aging, Scheduler application integration and profile mapping remain future work. Physical runtime behavior remains unverified.
- Phase 5C cooperative Scheduler core is implemented at `96203ffe5e66f0a4abf8c0c19b6fd82ffe681159`: fixed-capacity opaque job registration, Startup/Periodic/OnDemand timing, stable due order, single active job and externally reported completion. Periodic deadlines retain absolute cadence and skip missed periods. Callback/executor, RealtimeTriggered, priority, retry/backoff, Store mutation/aging, profile polling, FreeRTOS binding and application integration remain unimplemented.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.
- Phase 6a profile identity / Active Profile selection boundary is implemented at `e237aa17700d64b07917fda8e30d72076211bfe8`: opaque identity, fixed admitted identity set and manual selection with Unknown / Ambiguous / ManualSelectionRequired / Selected states. The compile-time VAG Profile Set contains only the Kamiq 2024 facelift identity reference `Kamiq_NW4`, with validation Pending. Selection establishes neither capability support nor physical validation; automatic resolver, routes, mappings and Phase 6c remain unimplemented.
- Phase 6b capability-support / normalized signal registry is implemented at `453efa72740231e35fe3523c90bd5a2b6dbc0e8c`: fixed immutable metadata using the existing SignalId/Unit types; separate Supported / Unsupported / Pending / Unknown support state; deterministic not-found lookup. Kamiq_NW4 has only four Numeric candidates (speed, rpm, coolantTemp, voltage), all Pending. Registry lookup and selection do not mutate samples, Store or support knowledge; runtime support learning, mapping, polling and diagnostic execution remain unimplemented.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock, TWAI backend, Classic CAN ISO-TP / `DiagnosticTransport` core, `ReadOnlyGuard` OBD/UDS safety gates and Generic OBD-II/UDS host service cores and cooperative Scheduler core are present in the repository | Software evidence exists; this does not prove physical behavior, broader UDS service/session/DTC behavior or application-facing diagnostic TX |
| Host Test | PASS — Phase 6b current | clang `22.1.8`, target `x86_64-pc-windows-msvc`; `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp -o C:\Users\user\AppData\Local\Temp\esp32-vag-phase6b-host-tests.exe`, followed by execution of that binary; tested source/test commit `453efa72740231e35fe3523c90bd5a2b6dbc0e8c` | Covers all existing diagnostic, VehicleData, Scheduler and Phase 6a regressions plus distinct support type, fixed descriptor membership/type/unit, Pending stability, not-found lookup and no sample/Store mutation; no confirmed Kamiq capability or physical behavior inference |
| ESP32 Compile | PASS — Phase 1 evidence | Arduino CLI `1.5.1`, Arduino-ESP32 `3.3.11` / ESP-IDF `5.5.5`, FQBN `esp32:esp32:esp32s3`; `arduino-cli compile --clean --build-path C:\Users\user\AppData\Local\Temp\esp32-vag-stage4t-build-final --fqbn esp32:esp32:esp32s3 --warnings all --verbose src`; `esp32_twai_can.cpp.o` appears in the build log; tested Stage 4T implementation commit `0c699d6` | Preserved Phase 1 backend participation evidence; not re-run for Phase 2A, Phase 3A, Phase 3B, Phase 4A, Phase 4B, Phase 5A, Phase 5B, Phase 5C, Phase 6a or Phase 6b because ESP32-facing source participation / platform boundary did not change |
| CI | PASS — Phase 2A evidence | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `35688710299` at head `43a550de1e835fffad22e63f29e03943b6dea6c5`; compile and test steps succeeded | Existing CI evidence covers Phase 2A; Phase 3A local host-test evidence is recorded separately. The workflow only compiles/executes host tests and is not ESP32, TWAI runtime, Bench, Hardware or Vehicle evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Stage 2 build-layout and backend-participation validation is complete. Stage 4T required revalidation after the material TX mapping change; Phase 1 host and ESP32 compile evidence at `0c699d6` remains current for that implementation scope. Phase 2A host compile/tests and CI run `35688710299` remain recorded at `43a550de`; Phase 3A host compile/tests passed at `5eefb44`; Phase 3B host compile/tests passed at `5e038fc`; Phase 4A host compile/tests passed at `4b86093`; Phase 4B host compile/tests passed at `59a10b0`; Phase 5A host compile/tests passed at `68864ba`; Phase 5B host compile/tests passed at `d7b8552`; Phase 5C host compile/tests passed at `96203ff`. No ESP32 compile was required because the Generic Core guard/service/sample/store/scheduler headers did not change ESP32-facing source participation or the platform boundary. All evidence remains scope-qualified and does not infer physical behavior, broader UDS service/session/DTC behavior, VehicleData eviction/TTL/automatic aging, Scheduler application integration or application-facing diagnostic TX.

Phase 6a host compile/tests passed at `e237aa1`, including all existing regressions. Profile headers contain no platform dependency and do not change ESP32-facing source participation; no new ESP32 evidence is claimed. Source/API review confirms manual identity selection only, without capability/signal registry, automatic matching, mappings, Scheduler binding or diagnostic execution.

Phase 6b host compile/tests passed at `453efa7`, including all existing regressions. Source/API review confirms immutable support metadata, no runtime-failure input or support mutation API, and no mapping, source arbitration, polling, Scheduler binding or diagnostic execution. ESP32 compile was not re-run because ESP32-facing source participation/platform boundary did not change; existing Phase 1 evidence is preserved, not promoted to new Phase 6b evidence.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
