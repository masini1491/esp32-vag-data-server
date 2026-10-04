# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — P1 AGENTS Playbook adoption normalization

Status: Ready / priority before Phase 5C execution

Goal:
- 將 project `AGENTS.md` 的 AI Development Playbook adoption declaration 調整為 current Playbook preferred structured shape。
- 只做 governance/declaration normalization；不得改變既有 project authority、actor responsibility、write allowlist、technical architecture、validation scope、read-only policy 或 Phase 5C technical contract。

Current authority / provenance:
- User explicitly authorized P1 governance normalization in this conversation.
- Current project `main` at admission: `f4228e55fe3f0f994c7c4e8d487485afbd3ee15d`.
- Current Playbook reviewed: `masini1491/ai-development-playbook@3772e7e60213c99482dc201c064757075a8a48a9`.
- Preferred adoption shape: current Playbook `examples/minimal-project/AGENTS.md` + `tools/adoption_doctor.py`.
- Existing `AGENTS.md` remains the semantic source to preserve; this Stage is structural normalization, not policy redesign.

Required patch — `AGENTS.md` only:
1. Rename `## AI Development Playbook adoption` to `## AI Development Playbook baseline`.
2. Keep exactly one active declaration each:
   - `Playbook baseline: main`
   - `Project AI mode: ChatGPT+Codex`
3. Add the thin shared-reporting declaration in the baseline section:
   - `Shared reporting contract: declared Playbook baseline → REPORTING.md`
   - substantive user-facing engineering replies follow shared reporting even when other shared Playbook owners are not activated.
4. Normalize bootstrap wording to current conditional-activation semantics:
   - first verify target repository / branch / workspace identity and read current project governance / relevant current coordination state;
   - shared reporting is the narrow adoption-level exception and does not itself activate `CHAT_INIT.md`;
   - if project-native routing says no other shared governance is required, stay on the project-native route;
   - when other shared Playbook governance is required, resolve the single declared baseline to exact revision, then read `CHAT_INIT.md` and only minimum necessary owners/sections;
   - adoption/tool capability must not expand Task/write/execution/completion authority.
5. In `## Authority boundary`, preserve existing project-specific authority statement and move/add an explicit no-authority-expansion statement so current Adoption Doctor can deterministically detect it:
   - Playbook adoption / Project AI mode does not bypass Current Write Target, Task/Stage authorization, execution permission, credentials, validation, release/deployment or other project-specific authority.
6. Add `## Project-specific minimum contract` and move the existing declarations there without semantic expansion:
   - Canonical technical source(s): `docs/ARCHITECTURE.md`; `VALIDATION.md`; `src/`; `tests/`; this `AGENTS.md`
   - Current coordination surface: `TASKS.md`
   - Required validation: `git diff --check`; current `TASKS.md` / `VALIDATION.md` requirements; Bench／Hardware／Vehicle evidence remains Pending without physical evidence
   - Project-specific exceptions or restrictions: GitHub `main` source-of-truth with clean fast-forward-only sync; ChatGPT Coordination Write Allowlist `/TASKS.md`, `/BACKLOG.md` and sanitized `/evidence/inbox/*.md`; read-only diagnostic policy; physical evidence cannot be inferred from software or compile evidence
7. Preserve all remaining project-specific sections and semantics unless a tiny wording move is strictly required to avoid duplicate authority declaration.

Explicit non-goals:
- Do not modify `TASKS.md` from Codex.
- Do not modify `BACKLOG.md`, README, docs, source, tests, tooling, workflows or validation files.
- Do not change ChatGPT write allowlist.
- Do not introduce durable continuity checkpoint surfaces.
- Do not add PR requirements or change direct-to-main Git policy.
- Do not change Phase 5C contract or implementation scope.
- Do not add model names / launch settings to repository governance.
- Do not reformat unrelated sections.

Validation:
- `git diff --check` PASS.
- Diff is limited to `AGENTS.md`.
- Run current Playbook Adoption Doctor against this repository if the execution environment can access the declared Playbook tooling; target is zero FAIL and zero WARN for adoption structure.
- If current environment cannot run Adoption Doctor, perform bounded manual verification against current `tools/adoption_doctor.py` and report the unavailable runtime separately; do not fabricate PASS.
- Canonical read-back must confirm the normalized headings/fields and unchanged project-specific authority semantics.

Actor ownership:
- Codex: this `AGENTS.md` governance mutation + validation + commit/push.
- ChatGPT: admission, result reconciliation and Hot cleanup.

STOP:
- If normalization would require changing actor topology, write scope, technical source-of-truth, validation requirements, read-only policy, Git promotion requirements or any Phase 5C semantic, STOP and report instead of expanding P1.

---
## HOT — Phase 5C cooperative Scheduler core

Status: Ready

Goal:
- 建立 host-testable、fixed-capacity、externally-driven 的 Generic Core Scheduler timing/lifecycle kernel。
- 本 Stage 只擁有 opaque job ID 的 bounded registration、due/active lifecycle 與 injected monotonic `Clock` timing；不擁有 job execution callback、diagnostic semantics、VehicleData Store mutation、profile/capability mapping、thread/task、persistence 或 source priority。

