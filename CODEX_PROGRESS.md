# Project State Summary

Reviewed: 2026-09-22

This file is the human-readable historical project-state summary. It is not the active work queue; current unfinished work is kept only in `TASKS.md`, and current validation authority is `VALIDATION.md`.

## Current project state

- Architecture Freeze v0.4 is established.
- Phase 1 software foundation is established: Generic CAN types, Board Profile → HardwareConfig → HAL, deterministic Mock CAN / Fake Clock, and the ESP32-S3 TWAI backend.
- Phase 2A host-testable Classic CAN ISO-TP / `DiagnosticTransport` core is implemented at `43a550de1e835fffad22e63f29e03943b6dea6c5`; local host compile/tests and GitHub Actions host-tests run `35688710299` passed.
- Phase 3A `ReadOnlyGuard` safety gate is implemented at `5eefb442bc00e58005cc933fa6d56ab8bbe45ef6`; deterministic host tests verify only semantic Mode `0x01` / `0x09` single-PID forwarding, fail-closed denial and lower transport status preservation.
- Phase 3B Generic OBD-II read-only host service core is implemented at `5e038fc114b5ea5b1bab6c1b517023f1e43e35c9`; it provides one-request-at-a-time raw Mode `0x01` data, supported-PID block and fixed VIN semantics exclusively through `ReadOnlyGuard`.
- Phase 4A `ReadOnlyGuard` UDS safety extension is implemented at `4b8609302dc6569af53d7718d4ea75c152c682cb`; it permits only semantic single-DID `ReadDataByIdentifier (0x22)` outbound construction and lower status propagation.
- Phase 4B Generic UDS `ReadDataByIdentifier` host service core is implemented at `59a10b0f6453c07bc65f370bd6186b0f46e4a997`; it provides one outstanding `0x22` DID read, raw positive data, terminal negative NRC and bounded `0x78` response-pending behavior through `ReadOnlyGuard`.
- Other UDS services, session/DTC/TesterPresent behavior, VAG routing, VehicleData and application-facing diagnostic TX remain unstarted.
- The project remains VAG-first, with Škoda Kamiq as the primary vehicle validation target. Portability order remains Kamiq → T-Roc → RAV4 → Wish.

## Long-term decisions

- Generic Core remains brand-independent; VAG routing, diagnostics, mappings and vehicle semantics belong behind the VAG Brand Layer / Vehicle Profile boundary.
- The v1 concrete path is ESP32-S3 Classic CAN/TWAI → ISO-TP → read-only OBD-II/UDS → VAG Brand Layer → VehicleData.
- Diagnostic behavior remains read-only. No coding, adaptation, Clear DTC, output tests, security access, flashing or actuator control is permitted.
- Bench, Hardware and Vehicle evidence cannot be inferred from software or compile evidence.

## Important revalidation state

- ESP32 TWAI backend participation in the standard Arduino build, Stage 4R runtime hardening and Stage 4T TX acceptance/backpressure correction are reflected in the current evidence; physical behavior remains Pending. See `VALIDATION.md` for current evidence and Pending authority.
- Phase 2A host-core evidence is current at `43a550de`; ESP32 compile was not re-run because ESP32-facing source participation / platform boundary did not change. This summary is historical and not a second queue or validation ledger; detailed changes remain in Git history.
- Phase 3A host tests passed at `5eefb44`; no ESP32 compile was re-run because the Generic Core guard did not change ESP32-facing source participation or the platform boundary. CI evidence remains scoped to the recorded Phase 2A run until a Phase 3A workflow result is separately recorded.
- Phase 3B host tests passed at `5e038fc`; no ESP32 compile was re-run because the Generic Core OBD service did not change ESP32-facing source participation or the platform boundary. CI evidence remains scoped to the recorded Phase 2A run until a Phase 3B workflow result is separately recorded.
- Phase 4A host tests passed at `4b86093`; no ESP32 compile was re-run because the guard-only UDS extension did not change ESP32-facing source participation or the platform boundary. This does not establish UDS response/runtime or physical evidence.
- Phase 4B host tests passed at `59a10b0`; no ESP32 compile was re-run because the Generic Core UDS service did not change ESP32-facing source participation or the platform boundary. This does not establish live UDS runtime or physical evidence.
