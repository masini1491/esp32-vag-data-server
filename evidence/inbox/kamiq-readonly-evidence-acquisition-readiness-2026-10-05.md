# Kamiq read-only evidence acquisition protocol readiness — 2026-10-05

Status: READY_FOR_DOCS_ONLY_PROTOCOL
Authority role: sanitized readiness evidence only; no physical PASS and no execution authority

## Baseline

Reviewed at project main:
- `480efeaa6366750dd8dd871368bcad43a7aa4df2`

Current state:
- Phase 6a profile identity/manual selection: complete.
- Phase 6b capability/normalized signal registry: complete.
- Phase 6c Deep Diagnostic on-demand: evidence-blocked.
- `TASKS.md = EMPTY`.
- Bench / Hardware / Vehicle remain Pending.
- `VEHICLE_CONFIRMED = none`.

Current hardware status remains incomplete:
- exact ESP32 board: TBD;
- CAN transceiver: TBD;
- GPIO configuration: TBD.

Therefore this work must not be framed as Phase 10 Vehicle PASS execution.

## Why a research protocol is useful now

Phase 6c is blocked on profile-owned concrete Deep Diagnostic evidence, not on generic Scheduler/UDS plumbing.

The repository already has enough local methodology evidence to define a safe acquisition process:
- MQB-sniffer: known diagnostic tool + splitter/sniffer + displayed-value/raw-traffic correlation;
- Telltale: preserve request/response/source/status provenance, fail closed, do not infer Unsupported from one failed observation;
- current read-only policy: no coding/adaptation/clear-DTC/security-access/output-test/routine-control/flashing/actuator control;
- current Kamiq note: route/DID/scaling/SFD/gateway/passive visibility remain Pending.

A canonical research protocol can therefore be written before any hardware/vehicle PASS exists.

## Protocol purpose

Create a repeatable, repo-safe process to turn a real-vehicle read-only observation into evidence that can later support:
- VAG/Kamiq route hypotheses;
- exact read-only DID/service identification;
- raw response validation;
- decode/scaling/unit derivation;
- capability support decisions;
- Phase 6c promotion;
- later Phase 10 vehicle validation.

The protocol itself establishes no vehicle fact.

## Safety hierarchy

### Preferred path A — passive observation of a known diagnostic tool

Use a legitimate/known diagnostic tool that already displays the target read-only measuring value.

Topology concept:
`vehicle OBD -> splitter / passive observation point -> known diagnostic tool`

Observe traffic without injecting new project-originated requests.

This is the preferred route for unknown VAG/SRS mappings because it avoids DID brute force.

### Path B — project-originated active read only after the exact request is known

Only after a specific read-only request has already been captured/established and reviewed may the project later consider replaying it through its own stack.

All such active diagnostic TX must:
- remain within an already-authorized read-only semantic;
- pass through `ReadOnlyGuard`;
- preserve target route/DID provenance;
- use bounded cadence;
- stop on ambiguity, unexpected response, gateway restriction or safety concern.

The acquisition protocol does not itself authorize active replay.

## Forbidden acquisition methods

Do not:
- brute-force DID/address ranges on a live vehicle;
- probe SRS/Airbag by sweeping requests;
- use Coding, Adaptation, Clear DTC, SecurityAccess, RoutineControl, Output Tests, Basic Settings, flashing or actuator control;
- infer a route/DID from another MQB vehicle and transmit it to Kamiq merely because the platform is similar;
- bypass the project ReadOnlyGuard for project-originated TX;
- treat one timeout / NO DATA / negative response as proof of Unsupported;
- publish full VIN or other unnecessary identifying vehicle data in the repository.

## Evidence record minimum

Every candidate observation intended for repository use should preserve, where available:

### Vehicle / session context
- sanitized vehicle target identity (e.g. Kamiq 2024 facelift / `Kamiq_NW4`);
- exact date/session identifier;
- ignition state;
- engine state;
- relevant operating condition for the observed value;
- diagnostic tool name/version;
- capture tool/topology;
- whether observation was passive-only or included a known authorized project TX.

