# Application-facing VehicleData read model readiness — 2026-10-05

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 4C canonical closure:
- project main: `374d5aef2b374a482ec3521d34f8ab210df7525d`
- `TASKS.md = EMPTY`
- Active profile selection exists.
- Capability registry exists.
- VehicleData Store/Cache exists.
- BLE / Web / Logger remain planned application consumers.
- Clients are architecturally required to consume normalized VehicleData/capabilities rather than raw CAN/DID/brand mappings.

## Readiness conclusion

A transport-neutral, cache-backed application read model is now non-speculative and admissible.

It has real consumers:
- Phase 7 BLE;
- Phase 8 Web;
- Logger/export.

The first slice must remain read-only and must not initiate diagnostic traffic.

## First-slice ownership

Provide one host-testable application-facing read projection that can report, from already-owned state:

1. Active profile selection state.
2. Optional active profile identity when selected.
3. Fixed capability descriptors associated with the selected profile, when available.
4. A bounded snapshot of current `VehicleDataStore` samples.

The read model is an adapter/projection over existing owners. It must not become a second source of truth.

## Required semantics

### Profile state
- preserve current `ProfileSelectionState` exactly;
- if state is not `Selected`, active profile identity is absent;
- do not guess/fallback to a profile.

### Capability view
- if a selected profile has a known registry, expose bounded descriptor metadata;
- preserve `CapabilitySupport` exactly;
- `Pending`, `Unknown`, `Unsupported` must not be converted into sample values;
- if no selected profile or no registry is associated, capability view is empty/not-present rather than fabricated Unsupported.

### VehicleData snapshot
- obtain data only from `VehicleDataStore::snapshot()`;
- preserve sample identity, value, unit, source, quality, availability and timestamp exactly;
- no automatic TTL/staleness inference beyond already stored sample state;
- insufficient output capacity fails explicitly; do not silently truncate.

### Read-only boundary
The read model must not:
- call OBD/UDS services;
- call `ReadOnlyGuard`;
- request Scheduler jobs;
- mutate `VehicleDataStore`;
- mutate profile selection;
- mutate capability support;
- discover routes/DIDs;
- trigger vehicle I/O.

Client refresh must be safe to repeat without generating ECU traffic.

## API shape constraints

Prefer simple fixed/bounded host-testable C++ data views/result structures using existing types.

Do not:
- introduce JSON as the core domain contract;
- bind to BLE/Web/Arduino/Wi-Fi libraries;
- introduce callback/event framework;
- introduce dynamic allocation solely for this slice;
- create a second SignalId/ProfileIdentity/CapabilitySupport/VehicleData representation unless a copy/view wrapper is strictly required.

The application projection may copy existing bounded values for ownership safety.

## Deterministic host-test minimum

- unresolved profile states expose no active identity;
- selected admitted profile exposes its identity;
- selected Kamiq_NW4 can expose exactly the existing four capability descriptors, preserving all four as Pending;
- unknown/no-registry profile path exposes no fabricated capabilities;
- empty Store produces empty sample snapshot;
- populated Store snapshot preserves multiple samples exactly;
- insufficient caller capacity returns explicit failure and no partial-success claim;
- repeated reads are deterministic and side-effect free;
- read does not mutate Store, ActiveVehicleProfile or registry state;
- no diagnostic transport / guard / UDS / OBD / Scheduler interaction is reachable from the read-model API;
- all existing regressions PASS.

## Explicit non-goals

- BLE serialization/characteristics;
- HTTP/JSON/Web UI;
- Logger file format;
- Scheduler/polling integration;
- DTC application presentation;
- profile auto-detection;
- TTL/aging policy;
- runtime support learning;
- vehicle-specific mapping;
- hardware/vehicle validation.

## Evidence boundary

This Stage establishes only a host-side application read contract over already-existing software state.

It creates no:
- diagnostic TX evidence;
- BLE/Wi-Fi evidence;
- ESP32 runtime evidence;
- Hardware PASS;
- Vehicle PASS.

Bench / Hardware / Vehicle remain Pending and `VEHICLE_CONFIRMED = none`.
