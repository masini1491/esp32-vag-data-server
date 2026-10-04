# Phase 6b capability + normalized signal registry readiness — 2026-10-04

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 6a canonical closure:
- project main: `4a60c09979f30593ef2542cae52abaa49c48e2ef`
- Phase 6a provides opaque ProfileIdentity, Active Vehicle Profile manual selection, and one Pending `Kamiq_NW4` profile descriptor.
- Existing `NormalizedVehicleSample::SignalId` is the canonical normalized signal identity representation.
- Existing `VehicleAvailability` is runtime sample availability and already contains Available / Unsupported / Unavailable / Pending / Unknown semantics.
- Current Phase 6 canonical semantics require capability support authority to remain distinct from transient runtime availability.

## Key design decision

Phase 6b must **not** reuse `VehicleAvailability` as capability support authority.

Why:
- `VehicleAvailability` describes the state of a concrete normalized observation/sample.
- Capability support answers whether the selected profile has authoritative support knowledge for a normalized signal.
- A timeout / NO DATA / malformed response / temporary route failure can change runtime observation state without proving capability Unsupported.
- Reusing one enum/state owner for both would make the forbidden transition "failed read -> Unsupported capability" too easy and semantically ambiguous.

Therefore Phase 6b needs a separate capability-support type owned by the profile/capability layer.

## Existing canonical identity to reuse

Do not create a second signal-ID type.

Use the existing bounded normalized signal identity semantics from:
- `NormalizedVehicleSample::SignalId`
- current canonical namespace examples in `docs/ARCHITECTURE.md`

The first slice may use normalized IDs already named by canonical project documents.

## Representative non-speculative consumer

A registry no longer needs to be empty/speculative because the repository now has:
- one concrete admitted Active Profile identity: Pending `Kamiq_NW4`;
- canonical normalized signal examples;
- a Phase 10 minimum vehicle-validation dataset;
- upstream Kamiq 2024 signal-description coverage evidence.

However, upstream signal-description coverage is not vehicle support confirmation.

Therefore the first Kamiq registry entries must remain **Pending**, not Supported.

Minimum representative candidate set:
- `vehicle.speed`
- `vehicle.rpm`
- `vehicle.coolantTemp`
- `vehicle.voltage`

These correspond to the current minimum first Vehicle PASS dataset except VIN, which is text/identity-oriented and can be deferred if adding it would expand this slice.

The above entries are normalized capability candidates only. They do not establish:
- ECU route;
- DID;
- CAN ID;
- scaling;
- source priority;
- polling policy;
- runtime availability;
- Hardware/Vehicle PASS.

## Phase 6b first-slice ownership

### Separate support state

Minimum support states:
- `Supported`
- `Unsupported`
- `Pending`
- `Unknown`

Semantics:
- `Supported`: profile authority explicitly admits support; this Stage should not mark Kamiq candidates Supported without qualifying evidence.
- `Unsupported`: profile authority explicitly states absence; never inferred from one failed observation.
- `Pending`: candidate is known/relevant but not yet validated.
- `Unknown`: no authoritative support determination.

No transient `Unavailable` state belongs in capability support; Unavailable remains runtime/sample availability.

### Normalized signal descriptor

Minimum descriptor responsibility:
- normalized signal identity using existing SignalId semantics;
- expected value type;
- normalized unit;
- capability support state.

Do not include in the first slice:
- ECU route;
- DID;
- CAN frame ID/bit;
- raw encoding/scaling formula;
- source priority;
- polling cadence;
- Scheduler job;
- diagnostic callback/executor;
- physical validation result.

### Registry ownership

Use fixed / compile-time bounded data for the first slice.

The selected profile may expose/query its admitted normalized capability descriptors, but:
- profile selection itself does not mutate support state;
- failed runtime reads cannot mutate support state;
- registry does not emit VehicleData samples;
- registry does not write VehicleDataStore;
- registry does not choose among conflicting ECU responses.

A query for an unregistered normalized signal must return a deterministic not-found / unknown-to-registry result rather than fabricating Unsupported.

## Kamiq_NW4 first slice

The compile-time Kamiq 2024 capability registry may contain the representative normalized signals above, all with support state `Pending`.

This is justified as candidate coverage/readiness evidence only and must remain explicitly not vehicle-confirmed.

No route/scaling/source mapping is admitted in Phase 6b.

## Deterministic host-test minimum

- support enum is distinct from `VehicleAvailability`;
- registry contains only the admitted fixed descriptors;
- lookup succeeds for each admitted normalized signal;
- unknown signal returns deterministic not-found and does not become Unsupported;
- every initial Kamiq_NW4 candidate remains Pending;
- descriptor identity/value-type/unit are stable and bounded;
- selecting Kamiq_NW4 does not promote Pending entries to Supported;
- registry lookup does not create/update `NormalizedVehicleSample` or `VehicleDataStore`;
- no API accepts runtime read failure / timeout / NO DATA as a support-state mutation;
- source/API inspection confirms no route/DID/CAN/scaling/source-priority/polling/Scheduler/diagnostic-execution ownership, no Arduino/ESP32/FreeRTOS dependency and no global mutable singleton.

## Explicit non-goals

Do not implement:
- runtime availability tracking;
- automatic support learning;
- support mutation from diagnostic results;
- route/source arbitration;
- ECU routing;
- DID/scaling/passive CAN mappings;
- polling policy;
- Scheduler integration;
- profile parser/schema/storage;
- Deep Diagnostic;
- physical support claims.

## Readiness conclusion

Phase 6b is admissible as a bounded generic capability/signal metadata slice because:
- canonical signal identity already exists;
- support-vs-runtime availability semantics are now canonical;
- Phase 6a provides a real profile consumer;
- the registry can carry only Pending candidate metadata without inventing vehicle mappings.

This avoids both failure modes:
- an empty speculative registry framework;
- false promotion of upstream signal-description coverage into confirmed support.
