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


## Phase 5B readiness finalization

Phase 5A normalized sample/value semantics completed at `68864bae3629474ad3a2fa90f5504fb6515653ce` and were canonically closed at `b16a5fd6708ff60204d5bb67e8d195935e93169f`.

No external discovery is required for Phase 5B. The remaining problem is a project-specific bounded latest-state cache over the already-frozen `NormalizedVehicleSample` contract, and current project architecture plus the shared Playbook provide sufficient ownership/complexity constraints.

### Phase 5B owner

Owner sentence:
> Phase 5B owns a bounded in-memory latest-state set keyed by opaque normalized signal ID; it does not own timestamp generation, source priority/arbitration, profile/capability registry, polling/scheduling, persistence, serialization or eviction policy.

### Phase 5B exact Store / Cache semantics

The first Store / Cache slice should be deliberately small and deterministic:

- Generic Core only; no Arduino/ESP32/CAN/TWAI/VAG types.
- Store complete `NormalizedVehicleSample` values keyed only by their opaque normalized signal ID.
- Use fixed / compile-time bounded capacity with no dynamic allocation requirement or unbounded growth.
- Duplicate logical IDs are not stored. An upsert for an existing ID updates that entry in place.
- The store does not call `Clock`; it consumes the timestamp already carried by each normalized sample.
- Same-ID ordering:
  - incoming timestamp < stored timestamp → ignore as older, do not mutate current state;
  - incoming timestamp == stored timestamp → accept, with later call winning deterministically;
  - incoming timestamp > stored timestamp → accept and replace current state.
- Equal-timestamp last-call-wins is only a deterministic tie rule. It does not create source priority or multi-producer arbitration semantics.
- Whole-sample replacement is atomic at the Store contract level: value, unit, source, quality, availability and timestamp move together.
- Availability/status transitions use the same ordering rules as value updates.
- An accepted non-`Available` sample replaces the prior sample and therefore exposes no current value. The Store must not secretly retain a hidden old value behind `Unavailable` / `Pending` / `Unsupported` / `Unknown`.
- If a producer intentionally wants to retain a last-known value, it must submit an `Available + Stale` sample. The Store does not synthesize stale state or aging.
- Full-capacity behavior:
  - update of an already-present ID still succeeds according to timestamp ordering;
  - insertion of a new ID when full returns a deterministic full/capacity result and leaves all stored entries unchanged;
  - no automatic oldest/newest/source/quality eviction in Phase 5B.
- Read API returns/copies the current sample for an ID without exposing mutable internal storage.
- Snapshot API is bounded and copy-based for future clients. It should preserve deterministic insertion-slot order; updates to an existing ID do not reorder that entry.
- A snapshot request whose caller-provided capacity is insufficient must report insufficient capacity without presenting a partial snapshot as complete. Exact result symbols/API spelling remain implementation detail.
- No erase/TTL/automatic expiry behavior is required in this Stage.
- No internal mutex/thread/task ownership is added. Phase 5B is single-owner / externally serialized by contract; concurrency ownership belongs to later integration once a real scheduler/runtime owner exists.

### Why no eviction / aging / arbitration yet

Shared Playbook architecture rules require semantic identity, ordering and bounded buffering to have explicit ownership, and specifically warn against inventing scheduler/cache/arbitration complexity from hypothetical future collisions.

Current repository evidence has no profile registry, no producer-priority contract and no Scheduler implementation. Therefore:
- eviction would invent a priority policy with no authority;
- automatic aging would invent a timing/polling lifecycle owned by future Scheduler/profile semantics;
- source arbitration would invent profile-level precedence;
- locks/concurrency would invent a runtime ownership model before a representative consumer exists.

Fail-closed fixed capacity plus timestamp convergence is the minimum sufficient Store contract.

### Phase 5B deterministic test intent

