# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 5B bounded VehicleData Store / Cache

Status: Ready

Goal:
- 在已完成的 Phase 5A `NormalizedVehicleSample` 契約之上，建立 host-testable、固定容量、latest-state normalized VehicleData Store / Cache。
- 本 Stage 只擁有「以 opaque normalized signal ID keyed 的 bounded current-state set」與 deterministic ordering/read/snapshot semantics；不擁有 Clock、registry、profile mapping、source priority、Scheduler、polling、persistence、serialization 或 eviction policy。

Canonical evidence:
- `evidence/inbox/phase5-vehicledata-readiness-2026-09-22.md`
- Phase 5A sample baseline: `68864bae3629474ad3a2fa90f5504fb6515653ce`
- Phase 5A canonical closure: `b16a5fd6708ff60204d5bb67e8d195935e93169f`
- Architecture owners: `docs/ARCHITECTURE.md`, `docs/VEHICLE_PROFILE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@7a975f6157207d9e0e01beb1131250988e8bc907`

Required implementation contract:
- Generic Core only; no Arduino/ESP32/CAN/TWAI/VAG-specific public types.
- Store complete `NormalizedVehicleSample` values keyed only by opaque normalized signal ID.
- Use fixed / compile-time bounded capacity; do not add unbounded growth or dynamic-allocation ownership requirement.
- Do not store duplicate logical signal IDs. Upsert of an existing ID updates that entry without increasing size.
- The Store must not own or call `Clock`; use only the timestamp already carried by each incoming sample.
- Same-ID ordering:
  - incoming timestamp < stored timestamp → ignore as older; no mutation;
  - incoming timestamp == stored timestamp → accept; later call wins deterministically;
  - incoming timestamp > stored timestamp → accept and replace.
- Equal-timestamp last-call-wins is only a deterministic tie rule. Do not infer source priority/arbitration.
- Accepted update replaces the whole sample atomically at Store contract level: value/unit/source/quality/availability/timestamp move together.
- Availability/status samples obey the same timestamp ordering.
- Accepted non-`Available` sample replaces any prior value; Store must not hide/retain the old value behind `Unavailable`, `Pending`, `Unsupported` or `Unknown`.
- Retained last-known value exists only when the incoming sample itself is `Available + Stale`; Store must not synthesize stale state or automatic aging.
- Full-capacity behavior:
  - existing-ID update remains allowed according to ordering;
  - new-ID insertion when full returns deterministic capacity/full result and leaves all entries unchanged;
  - no automatic eviction of oldest/newest/source/quality entry.
- Lookup/read returns or copies current sample without exposing mutable internal storage.
- Snapshot is bounded and copy-based; preserve deterministic insertion-slot order. Existing-ID updates do not reorder the entry.
- If caller-provided snapshot capacity is insufficient, report insufficient capacity and do not present a partial result as a complete snapshot. Exact API/result naming is implementation detail.
- No erase, TTL, automatic expiration/aging, persistence or serialization required in this Stage.
- No mutex/thread/task ownership; Phase 5B is single-owner / externally serialized by contract. Concurrency ownership remains future integration work.
- Avoid global mutable state and singleton ownership.

Deterministic host-test minimum:
- insert one available sample; lookup returns equal copy and size becomes one.
- newer timestamp for same ID replaces the entire sample without increasing size.
- equal timestamp for same ID is accepted and later call wins.
- older timestamp for same ID is ignored and stored sample remains bit/semantically unchanged.
- timestamp ordering works above `UINT32_MAX` without truncation.
- accepted `Available → Unavailable`, `Pending`, `Unsupported`, `Unknown` transition exposes no current value.
- stale last-known value is retained only when incoming sample is explicitly `Available + Stale`.
- full Store rejects insertion of a new ID without eviction or mutation of existing entries.
- full Store still permits an existing-ID update according to ordering.
- snapshot with sufficient caller capacity returns all entries in stable deterministic order.
- insufficient snapshot capacity is reported without claiming a complete partial snapshot.
- mutation of a lookup/snapshot copy cannot mutate internal Store state.
- source/API inspection confirms no Clock ownership, CAN/TWAI/VAG dependency, registry/profile schema, Scheduler, eviction, persistence, serialization or global mutable singleton.
- existing Phase 5A VehicleData sample tests and all diagnostic host regressions remain PASS.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later diff/code review, canonical reconciliation and coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new Store/Cache tests.
- existing VehicleData sample and diagnostic host regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim only Phase 5B bounded VehicleData Store/Cache semantics; do not claim Scheduler, capability/signal registry, VAG mappings, automatic aging, source arbitration, persistence, application integration or physical validation.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If implementation requires defining source priority/arbitration, eviction, scheduler/job model, automatic stale/expiry timing, profile/capability registry, persistence/storage backend, client serialization, locks/concurrency ownership, VAG semantics or CAN frame timestamp changes, STOP and report instead of expanding scope.
- If fixed-capacity latest-state semantics cannot be implemented without changing Phase 5A sample invariants, STOP before broadening that contract and report the minimum boundary issue.

---
