# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 4A UDS ReadOnlyGuard safety extension

Status: Ready

Goal:
- 在任何 Generic UDS service semantics implementation 之前，先把既有 `ReadOnlyGuard` 擴充成不可繞過的 semantic UDS single-DID read boundary。
- 此 Stage 只開放 `ReadDataByIdentifier (0x22)` 的單一 DID request；不實作 UDS response parser、session lifecycle、DTC、TesterPresent、VAG routing、VehicleData 或 live application integration。

Canonical evidence:
- `evidence/inbox/phase4-uds-readiness-2026-09-22.md`
- Existing guard baseline: Phase 3A `ReadOnlyGuard`
- Existing lower transport baseline: Phase 2A `DiagnosticTransport` / ISO-TP
- Project policy owners: `docs/READ_ONLY_POLICY.md`, `docs/ARCHITECTURE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@403c7f9b3a1cfcce410c3389debcb864646e6444`

Required implementation contract:
- Extend `ReadOnlyGuard` with a semantic UDS single-DID read entry point.
- Input is exactly one 16-bit DID; exact API symbol/name is implementation detail.
- Guard constructs exactly `22 DID_hi DID_lo` internally.
- Allowed UDS surface in this Stage is only service `0x22` with one DID.
- No arbitrary/raw UDS payload TX API may be exposed.
- Do not add `0x10`, `0x19`, `0x3E`, `0x14`, SecurityAccess, WriteDataByIdentifier, IO Control, RoutineControl or any other UDS service.
- Preserve existing OBD guard behavior exactly.
- Preserve lower `DiagnosticTransport::startSend` status in the existing guard result semantics; lower failure must not be translated to policy success.
- Keep the guard Generic Core / brand-independent and free of CAN/TWAI/VAG-specific types or DID meaning/scaling.
- Do not modify Phase 2A ISO-TP semantics.

Deterministic host-test minimum:
- representative DID request, e.g. `0xF190`, forwards exactly bytes `22 F1 90`.
- second representative DID proves byte order is deterministic.
- OBD Mode `01` / `09` existing allowlist tests remain PASS.
- existing denied OBD modes remain fail-closed.
- no generic/raw UDS TX entry point exists.
- lower `Busy`, `TxBusy`, `TxFailed`, `BusOff`, `NotInitialized` or equivalent relevant status propagation remains deterministic for allowed UDS read.
- source/API inspection confirms no CAN/TWAI/VAG-specific types or DID registry/scaling enter the guard.
- existing ISO-TP, ReadOnlyGuard OBD, and ObdService host regressions remain PASS.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later diff/code review, canonical reconciliation and coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new UDS guard tests.
- existing ISO-TP + OBD guard + OBD service regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim only Phase 4A UDS `0x22` outbound guard support; do not claim Generic UDS service semantics, NRC handling, DTC, session management, TesterPresent, VAG DID support or application-facing diagnostic TX.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If safe implementation requires exposing raw UDS TX, changing Phase 2A transport semantics, adding CAN/ECU routing, session/DTC/TesterPresent semantics, VAG DID knowledge, VehicleData or broader application TX plumbing, STOP and report instead of expanding scope.
- If the existing guard abstraction cannot add semantic `0x22` without a broader interface redesign, STOP before redesign and report the minimum required boundary change.

---
