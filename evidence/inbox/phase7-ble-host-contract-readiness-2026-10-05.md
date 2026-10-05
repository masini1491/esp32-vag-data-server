# Phase 7 BLE host-side contract readiness — 2026-10-05

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 8 canonical closure:
- project main: `f9b1f7e8075003697043f6a88ccd19769314431c`
- `TASKS.md = EMPTY`
- application-facing VehicleData read model is complete;
- Phase 8 Web host contract is complete;
- BLE remains a planned normalized VehicleData client;
- Bench / Hardware / Vehicle remain Pending.

## Readiness conclusion

Phase 7 may proceed as a host-only logical BLE application contract without physical radio, ESP32 BLE stack or negotiated ATT/MTU assumptions.

The first slice should define deterministic, bounded logical payloads sourced only from the application read model.

## First-slice contract

Expose logical read-only BLE application records for:
1. snapshot metadata:
   - schema/version;
   - profile selection state;
   - optional active profile identity;
   - capability count;
   - sample count;
2. capability record by index:
   - normalized signal identity;
   - expected value type;
   - unit;
   - CapabilitySupport;
3. sample record by index:
   - normalized signal identity;
   - value presence/type/value;
   - unit;
   - source;
   - quality;
   - availability;
   - exact 64-bit timestamp.

The logical records may later be mapped to GATT characteristics/read/notify operations, but this Stage must not freeze physical UUIDs, BLE library choice, MTU, connection interval or notification scheduling.

## Source-of-truth boundary

All BLE-facing data must come only from the existing application-facing VehicleData read model / copied snapshot.

Do not:
- access VehicleDataStore internals directly;
- access OBD/UDS/ReadOnlyGuard;
- trigger Scheduler jobs;
- query ECU on client read;
- expose raw CAN IDs, UDS DIDs, ECU routes or VAG/Kamiq mapping internals.

## Payload semantics

Use a deterministic bounded representation suitable for future BLE characteristic values.

Requirements:
- caller-provided fixed buffer;
- explicit required-capacity result on insufficient capacity;
- no silent truncation;
- no dynamic allocation solely for this slice;
- deterministic byte output for identical input;
- preserve Pending / Unknown / Unsupported / Unavailable distinctly;
- absent value must not be serialized as zero/false/empty;
- exact 64-bit timestamps must not lose precision;
- malformed or inconsistent snapshot input fails explicitly;
- payload contract must be versioned.

Binary or compact textual encoding may be selected by implementation if it satisfies the bounded deterministic contract. Do not reuse Web JSON merely for convenience unless its size/semantics are explicitly justified for the BLE logical record; BLE is a separate client transport contract, not a second domain model.

## Host mock

Provide a host-side mock/client decoder or test harness sufficient to prove:
- metadata record round-trip/interpretation;
- capability record semantics;
- numeric / boolean / text sample semantics;
- explicit no-value states;
- exact timestamp preservation;
- deterministic output;
- insufficient-capacity failure;
- invalid input rejection.

The mock is not BLE radio evidence.

## Explicitly deferred physical BLE decisions

Do not decide or claim:
- ESP32 BLE library;
- NimBLE vs other stack;
- service/characteristic UUIDs;
- negotiated MTU;
- fragmentation/reassembly strategy tied to a real ATT MTU;
- notify/indicate scheduling;
- connection interval;
- pairing/bonding/security policy;
- radio coexistence;
- throughput/power behavior.

These require an actual BLE integration Stage and, where applicable, ESP32/hardware evidence.

## Deterministic host-test minimum

- unresolved profile metadata;
- selected profile metadata;
- four Kamiq_NW4 Pending capability records;
- empty sample set;
- numeric, boolean and text samples;
- Available vs Unsupported / Unavailable / Pending / Unknown;
- exact 64-bit timestamp preservation;
- escaping/length handling for bounded strings as applicable;
- deterministic encoded bytes;
- capacity boundary and one-byte-short failure;
- malformed/inconsistent input rejection;
- repeated encoding has no side effects;
- source/API inspection confirms no diagnostic transport, OBD/UDS, ReadOnlyGuard, Scheduler, Arduino/ESP32/BLE stack dependency, brand raw mapping or global mutable singleton;
- all existing host regressions PASS.

## Validation boundary

This Stage may establish:
- host-side BLE application contract PASS;
- deterministic encoder/mock PASS.

It does not establish:
- ESP32 BLE compile;
- BLE radio runtime;
- GATT interoperability;
- smartphone compatibility;
- throughput;
- Bench / Hardware / Vehicle PASS.

`VEHICLE_CONFIRMED = none` remains unchanged.
