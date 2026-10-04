# Phase 6a profile identity / Active Profile readiness — 2026-10-04

Status: READY_FOR_BOUNDED_IMPLEMENTATION
Authority role: sanitized evidence/readiness staging only; no execution authority

## Project baseline

Reviewed after Phase 6R closure:
- project main: `18c5b7853a4e281eb448dd4d26418f2496c1c771`
- current Phase 6 canonical semantics include support-vs-availability separation, multi-responder ambiguity, and decode/scaling fail-closed.
- `src/profiles/` and `src/vag/` currently contain no implementation files.
- existing Generic Core provides VehicleData, Store and Scheduler but no profile identity/selection implementation.

## Narrow Revisit trigger

Local evidence was insufficient to answer whether Phase 6a had a concrete representative profile identity. A bounded external revisit was therefore performed against already relevant/public sources.

### Official Škoda 2024 evidence

Škoda Taiwan official 2024 Kamiq surfaces confirm:
- a 2024 facelift Kamiq product is the current project vehicle generation;
- the official owner's-manual detail route for edition `09-2024` identifies the model as `Kamiq_NW4`.

This is sufficient to preserve a concrete project-local identity token for the 2024 facelift target.

It does **not** establish:
- VIN/WMI/chassis-code matching rules;
- ECU routes;
- DIDs;
- scaling;
- SFD/gateway behavior;
- passive CAN IDs;
- Hardware/Vehicle PASS.

Sources checked:
- https://www.skoda.com.tw/apps/manuals/Detail?edition=09-2024&model=Kamiq_NW4
- https://www.skoda.com.tw/news/news-detail/kamiq2024
- official MY2024 Kamiq specification sheet linked from Škoda Taiwan.

### opendbc current upstream evidence

Current `commaai/opendbc` source at observed revision `35f7e0813462607ef1d703e52313e7571e31405d` contains:
- `SKODA_KAMIQ_MK1 = VolkswagenMQBPlatformConfig(...)`;
- documentation labels for Škoda Kamiq 2021–23 and Scala 2020–23;
- chassis code `NW` and Škoda WMI constraint for that upstream platform entry;
- concrete ECU firmware fingerprint sets for engine/transmission/SRS/EPS/fwdRadar;
- existing test routes for `SKODA_KAMIQ_MK1`.

Transferable conclusion:
- a VAG/Kamiq profile identity concept has a real upstream consumer and is not purely speculative;
- current upstream identity/fingerprint data is useful research/reference evidence.

Critical boundary:
- upstream `SKODA_KAMIQ_MK1` evidence is explicitly 2021–23 in the current source;
- therefore its chassis/fingerprint set must **not** be treated as confirmed 2024-facelift resolver authority without separate evidence;
- 2024 selection must remain manual/pending/unknown/ambiguous as appropriate until exact matching evidence exists.

## Phase 6a first-slice design

The minimum non-speculative implementation is limited to:

1. A brand-independent Active Vehicle Profile selection state that can represent:
   - `Unknown`
   - `Ambiguous`
   - `ManualSelectionRequired`
   - `Selected`

2. An opaque profile identity owned outside Generic protocol/data semantics.

3. A minimal compile-time VAG Profile Set with a concrete project target identity for:
   - Škoda Kamiq 2024 facelift;
   - official identity token/reference `Kamiq_NW4`;
   - evidence/validation state remaining `Pending` / not vehicle-confirmed.

4. Manual/explicit selection only for this first slice.
   - No VIN parser.
   - No WMI/chassis matcher.
   - No firmware fingerprint matcher.
   - No automatic resolver algorithm.

5. Selection state and profile validation/evidence state remain distinct.
   - selecting a profile does not imply Hardware/Vehicle PASS;
   - selecting the 2024 profile does not create capability support;
   - capability/signal ownership stays in Phase 6b.

## Required invariants

- Default state has no selected profile.
- `Unknown`, `Ambiguous`, and `ManualSelectionRequired` expose no active profile identity.
- `Selected` exposes exactly one profile identity from the admitted Profile Set.
- Transitioning from Selected to any unresolved state clears the active profile.
- Unknown/out-of-set profile identity cannot become Active.
- Profile selection does not create route/DID/scaling/capability data.
- The 2024 Kamiq descriptor remains explicitly Pending / not vehicle-confirmed.
- No opendbc 2021–23 fingerprint is copied into runtime source in this Stage.
- No third-party source code is copied.

## Explicit non-goals

Do not implement:
- BrandAdapter virtual/plugin interface;
- resolver algorithm;
- VIN/WMI/chassis parsing;
- firmware fingerprint matching;
- inheritance engine;
- profile file/parser/schema/storage format;
- capability/signal registry;
- route/DID/scaling/passive-CAN mappings;
- Scheduler binding;
- diagnostic execution;
- Deep Diagnostic;
- Hardware/Vehicle claims.

## Why this slice is now admissible

Before the bounded revisit, a profile layer would have lacked a concrete representative identity and risked becoming an empty framework.

The official `Kamiq_NW4` 2024 identity plus current upstream Kamiq MK1 profile evidence now provides a real VAG/Kamiq consumer while still preserving the evidence boundary that 2021–23 fingerprints are not 2024 confirmation.

This supports a small identity/selection ownership slice without inventing vehicle mappings.
