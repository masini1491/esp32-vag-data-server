# Project State Summary

Reviewed: 2026-09-22

This file is the human-readable historical project-state summary. It is not the active work queue; current unfinished work is kept only in `TASKS.md`, and current validation authority is `VALIDATION.md`.

## Current project state

- Architecture Freeze v0.4 is established.
- Phase 1 software foundation is established: Generic CAN types, Board Profile → HardwareConfig → HAL, deterministic Mock CAN / Fake Clock, and the ESP32-S3 TWAI backend.
- Phase 2A host-testable Classic CAN ISO-TP / `DiagnosticTransport` core is implemented at `43a550de1e835fffad22e63f29e03943b6dea6c5`; local host compile/tests and GitHub Actions host-tests run `35688710299` passed.
- Phase 3A `ReadOnlyGuard` safety gate is implemented at `5eefb442bc00e58005cc933fa6d56ab8bbe45ef6`; deterministic host tests verify only semantic Mode `0x01` / `0x09` single-PID forwarding, fail-closed denial and lower transport status preservation.
- OBD-II service semantics, UDS, VAG routing, VehicleData and application-facing diagnostic TX remain unstarted.
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