### Route / protocol provenance
- physical/link context if known;
- request source / target addressing or route if observable;
- diagnostic service;
- exact request bytes for the candidate read;
- exact response bytes;
- response source/controller identity if observable;
- response status/NRC and timing notes;
- no inferred route field may be silently promoted to confirmed.

### Semantic correlation
- normalized target signal/capability candidate;
- value shown by the known diagnostic tool;
- raw payload segment believed to correspond to that value;
- candidate decode/scaling formula;
- normalized unit;
- before/after or repeated observations used to test the candidate;
- ambiguity/conflict notes;
- whether multiple controllers responded.

### Evidence classification
- observation count;
- repeatability;
- provenance source;
- candidate / hypothesis / confirmed-for-this-session classification;
- what remains unknown;
- whether evidence is sufficient for source-code admission.

## Repository sanitization

Repository evidence must be sanitized:
- do not commit full VIN unless explicitly required and approved;
- mask or omit unnecessary serial numbers, owner-identifying data, location history or account identifiers;
- raw diagnostic bytes may be stored only when they are necessary engineering evidence and do not carry unnecessary personal data;
- preserve enough provenance to reproduce the engineering conclusion without exposing unrelated personal data.

## Minimum correlation standard

A proposed route/DID/scaling mapping should not be promoted from a single unexplained response.

For a numeric measuring value, prefer at least:
- one known displayed value + raw response pair;
- a second controlled observation where the displayed value materially changes;
- the same candidate decode/scaling explains both;
- no conflicting responder/source ambiguity remains unresolved.

For discrete/status values:
- observe at least two known states where safely possible;
- if state change cannot be induced safely, keep the mapping as hypothesis/Pending rather than manufacturing confidence.

Safety overrides the desire for multiple states.

## First-session sequence

1. **Capture-path preflight**
   - verify the passive observation topology can record traffic without altering the vehicle;
   - record tool versions and connection topology;
   - do not begin with SRS brute-force exploration.

2. **Known low-risk baseline**
   - capture at least one already-understood read-only diagnostic exchange (for example VIN or a generic live-data value) to validate timestamps, direction, request/response correlation and responder provenance.

3. **One known VAG measuring value**
   - choose a value that the legitimate diagnostic tool can explicitly display;
   - passively capture the exact request/response;
   - correlate displayed value with raw payload;
   - repeat if a safe value change is available.

4. **Deep Diagnostic candidate**
   - only after the capture path is proven;
   - prefer a specifically selected read-only measuring value;
   - for SRS/pretensioner work, observe what the known tool requests rather than sweeping unknown DIDs.

5. **Post-session classification**
   - record candidate route/DID/scaling separately;
   - do not update capability to Supported or admit runtime mapping until evidence review is complete.

## Phase 6c promotion threshold

Phase 6c may be promoted from Cold only when at least one concrete consumer has sufficient evidence to define:
- selected profile;
- normalized capability/signal identity;
- target ECU/route;
- exact authorized read-only service + DID;
- raw response validation;
- deterministic decode/scaling + unit;
- bounded failure/status semantics;
- provenance/evidence classification;
- no unresolved multi-responder ambiguity;
- continued ReadOnlyGuard enforcement.

A route/DID may still be marked hypothesis or Pending if physical confirmation is incomplete, but implementation authority must not exceed the evidence class.

## Relation to Phase 10

This protocol is a research/evidence-acquisition precursor, not Phase 10 PASS.

Phase 10 still requires actual project hardware/vehicle evidence for the minimum dataset:
- VIN;
- `vehicle.speed`;
- `vehicle.rpm`;
- `vehicle.coolantTemp`;
- `vehicle.voltage`.

Full Vehicle PASS additionally requires the relevant end-to-end project path, not merely observation through an external diagnostic tool.

## Readiness conclusion

A docs-only canonical evidence-acquisition protocol is admissible now.

No capture tooling, firmware TX, hardware selection or vehicle testing is authorized by this readiness note.
