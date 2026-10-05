# Logger / export contract readiness — 2026-10-05

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 7 canonical closure:
- project main: `7a655e09137f955cea915c488cd04840cc4db9eb`
- `TASKS.md = EMPTY`
- application-facing VehicleData read model is complete;
- Phase 8 Web consumer contract is complete;
- Phase 7 BLE host contract is complete;
- normalized sample semantics already own signal identity, value, unit, source, quality, availability and exact 64-bit timestamp;
- Bench / Hardware / Vehicle remain Pending.

## Readiness conclusion

The Logger / export Cold trigger is satisfied.

The minimum non-speculative first slice is a storage-neutral, host-testable export contract for one already-normalized VehicleData sample at a time.

This Stage must not implement a filesystem logger, persistence scheduler or background writer.

## First-slice contract

Provide a versioned bounded export record derived from an existing copied `NormalizedVehicleSample`.

The record must preserve, without semantic reinterpretation:
- signal identity;
- value type;
- explicit value presence;
- numeric / boolean / bounded text value when present;
- unit;
- source as the existing provenance field;
- quality;
- availability;
- exact 64-bit timestamp.

The export layer may wrap/copy an existing sample for ownership safety, but must not create a second signal identity system, duplicate enum semantics or infer new vehicle state.

## Source-of-truth boundary

Input must come from normalized application state, preferably the existing application read-model snapshot/copies.

Do not:
- read raw CAN frames;
- read OBD/UDS responses;
- call `ReadOnlyGuard`;
- trigger Scheduler work;
- query ECU;
- look up brand mappings;
- generate a new timestamp;
- mutate VehicleData Store, profile or capability state.

A logger/export request must be side-effect free with respect to vehicle/runtime state.

## Export serialization semantics

Provide a deterministic bounded serialization suitable for append-oriented host export.

Requirements:
- explicit schema/version;
- caller-provided fixed output buffer;
- exact required capacity on insufficient capacity;
- no silent truncation;
- no dynamic allocation solely for this slice;
- identical record -> identical bytes/text;
- exact 64-bit timestamp preservation;
- non-finite numeric values fail explicitly;
- invalid/inconsistent sample state fails explicitly;
- unavailable/unsupported/pending/unknown samples contain no fabricated value;
- strings have deterministic escaping/length semantics;
- output contains no raw CAN ID, UDS DID, ECU route or brand mapping detail.

A compact textual line or binary record is acceptable only if the representation is fully deterministic and host-round-trippable. The chosen representation must be documented as an export format, not a new canonical VehicleData domain model.

## Host parser / round-trip

Provide a bounded host-side parser/decoder or equivalent deterministic test harness that proves:
- numeric value preservation;
- boolean value preservation;
- text value preservation;
- explicit no-value states;
- unit/source/quality/availability preservation;
- exact timestamp preservation;
- malformed input rejection;
- deterministic serialization;
- capacity-boundary behavior.

The parser is test tooling / export consumer evidence; it does not become runtime persistence infrastructure.

## Explicitly deferred persistence decisions

Do not implement or choose:
- SD card / SPIFFS / LittleFS / NVS storage;
- filesystem path/layout;
- file rotation;
- retention limits;
- flush/fsync policy;
- buffering queue;
- background logger task;
- Scheduler integration;
- compression;
- upload/cloud transport;
- live streaming;
- CSV/JSON database schema beyond the bounded chosen record representation;
- replay into VehicleData Store.

These require a separate integration Stage with actual storage/runtime requirements.

## Deterministic host-test minimum

- numeric / boolean / text samples;
- Available sample with value;
- Unsupported / Unavailable / Pending / Unknown samples with no fabricated value;
- ValidCurrent / Stale / InvalidOrUnknown quality preservation;
- OBD / UDS / PassiveCan / Derived / Unknown source/provenance preservation;
- exact `uint64_t` timestamp near upper range;
- empty and maximum-length bounded strings as applicable;
- deterministic repeated serialization;
- exact capacity and one-byte-short failure;
- malformed/truncated/invalid-version input rejection;
- non-finite numeric rejection;
- round-trip semantic equality;
- source/API inspection confirms no filesystem, NVS, Arduino/ESP32 storage dependency, diagnostic transport, OBD/UDS, ReadOnlyGuard, Scheduler, brand raw mapping, global mutable singleton or second signal-identity registry;
- all existing host regressions PASS;
- `git diff --check` PASS.

## Validation boundary

This Stage may establish:
- host-side normalized export contract PASS;
- deterministic serializer/parser PASS.

It does not establish:
- persistent logger runtime;
- ESP32 filesystem/storage compile or runtime;
- retention/rotation behavior;
- background logging;
- physical hardware storage;
- Bench / Hardware / Vehicle PASS.

`VEHICLE_CONFIRMED = none` remains unchanged.
