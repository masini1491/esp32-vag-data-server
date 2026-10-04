# Phase 6c Deep Diagnostic on-demand readiness — 2026-10-05

Status: BLOCKED_ON_CONCRETE_PROFILE_EVIDENCE
Authority role: sanitized readiness evidence only; no execution authority

## Baseline

Reviewed after Phase 6b canonical closure:
- project main: `7fdb5506ef071a60aca8c1f41dc6b0e4ab087a35`
- `TASKS.md = EMPTY`
- Phase 6a profile identity/manual Active Profile selection is complete.
- Phase 6b capability + normalized signal metadata registry is complete.
- Existing Generic Core already provides:
  - cooperative Scheduler with `OnDemand`;
  - `ReadOnlyGuard::startUdsReadDataByIdentifier()`;
  - Generic UDS `0x22` one-request-at-a-time service;
  - normalized VehicleData and Store.

## Readiness question

Can Phase 6c implement a non-speculative Deep Diagnostic on-demand path without concrete Kamiq route/DID/scaling evidence?

Conclusion: **No.**

The missing piece is not generic scheduling or UDS transport. The missing owner is the profile-specific diagnostic mapping required to turn a normalized Deep Diagnostic capability into a real read-only vehicle request and decode result.

## What is already reusable

The following infrastructure is sufficient once a real profile mapping exists:

`client/capability request -> Scheduler OnDemand -> profile-owned diagnostic target -> ReadOnlyGuard -> UdsService 0x22 -> raw response -> profile decoder -> normalized result/status`

Existing scheduler semantics already provide:
- dormant OnDemand job;
- explicit request;
- no duplicate pending/active execution;
- one active job at a time;
- externally reported completion.

Existing UDS/guard semantics already provide:
- semantic single-DID `0x22` request;
- read-only guard construction of the request;
- positive `0x62` matching;
- terminal negative NRC;
- bounded NRC `0x78`;
- transport failure/timeout behavior.

Therefore Phase 6c does not need another generic executor framework merely to demonstrate these existing pieces.

## Missing concrete authority

No current canonical evidence establishes for Kamiq 2024:
- target ECU / diagnostic route;
- exact read-only DID for any Deep Diagnostic capability;
- raw payload layout;
- decode/scaling;
- unit/physical interpretation;
- session/gateway/SFD accessibility;
- capability state strong enough to promote a concrete Deep Diagnostic mapping to Supported.

Examples still Pending:
- J234/SRS measuring-value route;
- driver/passenger pretensioner resistance DID/scaling;
- igniter/circuit status mapping;
- extended ACC/ABS read-only values;
- extended ECU identification beyond current generic capabilities.

`vehicle_coverage` confirms only signal-description categories and cannot supply the missing route/DID/scaling authority.
MQB-sniffer supplies only a read-only research method; Golf observations are not Kamiq evidence.
No source may be promoted into runtime mapping merely because a similar VAG platform uses it.

## Why a generic Phase 6c shell is rejected

A shell that only binds Scheduler OnDemand to an abstract diagnostic callback/request object would add:
- executor/callback ownership with no real consumer;
- speculative route/request abstraction;
- duplicate orchestration around already-existing Scheduler/UdsService semantics;
- no additional vehicle capability.

That would violate the repository's no-speculative-abstraction rule and the Phase 6 decomposition STOP boundary.

## Promotion trigger

Phase 6c may be reconsidered when at least one concrete Deep Diagnostic consumer has sufficient legal/read-only evidence to define:

1. selected profile identity;
2. normalized capability/signal identity;
3. target ECU/route;
4. exact read-only service + DID (or other already-authorized read-only semantic);
5. raw response validation;
6. deterministic decode/scaling + unit;
7. expected failure/status mapping;
8. evidence classification and explicit Pending/Supported boundary;
9. confirmation that all active diagnostic TX remains behind `ReadOnlyGuard`.

Preferred first candidates:
- a bounded SRS/pretensioner read-only measuring value if exact route/DID/scaling can be established safely;
- otherwise another low-risk, concrete VAG Deep Diagnostic value with stronger evidence.

Physical vehicle confirmation is not necessarily required to write a hypothesis/test fixture, but production admission of a Kamiq-specific runtime mapping must not exceed the evidence authority available at that time.

## Research path when trigger is pursued

Use the existing local methodology:
- known read-only diagnostic tool + splitter/sniffer where legal/safe;
- select one value;
- capture request/response;
- correlate raw/displayed value;
- repeat across controlled observations;
- preserve route/source provenance;
- fail closed on ambiguity;
- never use coding/adaptation/output-test/security-access or destructive services.

Do not brute-force SRS/Airbag DIDs on a live vehicle.

## Current disposition

- Phase 6a: complete.
- Phase 6b: complete.
- Phase 6c: blocked / Cold until concrete profile-owned Deep Diagnostic evidence exists.
- `TASKS.md` should remain EMPTY.
- Bench / Hardware / Vehicle remain Pending.
- `VEHICLE_CONFIRMED = none`.
