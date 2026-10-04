# Telltale diagnostic/data-quality patterns

Upstream: [aa22396584/telltale](https://gitlab.com/aa22396584/telltale)
Reviewed: 2026-10-04
Role: Phase 6 profile/capability semantics; Phase 8 diagnostic provenance direction; Phase 10 validation method
Evidence class: `REFERENCE_PATTERN`
Reuse status: pattern/method/data-pipeline reference only; no source reuse
License / provenance: sanitized evidence reports GPL-3.0 disclosure; do not copy implementation code.

## Transferable findings

- One failed observation, timeout or `NO DATA` does not establish capability `Unsupported`. Keep support authority separate from transient availability.
- Conflicting answers from multiple controllers remain ambiguous until an explicit route/source arbitration authority exists. Arrival order and numerical plausibility are not authority.
- Malformed data, unsupported decode/scaling, out-of-range results or insufficient semantics fail closed; they must not become plausible normalized values through guessed or default values.
- A later bounded diagnostic trace may correlate request/route, raw response, source, decode outcome and normalized status for validation. This is not a requirement for unbounded firmware logging.
- Discovery and simulation do not authorize live actuation or broaden diagnostic TX permission; the project's existing `ReadOnlyGuard` remains authoritative.

## Project-specific boundary

These patterns inform canonical profile/data-quality semantics only. They do not establish any Kamiq route, DID, CAN ID, scaling, SFD behavior, passive-CAN visibility, hardware behavior or vehicle support. `VEHICLE_CONFIRMED = none`; Bench, Hardware and Vehicle evidence remain Pending.

## Do not infer / do not reuse

Do not copy GPL-3.0 source. Do not treat a single failed read as capability absence, select a conflicting ECU response without explicit authority, emit normal-looking values from uncertain decoding, add unbounded firmware capture, or infer live-vehicle TX authorization from discovery/simulation support.
