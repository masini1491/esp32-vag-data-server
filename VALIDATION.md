# Validation Authority

Reviewed: 2026-08-24

This file is the current validation contract, evidence summary and Pending authority. It is not an active task queue. Unfinished work belongs only in `TASKS.md`; detailed modification history belongs to Git history.

## Current baseline

- Baseline commit: `01dda61 docs: queue project state and validation records`
- No new compile, test or CI evidence was created by Stage 1B.
- Hardware and vehicle claims remain evidence-gated; software evidence must not be promoted to physical validation.

## Validation levels

| Level | Current status | Evidence / reproducibility | Current interpretation |
|---|---|---|---|
| Software / Static | PASS | Generic CAN model, Board Profile → HardwareConfig → HAL, Mock CAN, Fake Clock and TWAI backend are present in the repository | Software foundation exists; this does not prove physical behavior |
| Host Test | PASS — historical | GitHub Actions run `32558147322`; workflow compiles `tests/host/main.cpp` with `g++ -std=c++17 -Wall -Wextra -pedantic -I.` and executes the binary | Existing host evidence; re-run after relevant hardening changes |
| ESP32 Compile | Historical / Revalidation Required | Arduino-ESP32 `3.3.11`, FQBN `esp32:esp32:esp32s3`; prior compile evidence is recorded in project docs | Known Arduino build-layout gap means this is not current proof that `Esp32TwaiCan` participates in the standard Arduino build |
| CI | PASS — historical host CI | `.github/workflows/host-tests.yml`, GitHub-hosted `ubuntu-latest`, run `32558147322` | Host CI exists; no ESP32 backend CI evidence |
| Bench | Pending | No bench evidence recorded | Must remain Pending |
| Hardware | Pending | No physical ESP32/CAN transceiver evidence recorded | Must remain Pending |
| Vehicle | Pending | No real-vehicle evidence recorded; `VEHICLE_CONFIRMED = none` | Must remain Pending |

## Required revalidation

Before treating Phase 1 compile evidence as complete, Stage 2 must verify the Arduino build layout and produce reproducible evidence that the ESP32-S3 TWAI backend is actually compiled. Stage 5 must then consolidate fresh host and ESP32 evidence after hardening.

## Evidence rules

- Record tested commit SHA, toolchain/version, board/FQBN, exact command or CI run whenever the evidence supports it.
- Mark superseded or insufficient evidence as Historical, Revalidation Required or Pending; do not present it as a current PASS.
- Keep Software, Static, Host Test, ESP32 Compile, CI, Bench, Hardware and Vehicle levels separate.
- A PASS at one level never implies PASS at a higher physical or vehicle level.
