# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 6b capability + normalized signal registry

Status: Ready

Goal:
- 建立最小、host-testable 的 capability-support / normalized signal metadata registry。
- 本 Stage 只擁有 capability support authority 與 normalized signal descriptor/lookup；不得擁有 runtime availability、route/DID/scaling、polling、Scheduler 或 diagnostic execution。

Canonical evidence:
- `evidence/inbox/phase6-readiness-2026-10-04.md`
- `evidence/inbox/phase6b-capability-registry-readiness-2026-10-04.md`
- `docs/ARCHITECTURE.md`
- `docs/VEHICLE_PROFILE.md`
- `src/core/vehicle_data.h`
- Phase 6a profile identity baseline: `e237aa17700d64b07917fda8e30d72076211bfe8`

Required implementation contract:
- Reuse existing `NormalizedVehicleSample::SignalId` semantics; do not create a second normalized signal-ID system.
- Introduce a capability-support state separate from `VehicleAvailability`, with exactly:
  - `Supported`
  - `Unsupported`
  - `Pending`
  - `Unknown`
- `VehicleAvailability` remains runtime sample availability. Do not alias, typedef, reuse or conflate it with capability support.
- `Unavailable` is not a capability-support state.
- Unknown/unregistered signal lookup must return deterministic not-found/unknown-to-registry; it must not fabricate `Unsupported`.
- A single runtime failure / timeout / `NO DATA` / malformed response must have no API path that automatically changes capability support to `Unsupported`.
- Add a minimal fixed/compile-time normalized signal descriptor carrying only:
  - normalized signal identity;
  - expected `VehicleValueType`;
  - normalized unit;
  - capability support state.
- Keep the first slice fixed/bounded; no dynamic registry allocation, persistence or parser/schema engine.
- Add a minimal Kamiq_NW4 capability set with these normalized candidate IDs:
  - `vehicle.speed`
  - `vehicle.rpm`
  - `vehicle.coolantTemp`
  - `vehicle.voltage`
- Every Kamiq_NW4 candidate above must remain `Pending` / not vehicle-confirmed in this Stage.
- Profile selection does not promote capability state.
- Registry lookup does not create/update `NormalizedVehicleSample` and does not mutate `VehicleDataStore`.

Evidence boundary:
- Canonical architecture names these normalized signal identities and the Phase 10 minimum vehicle-validation dataset requires them.
- Upstream Kamiq coverage only establishes candidate/signal-description relevance, not support confirmation.
- No `Supported` Kamiq capability may be claimed without stronger authority.
- `VEHICLE_CONFIRMED = none`; Bench / Hardware / Vehicle remain Pending.

Deterministic host-test minimum:
- capability-support enum/type is distinct from `VehicleAvailability`;
- registry is fixed/bounded and contains only the admitted descriptors;
- lookup succeeds for every admitted normalized signal;
- unknown signal returns deterministic not-found/unknown-to-registry and never `Unsupported`;
- all initial Kamiq_NW4 descriptors are `Pending`;
- descriptor ID / expected value type / unit / support state are stable and bounded;
- selecting Kamiq_NW4 does not alter Pending states;
- registry lookup does not create or mutate VehicleData samples/store;
- no support mutation API accepts timeout/read-failure/`NO DATA` as an input;
- existing diagnostic, VehicleData, Scheduler and Phase 6a regressions remain PASS;
- source/API inspection confirms no ECU route, DID, CAN ID/bit, scaling formula, source priority/arbitration, polling policy, Scheduler binding, diagnostic execution, profile parser/schema/storage engine, Arduino/ESP32/FreeRTOS dependency or global mutable singleton.

Explicit non-goals:
- no runtime availability tracking;
- no automatic support learning;
- no support mutation from diagnostic results;
- no ECU/source arbitration implementation;
- no route/DID/scaling/passive-CAN mappings;
- no polling policy;
- no Scheduler integration;
- no profile persistence/parser/schema;
- no Deep Diagnostic implementation;
- no Hardware/Vehicle PASS claim.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, diff/code review, canonical reconciliation and Hot cleanup.

Validation / completion:
- project host compile/tests PASS with new Phase 6b tests and all existing regressions;
- `git diff --check` PASS;
- if ESP32-facing source participation/platform boundary does not change, no new ESP32 compile is required solely for formality; evidence scope must remain explicit;
- canonical docs may claim only the capability-support / normalized signal registry slice implemented here;
- Bench / Hardware / Vehicle remain Pending.

STOP:
- if implementation requires concrete VAG/Kamiq route/DID/scaling/passive CAN data, runtime support learning, polling/Scheduler integration, diagnostic execution, source arbitration, persistence/parser/schema or broader framework architecture, STOP and report instead of expanding scope.

---
