# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 4B Generic UDS ReadDataByIdentifier service core

Status: Ready

Goal:
- 在已完成的 Phase 4A semantic UDS `0x22` guard 之上，建立 host-testable Generic UDS `ReadDataByIdentifier` service semantics。
- 本 Stage 僅處理 single-DID request/response matching、negative response、NRC `0x78` response-pending 與 timeout；不做 session、DTC、TesterPresent、VAG routing/scaling、VehicleData 或 application-facing TX。

Canonical evidence:
- `evidence/inbox/phase4-uds-readiness-2026-09-22.md`
- Phase 4A guard baseline: `4b8609302dc6569af53d7718d4ea75c152c682cb`
- Existing lower transport baseline: Phase 2A `DiagnosticTransport` / ISO-TP
- Project policy owners: `docs/READ_ONLY_POLICY.md`, `docs/ARCHITECTURE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@403c7f9b3a1cfcce410c3389debcb864646e6444`

Required implementation contract:
- Generic UDS service depends on `ReadOnlyGuard` and injected `Clock`; do not call `DiagnosticTransport::startSend` directly.
- Exactly one outstanding DID read at a time; second request while active returns deterministic Busy without lower TX.
- Outbound request only through the semantic Phase 4A guard entry point.
- Positive response must match `62 DID_hi DID_lo <data...>`.
- Wrong positive SID or DID is `UnexpectedResponse` and leaves the request active.
- Matching positive response with no data bytes is terminal `InvalidResponse`.
- Generic negative response handling:
  - matching `7F 22 NRC` with NRC `0x78`: keep request active and refresh a bounded response-pending deadline;
  - matching `7F 22 NRC` with any other NRC: terminal negative-response result and expose the NRC;
  - malformed negative response: terminal `InvalidResponse`;
  - `7F` for a different request SID: `UnexpectedResponse` and leave request active.
- Initial response timeout begins only after lower transmission has completed enough to await response.
- Use injected `Clock` for response timeout and response-pending timeout.
- Repeated NRC `0x78` must be bounded by configurable max pending count (or equivalent bounded pending budget); it must not extend forever.
- Lower guard/transport start/poll/receive failures propagate deterministically as transport failure.
- Return raw DID data bytes only; do not decode DID meaning/scaling.
- Keep API Generic Core / brand-independent and free of CAN/TWAI/VAG-specific types.
- Do not modify Phase 2A ISO-TP semantics or Phase 3 OBD semantics.

Deterministic host-test minimum:
- representative DID request (e.g. `0xF190`) forwards via `ReadOnlyGuard` and matching `62 F1 90 <data>` completes with exact raw bytes.
- wrong positive SID leaves request active as unexpected.
- correct `0x62` with wrong DID leaves request active as unexpected.
- matching `62 DID_hi DID_lo` with no data is invalid response.
- second request while active is Busy and does not create lower TX.
- initial no-response timeout is FakeClock-driven.
- lower start and poll/receive failure propagation is deterministic.
- matching `7F 22 78` keeps request active and refreshes pending deadline.
- repeated `0x78` reaches configured bound deterministically.
- matching terminal NRC (representative non-`0x78`) completes as negative response and exposes NRC.
- malformed `7F` response is invalid.
- `7F` for another request SID is unexpected and keeps current request active.
- existing ISO-TP, ReadOnlyGuard OBD/UDS, ObdService regressions remain PASS.
- API/source inspection confirms no direct raw `DiagnosticTransport::startSend`, CAN/TWAI/VAG types, DID registry or scaling enters Generic UDS service.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later code/diff review, canonical reconciliation and coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new UDS service tests.
- existing ISO-TP + OBD + ReadOnlyGuard regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim only Generic UDS `0x22` host service semantics within this Stage scope; do not claim session management, DTC, TesterPresent, VAG DID support/scaling, application-facing diagnostic TX or real-vehicle support.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If implementation requires bypassing `ReadOnlyGuard`, modifying ISO-TP semantics, adding session/DTC/TesterPresent, CAN/ECU routing, VAG DID knowledge/scaling, scheduler/VehicleData or application TX plumbing, STOP and report instead of expanding scope.
- If correct NRC `0x78` handling requires a broader transport or dispatcher redesign, STOP before that redesign and report the minimum required boundary issue.

---
