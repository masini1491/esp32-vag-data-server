# Phase 5 VehicleData / Scheduler readiness — 2026-09-22

Status: COMPLETE FOR PHASE 5A ADMISSION
Purpose: Phase 5 architecture/readiness evidence only; no execution authority.

## Current authority / baseline

- Target repository baseline reviewed: `005d2073107a44e1ee6bdfe5d8be70c0109fb7ba`
- Shared Playbook reviewed: `masini1491/ai-development-playbook@73caca6d10728453c584e5480018b93d99ad1760`
- Current Hot state before this review: `TASKS.md = EMPTY`
- Phase 2A transport, Phase 3 OBD, and Phase 4 minimum UDS `0x22` host cores are complete.

## Current repository facts

- No `VehicleData`, VehicleData cache/store, capability registry, normalized signal registry or Scheduler implementation exists yet.
- `src/core` currently contains only generic CAN types and the injected 64-bit `Clock` abstraction.
- Profile / VAG / passive-CAN implementation directories remain scaffold-only.
- Architecture already declares:
  - VehicleData is a normalized application boundary;
  - signal semantics may include ID, Value, Unit, Timestamp, Source, Quality and Availability;
  - unsupported / unavailable / pending / unknown must not be represented by fake values;
  - BLE/Web/Logger/HUD should consume VehicleData + capability semantics rather than raw CAN/DID knowledge;
  - profile-owned mapping/polling policy and Phase 6b capability/signal registry remain later responsibilities.

## Ownership decomposition

Phase 5 should not be implemented as one large Stage.

### Phase 5A — normalized VehicleData sample/value semantics

Owner sentence:
> Phase 5A owns the brand-independent representation and invariants of one normalized vehicle signal observation/status; it does not own registry, cache replacement, scheduling or profile mapping.

Required boundary:
- Generic Core only; no Arduino/ESP32/CAN/TWAI/VAG types.
- No actual signal registry and no hard-coded `vehicle.speed`, `vehicle.rpm` etc. as authoritative global IDs.
- Signal ID is an opaque normalized identifier supplied by a future profile/registry owner.
- Identifier/value/text storage must be copy-safe / lifetime-safe; do not retain caller-owned transient pointers.
- Minimum value domain supports:
  - numeric scalar;
  - boolean;
  - bounded text sufficient for a 17-character VIN.
- Availability states at minimum:
  - Available
  - Unsupported
  - Unavailable
  - Pending
  - Unknown
- Quality is independent from Availability and at minimum distinguishes valid/current, stale and invalid/unknown quality.
- Source is generic and may distinguish OBD, UDS, Passive CAN, Derived and Unknown without knowing brand-specific routes.
- Timestamp is monotonic milliseconds represented as `uint64_t` semantics.
- A non-Available state must not present a value as if it were current. API/design must make absence explicit rather than relying on zero/false/empty sentinel values.
- A stale but still explicitly retained last-known sample may keep a value only if it is marked as stale quality; it must not become indistinguishable from a current valid value.
- Unit metadata is normalized/brand-independent but Phase 5A must not create a global unit registry or profile schema.

### Phase 5B — bounded VehicleData Store / Cache (not admitted yet)

After Phase 5A completion, a separate readiness/admission should define:
- bounded ownership of current samples keyed by normalized ID;
- insert/update/read behavior;
- deterministic capacity/full behavior;
- timestamp/order semantics for same signal;
- explicit invalidation / availability transitions;
- snapshot/read API for future clients;
- no scheduler and no profile-specific mapping.

The store should not be designed until Phase 5A value/status invariants are concrete and tested.

### Phase 5C — Scheduler core (not admitted yet)

Scheduler should remain separate because it owns time/task lifecycle rather than VehicleData representation.

Potential later responsibilities:
- distinguish startup / periodic / on-demand / realtime-triggered / unsupported policies;
- use injected `Clock`;
- bounded task ownership and no overlapping execution for a single diagnostic service owner;
- update VehicleData Store only through explicit producer/result boundary.

Do not freeze callback/job/profile schema in Phase 5A. Phase 5C admission should happen only after representative Phase 5A/5B data flow exists, consistent with the Playbook rule against speculative scheduler/cache architecture.

## 64-bit timestamp Cold trigger review

The existing Cold item concerns `CanFrame.timestamp`, which currently derives from Arduino `millis()` behavior.

Phase 5A does **not** require promotion of that item:
- normalized VehicleData timestamp semantics can use the already-existing injected `Clock::nowMs()` 64-bit contract;
- Phase 5A/5B need not inherit `CanFrame.timestamp`;
- no passive CAN source-time transfer is part of current Phase 5A scope.

Therefore keep the frame timestamp item Cold until a concrete path (most likely passive CAN / source-frame freshness) requires preserving frame-origin time into normalized VehicleData.

## Deterministic Phase 5A test intent

Minimum representative tests:
- available numeric sample preserves opaque ID, normalized unit metadata, source, quality and a timestamp above 32-bit range without truncation;
- available boolean sample preserves type/value;
- available text sample safely preserves a 17-character VIN-like value after caller buffer lifetime/change cannot affect stored value;
- Unsupported / Unavailable / Pending / Unknown states expose no current value;
- stale last-known sample remains distinguishable from current/valid sample;
- copy/move or equivalent value semantics do not leave dangling signal-ID/text storage;
- source/API inspection confirms no CAN/TWAI/VAG types, signal registry, profile schema, scheduler or global mutable store is introduced.

## Non-goals

- No VehicleData Store/Cache in Phase 5A.
- No Scheduler in Phase 5A.
- No capability registry / normalized signal registry.
- No VAG signal IDs, DID/CAN mapping or scaling.
- No BLE/Web/Logger serialization.
- No frame timestamp refactor.
- No application diagnostic TX changes.
- Bench / Hardware / Vehicle remain Pending.
