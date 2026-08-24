# Project State Summary

Reviewed: 2026-08-24

This file is the human-readable historical project-state summary. It is not the active work queue; current unfinished work is kept only in `TASKS.md`, and current validation authority is `VALIDATION.md`.

## Current project state

- Architecture Freeze v0.4 is established.
- Phase 1 software foundation is established: Generic CAN types, Board Profile → HardwareConfig → HAL, deterministic Mock CAN / Fake Clock, and the ESP32-S3 TWAI backend.
- Phase 2 ISO-TP implementation has not started.
- The project remains VAG-first, with Škoda Kamiq as the primary vehicle validation target. Portability order remains Kamiq → T-Roc → RAV4 → Wish.

## Long-term decisions

- Generic Core remains brand-independent; VAG routing, diagnostics, mappings and vehicle semantics belong behind the VAG Brand Layer / Vehicle Profile boundary.
- The v1 concrete path is ESP32-S3 Classic CAN/TWAI → ISO-TP → read-only OBD-II/UDS → VAG Brand Layer → VehicleData.
- Diagnostic behavior remains read-only. No coding, adaptation, Clear DTC, output tests, security access, flashing or actuator control is permitted.
- Bench, Hardware and Vehicle evidence cannot be inferred from software or compile evidence.

## Important revalidation state

- Existing ESP32 compile evidence used Arduino-ESP32 3.3.11 with `esp32:esp32:esp32s3`, but the current queue records a build-layout gap: it does not yet prove that the TWAI backend participates in a standard Arduino build. This evidence is historical and requires revalidation.
- See `VALIDATION.md` for the current evidence baseline and Pending / Revalidation Required states. Detailed changes remain in Git history.
