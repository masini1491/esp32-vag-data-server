# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 3A ReadOnlyGuard safety gate

Status: Ready

Goal:
- 在 Generic OBD-II service implementation 之前，先建立不可繞過的 read-only diagnostic TX policy boundary。
- 此 Stage 僅建立最小 host-testable `ReadOnlyGuard` outbound enforcement，不實作 OBD response parser、UDS、VAG routing、VehicleData 或 live vehicle integration。

Canonical evidence:
- `evidence/inbox/phase3-obd-readiness-2026-09-22.md`
- Phase 2A lower transport baseline: `DiagnosticTransport` / `IsoTpTransport`
- Project policy owners: `docs/READ_ONLY_POLICY.md`, `docs/ARCHITECTURE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@53158c560333bd8f701ceced77f998fcc4a6d093`

Required implementation contract:
- Dependency direction: future OBD service code must send through `ReadOnlyGuard` before `DiagnosticTransport`; the guard must not implement another ISO-TP state machine.
- Expose a semantic single-PID OBD request entry point; exact class/method spelling is implementation detail.
- Phase 3A allowlist only:
  - Mode `0x01` + one PID
  - Mode `0x09` + one PID
- Everything else fails closed and must not invoke lower transport TX.
- Denial status must be distinguishable from malformed input and lower transport failure.
- Allowed request must preserve the lower `DiagnosticTransport::startSend` status without translating a transport failure into policy success.
- No arbitrary raw diagnostic payload TX API may be exposed through the guard.
- `poll()` / receive behavior may delegate transparently to the lower transport; do not duplicate transport framing/state.
- Keep the implementation Generic Core / brand-independent and free of CAN/TWAI-specific types.
- Do not implement UDS allowlist in this Stage.
- Do not add Mode 03 / Mode 07 / DTC read semantics yet even if read-only; those require later explicit admission.
- Mode `0x04` and every non-allowlisted mode must be denied.

Deterministic host-test minimum:
- Mode `01` allowed request forwards exactly `01 <PID>`.
- Mode `09` allowed request forwards exactly `09 <PID>`.
- Mode `04` denied and lower transport send count remains unchanged.
- representative other non-allowlisted mode denied and lower transport send count remains unchanged.
- malformed/invalid semantic request rejected without lower TX.
- lower transport `Busy`, `TxBusy`, `TxFailed`, `BusOff`, `NotInitialized` or equivalent relevant status propagation remains deterministic for allowed requests.
- `poll` / receive pass-through behavior does not invent service completion semantics.
- compile-time/source inspection confirms no CAN/TWAI-specific types leak into the guard API.

Actor ownership:
- Codex: source/tests and canonical docs/validation mutation required by this Stage.
- ChatGPT: readiness/evidence, Stage scope, later code/diff review, canonical reconciliation, coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new guard tests.
- existing ISO-TP host regression remains PASS.
- `git diff --check` PASS.
- If no ESP32-facing source participation/platform boundary changes, do not require a new ESP32 compile solely for formality; record scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners must not claim OBD-II service semantics complete; only Phase 3A guard completion may be recorded.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If a safe guard requires modifying Phase 2A transport semantics, exposing a raw diagnostic bypass, changing CAN routing/dispatcher ownership, or defining UDS/OBD service semantics beyond this allowlist boundary, STOP and report the minimum architecture issue instead of expanding scope.
- If current evidence shows the guard cannot be placed in front of all future service TX without a broader interface redesign, STOP before implementing that redesign.

---
