# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 5A normalized VehicleData sample semantics

Status: Reconciliation correction required

Goal:
- 建立 brand-independent、host-testable 的 normalized VehicleData sample/value semantics，作為未來 VehicleData Store/Cache 與 Scheduler 的資料契約。
- 本 Stage 只擁有「單一 normalized signal observation/status」的 representation 與 invariants；不建立 registry、store/cache、scheduler、profile mapping、VAG signal semantics 或 client serialization。

Canonical evidence:
- `evidence/inbox/phase5-vehicledata-readiness-2026-09-22.md`
- Architecture owners: `docs/ARCHITECTURE.md`, `docs/VEHICLE_PROFILE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@73caca6d10728453c584e5480018b93d99ad1760`

Required implementation contract:
- Generic Core only; no Arduino/ESP32/CAN/TWAI/VAG-specific public types.
- Define one normalized signal sample/value model with explicit:
  - opaque normalized signal identifier;
  - value type;
  - unit metadata;
  - source;
  - quality;
  - availability;
  - monotonic millisecond timestamp using 64-bit semantics.
- Signal identifier is opaque data supplied by a future profile/registry owner; do not establish a canonical/global signal registry in this Stage.
- Minimum value domain:
  - numeric scalar;
  - boolean;
  - bounded text with capacity sufficient for a 17-character VIN.
- Identifier and text storage must be lifetime-safe/copy-safe; do not retain caller-owned transient pointers or references.
- Availability must distinguish at minimum:
  - Available
  - Unsupported
  - Unavailable
  - Pending
  - Unknown
- Quality must be independent from Availability and distinguish at minimum:
  - Valid/current
  - Stale
  - Invalid/unknown
- Source must remain generic and distinguish at minimum:
  - OBD
  - UDS
  - Passive CAN
  - Derived
  - Unknown
- A non-Available state must not expose a value as if it were current. Zero/false/empty string are not absence sentinels.
- A stale last-known sample may retain its value only when quality explicitly marks it stale; callers must be able to distinguish it from current valid data.
- Unit metadata remains normalized/brand-independent; do not create a unit registry/schema in this Stage.
- Timestamp must preserve values above 32-bit range without truncation.
- Do not use or modify `CanFrame.timestamp`; Phase 5A normalized timestamp semantics are independent and may use existing 64-bit `Clock::nowMs()` semantics.
- Avoid global mutable state and singleton ownership.

Deterministic host-test minimum:
- available numeric sample preserves identifier, value, unit, source, quality and a timestamp greater than `UINT32_MAX`.
- available boolean sample preserves type/value.
- available bounded text sample preserves a 17-character VIN-like value.
- caller buffer/string mutation after sample construction cannot mutate stored identifier/text data or create dangling lifetime.
- Unsupported / Unavailable / Pending / Unknown samples expose no current value.
- stale last-known value remains available only with explicit stale quality and is distinguishable from current valid data.
- copy/move or equivalent value semantics preserve lifetime-safe identifier/text/value state.
- compile/source inspection confirms no CAN/TWAI/VAG type dependency, signal registry, profile schema, store/cache, scheduler or global mutable state is introduced.
- existing ISO-TP / OBD / UDS / ReadOnlyGuard host regressions remain PASS.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later code/diff review, canonical reconciliation and coordination cleanup.

Reconciliation finding:
- Implementation `68864bae3629474ad3a2fa90f5504fb6515653ce` and the Phase 5A host-test/canonical-state updates are accepted for the Stage contract.
- One current validation-owner inconsistency remains in `VALIDATION.md`: the `ESP32 Compile` row still says the preserved Phase 1 compile was not re-run only through Phase 4B, while the same file's current baseline / Host Test / Required revalidation sections already include Phase 5A.
- Residual correction is documentation-only: update that ESP32 Compile row so its scope statement also includes Phase 5A and remains explicit that this is preserved Phase 1 evidence, not new Phase 5A ESP32 compile evidence.
- Do not change source/tests or broaden Phase 5A. Do not alter Bench / Hardware / Vehicle Pending.

Validation / completion:
- project host compile/tests PASS with new VehicleData sample tests.
- existing diagnostic host regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim only Phase 5A normalized VehicleData sample/value semantics; do not claim VehicleData Store/Cache, Scheduler, capability registry, signal registry, VAG mappings, application integration or physical validation.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If safe implementation requires choosing a global signal registry/schema, profile inheritance/storage format, cache replacement policy, scheduler/job model, client serialization, VAG-specific signal semantics, CAN frame timestamp refactor, or broader application architecture, STOP and report instead of expanding scope.
- If the minimum representation cannot make availability/value absence explicit without introducing a broader ownership model, STOP before inventing that broader model and report the boundary issue.

---