Minimum host scenarios:
- insert one available sample; lookup returns an equal copy and size becomes one;
- second sample with same ID and newer timestamp replaces the whole prior sample without increasing size;
- same ID with equal timestamp is accepted and later call wins deterministically;
- same ID with older timestamp is ignored and stored state is unchanged;
- timestamp ordering works above `UINT32_MAX` without truncation;
- `Available → Unavailable/Pending/Unsupported/Unknown` accepted transition removes current value according to sample semantics;
- stale last-known value is retained only when incoming sample itself is `Available + Stale`;
- full store rejects a new ID without evicting or mutating existing entries;
- full store still allows valid update of an existing ID;
- snapshot copies all entries in deterministic stable order when caller capacity is sufficient;
- insufficient snapshot capacity is reported without claiming a complete partial snapshot;
- read/snapshot copies cannot mutate internal Store state;
- source/API inspection confirms no Clock ownership, CAN/TWAI/VAG dependency, registry/profile schema, Scheduler, eviction, persistence, serialization or global mutable singleton is introduced;
- all existing VehicleData sample and diagnostic host regressions remain PASS.

### Phase 5C remains deferred

Scheduler remains a separate later Stage. Phase 5B does not freeze:
- startup / periodic / on-demand / realtime-triggered job model;
- polling cadence;
- callback/executor shape;
- diagnostic service arbitration;
- automatic stale/expiry timing;
- profile polling policy;
- concurrency/thread/task ownership.

Those decisions require the now-concrete Phase 5A sample and Phase 5B store semantics plus a representative producer/consumer flow.


## Phase 5C Scheduler readiness finalization

Phase 5A and Phase 5B now provide concrete normalized sample and bounded Store semantics. Phase 5C can therefore freeze a minimal scheduler lifecycle without inventing profile schema, Store mutation callbacks or diagnostic protocol knowledge.

Because scheduler is a mature application/runtime capability, a bounded public pattern check was performed before architecture freeze.

Public pattern sources:
- FreeRTOS `vTaskDelayUntil()` reference / periodic-task guidance: https://www.freertos.org/media/2018/FreeRTOS_Reference_Manual_V10.0.0.pdf
- FreeRTOS current `vTaskDelay()` documentation explaining why fixed-frequency periodic work should use absolute rather than call-relative timing: https://freertos.org/a00127.html
- Zephyr Workqueue / Delayable Work documentation: https://docs.zephyrproject.org/latest/kernel/services/threads/workqueue.html

Relevant transferable observations:
- fixed-frequency periodic scheduling should advance from an absolute schedule/deadline rather than “completion time + interval” if drift is to be avoided;
- scheduled/pending/running work lifecycle is distinct from the code that actually executes the work;
- repeated scheduling of the same logical work does not require unbounded duplicate queue entries.

These are architecture patterns only. No FreeRTOS or Zephyr source/API is copied into Generic Core.

### Phase 5C owner

Owner sentence:
> Phase 5C owns bounded cooperative job timing/due/active lifecycle over opaque job IDs using injected monotonic `Clock`; it does not own job execution, diagnostic protocol/service semantics, VehicleData Store mutation, profile/capability mapping, threading, persistence or source priority.

### Why Phase 5C can proceed before Phase 6

The scheduler can remain independent of future profile semantics if its contract is limited to opaque job identity + generic schedule policy and exposes due jobs to an external executor.

This avoids freezing:
- OBD/UDS callback signatures;
- Brand/Profile signal mappings;
- capability registry schema;
- Store update callbacks;
- task/thread/FreeRTOS ownership.

Phase 6 can later instantiate scheduler jobs from an Active Profile without modifying the scheduler’s generic timing lifecycle.

### Phase 5C minimum first slice

Use a fixed / compile-time bounded cooperative scheduler.

Generic policies admitted in this first slice:
- `Startup`
- `Periodic`
- `OnDemand`

Do **not** implement `RealtimeTriggered` yet. A real realtime producer/event identity and loss/coalescing contract does not exist until later passive-CAN/runtime work. Inventing its queue/coalescing semantics now would be speculative.

Likewise, `Unsupported` remains a capability/profile state, not a runnable scheduler policy. Unsupported work is simply not registered as a runnable job. The scheduler may return deterministic not-found/not-schedulable results for unknown IDs, but it must not become the capability registry owner.

### Job identity / registration

- Job ID is opaque caller-owned identity; exact underlying type/API spelling is implementation detail.
- No canonical/global job registry or semantic job names are created by Phase 5C.
- Fixed bounded capacity; duplicate job ID registration is rejected.
- Capacity full rejects a new job without mutating existing jobs.
- No dynamic registration schema, persistence, config parser or profile schema.
- No unregister/update-period API is required in the first slice.