Canonical evidence:
- `evidence/inbox/phase5-vehicledata-readiness-2026-09-22.md`
- Phase 5A normalized sample baseline: `68864bae3629474ad3a2fa90f5504fb6515653ce`
- Phase 5B bounded Store baseline: `d7b8552c8cb4599e334162e55d6aa4b387479a50`
- Phase 5B canonical closure: `eaf2edd065ef97e53982cfa9773409de529d236f`
- Architecture owners: `docs/ARCHITECTURE.md`, `docs/VEHICLE_PROFILE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@9ffab149023019e795f7df3e05d301c6516c2681`

Required implementation contract:
- Generic Core only; no Arduino/ESP32/FreeRTOS/CAN/TWAI/VAG-specific public types.
- Use injected `Clock`; no sleep/busy-wait/thread/task ownership.
- Fixed / compile-time bounded job capacity; reject duplicate opaque job IDs.
- Capacity full rejects a new job without mutating existing jobs.
- No global/canonical job registry or semantic job-name catalog.
- Policies admitted in this Stage only:
  - `Startup`
  - `Periodic`
  - `OnDemand`
- Do not implement `RealtimeTriggered` in this Stage.
- Unsupported capability is not a runnable scheduler policy; scheduler must not become capability-registry owner.
- Scheduler is callback-free and externally driven:
  - caller asks for/takes the next due job;
  - taking a due job marks exactly one job active;
  - while a job is active, no second job may be dispatched;
  - caller later reports completion for that same active job.
- Wrong-job completion is rejected without corrupting current active state.
- Due selection is deterministic registration/insertion order; no priority model.
- Scheduler does not execute callbacks and does not directly update `VehicleDataStore`.

Startup:
- becomes due once after registration / scheduler initialization;
- after take + completion it never becomes due again in this Stage;
- no automatic retry.

Periodic:
- period must be > 0;
- first deadline = registration-time `Clock::nowMs() + period`;
- not due before deadline; due exactly at deadline;
- cadence is anchored to scheduled deadlines, not completion time;
- after completion, advance from previous scheduled deadline by whole period increments until next deadline is strictly in the future relative to current `Clock::nowMs()`;
- late completion therefore skips missed periods and never creates catch-up burst;
- do not add jitter, priority, backoff or source-specific cadence.

OnDemand:
- dormant until explicitly requested;
- request makes it due;
- repeated request while pending or active does not create duplicate execution; return deterministic already-pending/busy-equivalent result;
- completion returns it to dormant;
- no automatic retry/requeue.

Time semantics:
- 64-bit monotonic milliseconds via injected `Clock`;
- cover values above `UINT32_MAX`;
- no wall-clock/timezone/RTC semantics;
- Scheduler deadlines do not rewrite VehicleData timestamps.

Deterministic host-test minimum:
- bounded registration PASS, duplicate-ID rejection, capacity-full rejection without mutation.
- Startup dispatches exactly once.
- Periodic not due before first deadline, due exactly at deadline.
- periodic completion preserves absolute cadence.
- late periodic completion skips missed periods and schedules first future deadline, with no catch-up burst.
- repeated periodic cycles do not accumulate completion-time drift.
- timestamps above `UINT32_MAX` remain correct.
- OnDemand dormant → request due → take active → completion dormant.
- duplicate OnDemand request while pending/active does not enqueue duplicate work.
- unknown-ID request returns deterministic not-found.
- simultaneous due jobs dispatch in stable registration order.
- active job blocks all additional dispatch until completion.
- wrong-job completion is rejected without state corruption.
- source/API inspection confirms no callback/executor abstraction, VehicleDataStore mutation, CAN/TWAI/VAG dependency, profile/capability registry, FreeRTOS/Arduino dependency, thread/mutex/task ownership or global mutable singleton.
- existing Phase 5A/5B VehicleData regressions and diagnostic host regressions remain PASS.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later diff/code review, canonical reconciliation and coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new Scheduler tests.
- existing VehicleData sample/Store and diagnostic host regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim only Phase 5C cooperative Scheduler core semantics; do not claim RealtimeTriggered, priority, multi-flight concurrency, callbacks/executor, automatic retry/backoff, scheduler-driven aging, profile polling integration, FreeRTOS binding, application integration or physical validation.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If implementation requires concrete OBD/UDS/VAG job definitions, profile/capability registry, polling-policy schema, callback/executor ownership, VehicleData Store mutation, automatic stale/TTL aging, retry/backoff, priorities, multi-resource concurrency, realtime event coalescing, persistence/config schema, FreeRTOS task/timer/workqueue binding or broader application runtime architecture, STOP and report instead of expanding scope.
- If a callback-free opaque-job scheduler cannot satisfy these semantics without representative Phase 6 producer/profile details, STOP and defer rather than inventing those details.

---
