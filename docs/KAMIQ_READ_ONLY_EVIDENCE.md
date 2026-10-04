# Kamiq Read-only Evidence Acquisition Protocol

Status: `PROTOCOL_SPECIFICATION_ONLY` — no physical observation or validation evidence was acquired by this document.

## Purpose and authority

This SOP defines how future, separately authorized research can record read-only diagnostic observations for the 2024 Škoda Kamiq facelift (`Kamiq_NW4`). It is an evidence-acquisition precursor for Phase 6c and later Phase 10 validation. It establishes no vehicle fact, mapping, capability support, or execution authority. Bench, Hardware, and Vehicle remain Pending; `VEHICLE_CONFIRMED = none`.

Use the existing profile, capability, read-only, and validation authorities linked from [REFERENCES.md](REFERENCES.md), [Vehicle Profile](VEHICLE_PROFILE.md), and [Read-only Diagnostic Policy](READ_ONLY_POLICY.md). Preserve every conclusion at the evidence scope that supports it.

## Acquisition hierarchy

### Path A — passive observation of a known diagnostic tool

Prefer observing a legitimate, known diagnostic tool that already displays the selected read-only value, through a passive observation point such as an OBD splitter. Record traffic without injecting project-originated requests. Confirm before the session that the observation topology does not alter vehicle communication. This is the preferred method for unknown VAG/SRS routes because it does not sweep or guess requests.

### Path B — project-originated active read

A future project-originated read may be considered only after the exact read-only request and target route are already known and reviewed, and a separate task authorizes that execution. Every request must use the existing semantic `ReadOnlyGuard` path, remain within its authorized allowlist, use bounded cadence, and retain route/request provenance. Stop on ambiguity, unexpected responses, gateway restrictions, or safety concerns.

This SOP does not authorize Path B or grant TX permission. The existence of a captured request, test fixture, request builder, or protocol implementation does not grant that permission.

## Forbidden methods

- Do not brute-force live DID or address ranges; do not sweep SRS/Airbag requests.
- Do not use Coding, Adaptation, Clear DTC, SecurityAccess, RoutineControl, Output Tests, Basic Settings, flashing, or actuator control.
- Do not copy another MQB vehicle's route, DID, CAN identifier, or scaling into a Kamiq profile because the platform looks similar.
- Do not bypass `ReadOnlyGuard` or send an unreviewed project-originated request.
- Do not promote a single timeout, `NO DATA`, negative response, malformed response, or failed route to capability `Unsupported`.
- Do not commit a full VIN or unnecessary personal, owner, location, serial-number, or account data. A full VIN requires a separately established need and explicit approval.

## Evidence record

Create one sanitized record per session. Use `Unknown`, `Pending`, or `Not observed` when a field is unavailable; do not fill gaps by inference.

### Vehicle and session context

- sanitized target identity (for example, Kamiq 2024 facelift / `Kamiq_NW4`);
- date and session identifier;
- ignition/engine state and relevant operating condition;
- diagnostic tool name/version;
- capture tool and observation topology;
- Path A passive observation or separately authorized Path B, with authorization reference if applicable.

### Route and protocol provenance

- physical/link context when known;
- request source, target addressing, and route, each labeled observed, inferred, or unknown;
- service and DID when known;
- exact request and response bytes when available and safe to retain;
- response source/controller identity, status/NRC, direction, and timing when observable;
- any conflicting or unidentified responder.

### Value correlation

- normalized signal/capability candidate;
- diagnostic tool's displayed value and observation conditions;
- raw response field believed to represent that value;
- candidate decode/scaling and normalized unit, each with its evidence status;
- each repeated observation and whether the displayed value materially changed;
- responder ambiguity, decode uncertainty, and all unresolved questions.

### Evidence disposition and privacy

- observation count and repeatability;
- provenance source and capture mode;
- scope-limited classification (candidate/hypothesis or session-correlated observation);
- remaining unknowns and whether the record meets a later implementation review threshold;
- sanitization performed and any fields intentionally omitted.

Raw bytes belong in the repository only when necessary to reproduce the engineering conclusion and free of unnecessary personal data. Preserve useful provenance while removing unrelated identifying information.

## Correlation and promotion thresholds

A single unexplained response is not enough to promote a route/DID/scaling mapping.

For a numeric value, prefer two materially different displayed-value observations that the same candidate decode/scaling explains, with request/response provenance and no unresolved responder conflict. For a discrete/status value, prefer more than one known state when safely available. Do not cause an unsafe state change to obtain another sample; if sufficient observations are unavailable, keep the candidate as a hypothesis/Pending.

Before a Phase 6c implementation can be considered, review evidence for all of the following:

- selected profile and normalized capability/signal identity;
- target ECU/route and exact authorized read-only service/DID;
- validated raw response and deterministic decode/scaling/unit;
- bounded failure/status semantics;
- provenance and evidence classification;
- no unresolved multi-responder ambiguity;
- continued enforcement through `ReadOnlyGuard`.

Evidence may support a scope-limited hypothesis while physical confirmation remains Pending. Runtime admission must not claim more than the evidence establishes. A failed observation never establishes `Unsupported` by itself.

## Future first-session sequence

This sequence describes a future, separately approved acquisition session; this Stage performs none of these operations.

1. **Capture-path preflight:** verify passive observation can record without altering vehicle communication; record versions and topology. Do not begin with SRS/DID exploration.
2. **Known low-risk baseline:** passively record an already-understood read-only exchange to check direction, timestamps, request/response association, and responder provenance.
3. **One known VAG measuring value:** choose a value explicitly displayed by the known tool; record its request/response and correlate raw/displayed values. Repeat only when a safe materially different observation is available.
4. **One Deep Diagnostic candidate:** only after the capture path works; select a specific read-only measuring value. For SRS/pretensioner research, observe the known tool's request instead of sweeping unknown DIDs.
5. **Post-session review:** classify each candidate, list uncertainty, sanitize the record, and review it before considering profile/runtime admission.

## Relation to Phase 10

Observation through an external tool is research evidence, not end-to-end project Vehicle PASS. Phase 10 still requires real project hardware/vehicle evidence for VIN, `vehicle.speed`, `vehicle.rpm`, `vehicle.coolantTemp`, and `vehicle.voltage`; full Vehicle PASS also requires the applicable end-to-end project path. This SOP supplies no such evidence and does not change any validation status.
