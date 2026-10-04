# Telltale diagnostic reference patterns — 2026-10-04

Status: REFERENCE_PATTERN / sanitized external evidence
Purpose: preserve transferable diagnostic/data-quality patterns for later Phase 6 / Phase 8 / Phase 10 canonical reconciliation. This file is evidence staging only and has no execution authority.

## Source identity / provenance

User-supplied project:
- https://gitlab.com/aa22396584/telltale

Current public discovery surfaces identify the project as:
- GitLab topic listing: `ImL1s / telltale`, described as an open-source Flutter OBD2 telemetry and diagnostics app for ELM327 adapters.
- Google Play package: `com.cbstudio.telltale`.
- Public project/community descriptions state that primary development moved to GitLab; older GitHub pages may be stale or unavailable.

License / reuse boundary:
- Google Play's current open-source disclosure identifies the source as GPL-3.0.
- Treat this repository as pattern/reference evidence only unless a separate license/provenance review explicitly authorizes source reuse.
- Do not copy GPL-3.0 implementation code into this project by default.

## Transferable findings

### 1. `NO DATA` / one failed read is not proof of `Unsupported`

Telltale's current release notes explicitly state that `0100 NO DATA` is not proof that the vehicle has no OBD.

Transferable project rule:
- A single timeout, transport failure, `NO DATA`, malformed response, temporary gateway restriction or one failed route must not automatically promote a capability to `Unsupported`.
- `Unsupported` should require profile/capability authority or sufficiently strong evidence.
- Transient read failure should map to bounded runtime states such as `Unavailable`, `Unknown`, or equivalent evidence-qualified state according to the owning layer.
- Phase 6 capability/profile work should keep capability support evidence separate from one observation's availability.

Relevance:
- Phase 6a Brand/Profile boundary
- Phase 6b capability/signal registry
- Phase 10 vehicle validation

### 2. Multi-controller disagreement must remain explicit ambiguity

Telltale's public product description states that when two controllers provide different answers to the same question, it does not arbitrarily choose one.

Transferable project rule:
- Multiple responder / ECU disagreement must not be collapsed into one authoritative normalized value without an explicit route/source arbitration contract.
- Until Brand Layer / Active Profile has sufficient provenance and source-priority authority, conflicting responders remain ambiguous / unresolved evidence.
- Do not use registration order, first response, last response, numerically plausible value or transport arrival order as an implicit truth-selection policy.

Relevance:
- Phase 6 VAG ECU routing
- Phase 6b source/capability semantics
- future multi-ECU diagnostic aggregation

### 3. Decode/scaling uncertainty must fail closed instead of producing plausible values

Telltale states that suspicious/unverified data retains status/source metadata, bad packets are not silently converted into plausible readings, and unsupported formula constructs are refused rather than evaluated into a plausible wrong number.

Transferable project rule:
- Decode failure, unsupported scaling expression, malformed raw payload, out-of-range field or insufficient formula semantics must not become a normal-looking `NormalizedVehicleSample`.
- Preserve provenance / quality / availability distinction through the decode boundary.
- A decoder may reject or produce an explicit invalid/unknown status; it must not substitute zero, false, empty text, guessed scaling or an approximate formula unless that approximation is itself an explicit, separately owned semantic.
- Profile data ingestion / PC preprocessing should validate formula/scaling support before runtime admission.

Relevance:
- Phase 6 signal mapping/scaling
- future profile-builder / preprocessing tooling
- VehicleData quality semantics

### 4. Diagnostic trace should preserve request/response provenance and status

Telltale records diagnostic traffic and supports export while retaining status tags.

Transferable project direction:
- Future Logger / validation tooling should be able to correlate, where applicable:
  `request / route -> raw response -> source/provenance -> decode outcome -> normalized result/status`.
- Human-facing logs should not preserve only the final numeric value when raw/provenance evidence is needed for validation or disputed decoding.
- Logging design must remain bounded and privacy-safe; this finding does not authorize unbounded raw capture in firmware.

Relevance:
- Phase 8/Logger-related application integration
- Phase 10 bench/hardware/vehicle validation
- VAG DID/scaling investigation

### 5. Discovery/simulation evidence does not authorize live actuation

Telltale publicly distinguishes Mode 08 discovery/synthetic fixtures from live actuation and describes bounded read-only UDS use separately.

Transferable project rule:
- Simulator/test-fixture/discovery support must remain separate from live-vehicle authorization.
- Existence of a protocol service, request builder, fixture or decoder does not authorize transmitting a destructive/control request to a real vehicle.
- The project's existing `ReadOnlyGuard` remains the authority boundary; no Telltale feature expands this repository's read-only scope.

Relevance:
- permanent read-only safety policy
- future diagnostic research fixtures
- hardware/vehicle validation

## Positive corroboration of existing architecture

Telltale's public update notes say acceleration timing uses an observation clock rather than wall time.

This supports, but does not change, the project's existing use of monotonic time for protocol/runtime scheduling and VehicleData timestamps. No architecture change is required from this observation.

## Explicit non-adoptions

Do not use this reference as authority to add:
- Flutter/Riverpod/mobile application architecture to firmware;
- ELM327 Bluetooth/Wi-Fi adapter session architecture into the direct-TWAI Generic Core;
- arbitrary custom-PID runtime expression evaluation on ESP32;
- Mode 08 live actuation;
- arbitrary raw UDS TX;
- coding/adaptation/security access/output tests/flashing;
- a claim that Kamiq/VAG routes, DIDs, scaling or physical behavior are confirmed.

## Canonical reconciliation trigger

When Phase 5C is closed and Phase 6 readiness begins:
- consider adding Telltale to the canonical reference index as `REFERENCE_PATTERN`;
- prefer classifications such as `RESEARCH_METHOD_REFERENCE` / `DATA_PIPELINE_REFERENCE`;
- fold only the five transferable findings above into the minimum relevant canonical owners;
- preserve GPL-3.0 / no-direct-source-reuse boundary;
- do not turn this evidence note into a second architecture or validation authority.

## Public evidence checked

- GitLab OBD2 topic listing for `ImL1s / telltale` (current activity/discovery surface).
- Google Play listing for package `com.cbstudio.telltale` (features, uncertainty behavior, diagnostic records, current release notes, GPL-3.0 disclosure).
- Developer's public r/CarHacking release/update post (move to GitLab, read-only UDS / Mode 08 discovery distinction, observation-clock update).

No claim in this note establishes real-vehicle behavior for this repository.
