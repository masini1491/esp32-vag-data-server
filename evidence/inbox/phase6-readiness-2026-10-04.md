# Phase 6 readiness / decomposition — 2026-10-04

Status: READY_FOR_CANONICAL_RECONCILIATION; source implementation not yet admitted
Authority role: sanitized evidence / readiness staging only; no execution authority

## Baseline

Project baseline reviewed:
- `masini1491/esp32-vag-data-server@576651a316ef5e71b02fcd137a42d78d977c7e99`
- `TASKS.md = EMPTY`
- Phase 5A / 5B / 5C are canonically closed.
- Phase 6 roadmap remains:
  - Phase 6a Brand extension boundary + Brand Profile Set / Active Profile
  - Phase 6b Capability registry + normalized signal registry
  - Phase 6c Deep Diagnostic on-demand path

Shared Playbook reviewed:
- `masini1491/ai-development-playbook@3772e7e60213c99482dc201c064757075a8a48a9`

## Current local evidence read

Canonical owners:
- `docs/ARCHITECTURE.md`
- `docs/VEHICLE_PROFILE.md`
- `docs/DEVELOPMENT.md`
- `docs/REFERENCES.md`
- `docs/references/SYNTHESIS.md`

Phase-specific local references:
- `docs/references/vag/MCD_DIAG_RS.md`
- `docs/references/vag/MQB_SNIFFER.md`
- `docs/references/vag/VEHICLE_COVERAGE.md`
- `docs/references/portability/vag/KAMIQ.md`
- `evidence/inbox/telltale-reference-2026-10-04.md`

No current source symbol establishes a BrandAdapter / Brand Profile Set / Active Vehicle Profile / capability registry implementation.

## Evidence conclusions

### mcd-diag-rs

Supports the data-pipeline direction:

`legitimately available source definitions -> PC preprocessing/flattening -> trimmed profile data -> ESP32 runtime`

It does not confirm Kamiq 2024 routes, DIDs, scaling, SFD behavior or proprietary data reuse.

### vehicle_coverage / Kamiq local synthesis

Confirms only that a Kamiq 2024 upstream coverage source contains relevant signal-description categories.

It does **not** establish:
- exact ECU route;
- CAN ID;
- DID;
- raw encoding/scaling;
- diagnostic session;
- SFD/gateway availability;
- passive CAN visibility;
- any real-vehicle PASS.

Therefore Phase 6 must not seed runtime mappings from these descriptions as if they were confirmed values.

### MQB-sniffer

Supports only a read-only research methodology:
`known tool + splitter/sniffer -> select one live value -> correlate request/response/raw/display -> infer candidate -> repeat/validate`.

Golf-specific routes/DIDs are not Kamiq evidence.

### Telltale trigger reconciliation

The Phase 6 trigger recorded in `BACKLOG.md` is now active because Phase 5C is closed and Phase 6 readiness has begun.

Five transferable findings are admitted for canonical reconciliation:

1. **One failed observation / `NO DATA` is not capability Unsupported.**
   - capability support authority and runtime observation availability are separate;
   - timeout / transport failure / one route failure / malformed response must not automatically become `Unsupported`.

2. **Multi-controller disagreement remains ambiguity until explicit arbitration authority exists.**
   - do not select first response, last response, registration order or numerically plausible response as truth;
   - Brand/Profile routing must preserve responder/source provenance.

3. **Decode/scaling uncertainty fails closed.**
   - malformed payload, unsupported scaling/expression, out-of-range decode or insufficient semantics must not produce a normal-looking normalized value;
   - no zero/false/empty/guessed approximation as hidden fallback.

4. **Diagnostic trace provenance is valuable for later validation.**
   - future bounded tooling/logger should be able to correlate request/route, raw response, source, decode outcome and normalized result/status when material;
   - this does not authorize unbounded capture in firmware.

5. **Discovery/simulation does not authorize live actuation.**
   - existing `ReadOnlyGuard` remains authoritative;
   - fixture/request-builder/protocol support never expands live-vehicle TX authority.

Telltale remains `REFERENCE_PATTERN` / `RESEARCH_METHOD_REFERENCE` / `DATA_PIPELINE_REFERENCE`; GPL-3.0 source is not directly reused by default.

## Phase 6 decomposition

### Phase 6R — canonical reference/profile semantics reconciliation

Ready now; docs-only.

Purpose:
- canonicalize the Telltale reference and the three Phase-6-relevant semantic rules before source implementation:
  - observation failure != Unsupported;
  - multi-responder disagreement remains unresolved without explicit route/source authority;
  - decoder/scaling failures fail closed.

Minimum canonical destinations:
- `docs/REFERENCES.md`: add Telltale as a Phase 6/8/10 `REFERENCE_PATTERN` with no-source-reuse GPL-3.0 boundary.
- new local detailed note under `docs/references/vag/` or the nearest existing reference-note family, derived only from the sanitized evidence note.
- `docs/references/SYNTHESIS.md`: bounded Phase 6 synthesis of transferable findings.
- `docs/VEHICLE_PROFILE.md`: minimum profile/capability semantic clarification where it is the canonical owner.

Do not change source/tests in Phase 6R.

### Phase 6a — Brand/Profile ownership boundary

Do **not** admit implementation until Phase 6R is canonically closed and re-read.

Expected responsibility after reconciliation:
- Brand Layer owns brand-specific identification/routing/interpretation boundaries.
- Brand Profile Set owns known profile definitions.
- Resolver owns selection state, but no speculative detection algorithm.
- Active Vehicle Profile is the sole runtime profile selection.
- insufficient evidence remains `Unknown`, `Ambiguous` or `Manual selection required`.
- no actual Kamiq route/DID/scaling may be invented.
- no capability registry semantics should be pulled forward from Phase 6b.

Readiness question after 6R:
Can a minimal, representative profile ownership boundary be implemented without creating empty/speculative framework or fake vehicle mappings?
If not, keep Phase 6a at architecture/evidence state until the first concrete legally usable profile data / physical evidence exists.

### Phase 6b — capability + normalized signal registry

Keep separate from 6a.

This Stage must own:
- support/capability authority distinct from runtime availability;
- normalized signal identity/metadata registration;
- supported / unsupported / pending / unknown semantics;
- source/provenance and future explicit arbitration policy where evidence exists.

It must not infer Unsupported from one failed observation.

### Phase 6c — Deep Diagnostic on-demand

Keep separate from 6a/6b.

It may later bind Active Profile capability + Scheduler OnDemand + existing read-only OBD/UDS service boundaries, but:
- no arbitrary raw TX;
- no coding/adaptation/clear DTC/security access/output tests/routine control/flashing;
- no high-rate SRS/pretensioner polling.

## Why no direct Phase 6 source implementation yet

The Generic Core is now mature enough to consume a profile, but the repository still has no confirmed Kamiq route/DID/scaling data and no existing representative Brand/Profile source implementation.

Creating a broad BrandAdapter/plugin/inheritance/resolver framework before canonical support/ambiguity/decode semantics are reconciled would risk speculative abstraction.

Therefore the minimum next executable work is Phase 6R docs-only canonical reconciliation. Phase 6a implementation admission follows a fresh post-6R readiness check.

## STOP boundaries

Do not:
- claim Kamiq support / Hardware PASS / Vehicle PASS;
- copy upstream proprietary databases or GPL-3.0 implementation code;
- invent VAG routes, DIDs, scaling, SFD behavior or passive CAN IDs;
- create a generic plugin framework, inheritance engine, parser/schema/storage format without a representative consumer;
- merge Phase 6a, 6b and 6c into one implementation Stage.