### Cooperative dispatch lifecycle

Scheduler is externally driven:
- it owns no thread/task and performs no sleep/busy-wait;
- it reads injected `Clock::nowMs()`;
- caller asks for/takes the next due job;
- taking a due job marks exactly one job active;
- while one job is active, no second job is dispatched;
- caller later reports completion of that same job;
- completion transitions scheduler state and computes any next periodic deadline.

This global single-flight rule is intentionally conservative for the first diagnostic-oriented runtime. It prevents overlapping access before resource-group/service arbitration exists. Later evidence may justify multi-resource concurrency as a separate architecture change.

Scheduler does not execute callbacks and does not directly touch `VehicleDataStore`.

### Startup policy

- Startup job becomes due once after registration/scheduler initialization.
- Once taken and completed, it never becomes due again in this slice.
- No automatic retry policy is created.

### Periodic policy

- Period must be > 0.
- Initial due time is registration-time `Clock::nowMs() + period`; immediate startup behavior should use a separate Startup job rather than overloading Periodic semantics.
- Periodic cadence is anchored to scheduled deadlines, not actual completion time.
- On completion, advance from the previous scheduled deadline by whole period increments until the next deadline is strictly in the future relative to current `Clock::nowMs()`.
- Therefore late execution skips missed periods rather than creating catch-up bursts and does not accumulate completion-time drift.
- No jitter, priority, rate adaptation, backoff or source-specific cadence logic is owned here.

### On-demand policy

- OnDemand job is dormant until explicitly requested.
- A request makes it due.
- Repeated request while already pending or active must not create duplicate queued instances; return a deterministic already-pending/busy-equivalent result.
- Completion returns it to dormant state.
- No automatic retry/requeue on failure is created; execution result semantics belong to the external owner.

### Due selection

- If multiple jobs are simultaneously due, choose deterministic registration/insertion order.
- No priority model is introduced.
- Existing active job always blocks further dispatch.

### Time semantics

- Use injected 64-bit monotonic `Clock`.
- Scheduler owns schedule deadlines only; it does not rewrite VehicleData timestamps.
- Tests must cover values above `UINT32_MAX`.
- No wall clock / timezone / RTC semantics.

### Phase 5C deterministic test intent

Minimum host scenarios:
- bounded registration, duplicate-ID rejection and full-capacity rejection without mutation;
- Startup job dispatches exactly once;
- Periodic job is not due before first absolute deadline and is due exactly at deadline;
- periodic completion before next interval preserves absolute cadence;
- late periodic completion skips all missed periods and schedules the first future deadline without catch-up burst;
- repeated cycles do not accumulate “completion time + interval” drift;
- time above `UINT32_MAX` is preserved;
- OnDemand is dormant before request, due after request, dormant after completion;
- duplicate OnDemand request while pending or active does not enqueue duplicate execution;
- unknown-ID request is deterministic not-found;
- simultaneous due jobs dispatch in stable registration order;
- once one job is taken active, no second job dispatches until matching completion;
- wrong-job completion is rejected without corrupting scheduler state;
- scheduler has no callback execution, VehicleDataStore mutation, CAN/TWAI/VAG dependency, profile/capability registry, FreeRTOS/Arduino dependency, thread/mutex/task ownership or global mutable singleton;
- existing VehicleData/Store and diagnostic host regressions remain PASS.

### Phase 5C STOP / deferred areas

Do not add in this slice:
- RealtimeTriggered event semantics;
- job priorities or source priority;
- multi-resource/multi-flight concurrency;
- execution callback/executor abstraction;
- automatic retry/backoff;
- scheduler-driven VehicleData stale/TTL aging;
- profile/capability registry or polling policy schema;
- persistence/config serialization;
- OBD/UDS/VAG-specific job definitions;
- FreeRTOS task/timer/workqueue bindings.

If implementation cannot preserve a callback-free opaque-job boundary without needing concrete producer/profile semantics, STOP and defer Phase 5C until Phase 6 rather than inventing those semantics.
