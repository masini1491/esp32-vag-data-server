# Phase 8 Web API/UI fixture path readiness — 2026-10-05

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed at project main:
- `cf9b3431012705263897441d6cda73eaed9f7749`
- `TASKS.md = EMPTY`
- Application-facing VehicleData read model / snapshot API is canonically complete.
- BLE / Web / Logger remain unimplemented.
- Bench / Hardware / Vehicle remain Pending.

## Readiness conclusion

The Phase 8 Web trigger is satisfied.

A bounded host-side Web/API fixture slice can now consume the application read model without touching ESP32 Wi-Fi, vehicle I/O or diagnostic services.

This is a real consumer of the application read boundary, not a speculative framework.

## First-slice responsibility

Build a host-testable Web-facing representation driven only by fixture/mock application-read snapshots.

The first slice may include:
- a transport-neutral Web/API DTO/serialization contract derived from the application read model;
- deterministic host tests for selection/capability/sample representation;
- a minimal local/static UI fixture that renders representative states without requiring ESP32 networking.

The Stage must not require a real HTTP server or Wi-Fi stack.

## Required semantics

Preserve:
- profile selection state;
- optional active profile identity;
- capability descriptor identity/type/unit/support;
- VehicleData sample identity/value/unit/source/quality/availability/timestamp.

Do not:
- invent Supported capability from Pending;
- convert missing capability registry to Unsupported;
- convert unavailable/unknown sample state into numeric zero/false/empty;
- expose raw CAN IDs, DIDs, ECU addresses or brand-internal mapping;
- trigger diagnostic reads on page/API refresh.

## Fixture coverage

At minimum represent:
- unresolved profile / no active identity;
- selected Kamiq_NW4 with four Pending capability descriptors;
- empty VehicleData snapshot;
- populated numeric samples;
- unavailable/pending/unknown sample states;
- stale/invalid quality where already expressible;
- explicit bounded serialization/fixture failure rather than silent truncation.

## Architecture boundary

This slice is host-side contract/UI fixture only.

No:
- Arduino;
- ESP32 Wi-Fi;
- SoftAP;
- WebServer library selection;
- production HTTP binding;
- BLE;
- Logger;
- Scheduler;
- OBD/UDS/ReadOnlyGuard;
- vehicle mapping;
- hardware or vehicle PASS.

## Promotion rationale

Web is sequenced ahead of BLE because the cache-backed read model can be exercised immediately with PC/browser fixtures without freezing BLE MTU/stack-specific details.

BLE remains a valid Cold/ready item after the same application-read-model trigger, but should not execute concurrently with this Hot Stage.

Logger remains dependent on stable client-facing contracts and should stay Cold.

## Validation boundary

Host/static validation only.

This Stage creates no:
- ESP32 runtime evidence;
- Wi-Fi evidence;
- network-stack evidence;
- Bench/Hardware/Vehicle PASS.

`VEHICLE_CONFIRMED = none`.
