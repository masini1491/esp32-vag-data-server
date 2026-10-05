# Phase 4C UDS ReadDTCInformation readiness — 2026-10-05

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 3C canonical closure:
- project main: `1915e2d3755976149d374e04b3fc751f29377f0b`
- `TASKS.md = EMPTY`
- Generic OBD-II Mode `0x03` stored-DTC read is complete.
- Generic UDS `0x22 ReadDataByIdentifier` guard/service core is complete.
- Current read-only policy explicitly permits future read-only `0x19 ReadDTCInformation` when needed.
- `0x14 ClearDiagnosticInformation`, SecurityAccess, write/control services and actuator behavior remain prohibited.

## Readiness conclusion

The Cold trigger for a bounded Generic UDS `0x19` slice is satisfied.

The minimum non-speculative first slice is:

`ReadDTCInformation (0x19) / reportDTCByStatusMask (subfunction 0x02)`

Why this slice:
- it is read-only and explicitly inside the current policy boundary;
- it returns actual bounded DTC/status records instead of creating a shell framework;
- it can reuse the existing one-request-at-a-time UDS lifecycle, timeout, transport failure, terminal NRC and bounded NRC `0x78` behavior;
- it does not require VAG/Kamiq-specific DTC definitions, ECU routes, sessions or vehicle evidence.

## Request contract

Add a dedicated semantic guard entry point for exactly:

`19 02 <DTCStatusMask>`

The caller may supply only the one-byte status mask.

The guard constructs the service/subfunction bytes itself.

Do not add:
- generic raw UDS request API;
- caller-selected `0x19` subfunction;
- `0x14 ClearDiagnosticInformation`;
- session/security/write/control operations.

## Positive response contract

Expected positive response shape:

`59 02 <DTCStatusAvailabilityMask> [DTCAndStatusRecord...]`

Each admitted record is exactly:
- 3 raw DTC bytes;
- 1 raw status byte.

First-slice semantics:
- preserve the response `DTCStatusAvailabilityMask`;
- preserve DTC/status bytes without SAE/VAG textual decoding;
- zero records are valid when the response contains only service + subfunction + availability mask;
- record area length must be an exact multiple of four;
- fixed/bounded storage only;
- over-capacity response fails closed; no silent truncation;
- do not treat all-zero DTC bytes as padding unless future authority explicitly requires that semantic;
- no VehicleData conversion.

## Request/response matching

The service must:
- match positive service `0x59`;
- match subfunction `0x02`;
- preserve one-outstanding-request / Busy behavior;
- reject malformed positive response as `InvalidResponse`;
- treat unrelated positive service/subfunction as `UnexpectedResponse` without accepting it as the current result.

## Negative response / response-pending

Existing UDS negative-response lifecycle should be generalized only as much as necessary for the active semantic request:
- terminal negative response shape: `7F 19 <NRC>`;
- bounded response-pending: `7F 19 78`;
- wrong negative-response service remains `UnexpectedResponse`;
- malformed negative response remains fail closed;
- existing `0x22` behavior must remain unchanged.

Do not build a generic arbitrary service executor merely to share code.

## Bounded record capacity

The current UDS response buffer is fixed at 64 bytes.

For the `0x59 0x02` shape, the first three bytes are response metadata and each record consumes four bytes. The implementation should therefore use an explicit fixed record capacity compatible with the existing bounded response buffer and fail closed when the admitted record capacity would be exceeded.

No dynamic allocation is required.

## Deterministic host-test minimum

- guard constructs exactly `19 02 <mask>`;
- guard API does not accept arbitrary subfunction/service bytes;
- existing `0x22` guard semantics remain unchanged;
- one outstanding request / Busy semantics;
- valid zero-record response;
- one and multiple four-byte records;
- preserves availability mask and raw three-byte DTC + status bytes;
- wrong positive service -> UnexpectedResponse;
- wrong positive subfunction -> UnexpectedResponse;
- response shorter than service/subfunction/availability -> InvalidResponse;
- misaligned record area -> InvalidResponse;
- fixed-capacity overflow -> fail closed;
- terminal `7F 19 NRC`;
- bounded `7F 19 78` response-pending behavior;
- wrong negative-response service -> UnexpectedResponse;
- timeout and existing transport failure coverage;
- all existing OBD, UDS `0x22`, VehicleData, Scheduler and Phase 6 regressions PASS.

## Explicit non-goals

Do not implement:
- any other `0x19` subfunction;
- `0x14 ClearDiagnosticInformation`;
- DTC text/database decoding;
- VAG/Kamiq-specific DTC meaning;
- ECU route discovery;
- session switching;
- TesterPresent;
- SecurityAccess;
- application-facing polling;
- Scheduler integration;
- VehicleData mapping;
- hardware/vehicle claims.

## Evidence boundary

This Stage is generic protocol semantics only.

It creates no evidence for:
- Kamiq support;
- ECU routing;
- DTC presence on a real vehicle;
- Hardware PASS;
- Vehicle PASS.

Bench / Hardware / Vehicle remain Pending and `VEHICLE_CONFIRMED = none`.
