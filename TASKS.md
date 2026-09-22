# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 3B Generic OBD-II read-only service core

Status: Ready

Goal:
- 在已完成的 `ReadOnlyGuard` safety gate之上，建立 host-testable Generic OBD-II read-only service semantics。
- 此 Stage 僅處理 bounded request/response semantics、matching、timeout、supported-PID block與 VIN；不做 DTC、UDS、VAG、VehicleData、routing或 scheduler。

Canonical evidence:
- `evidence/inbox/phase3-obd-readiness-2026-09-22.md`
- Phase 3A guard baseline: `5eefb442bc00e58005cc933fa6d56ab8bbe45ef6`
- Phase 2A transport baseline: `DiagnosticTransport` / `IsoTpTransport`
- Project policy owners: `docs/READ_ONLY_POLICY.md`, `docs/ARCHITECTURE.md`
- Shared Playbook readiness revision reviewed: `masini1491/ai-development-playbook@53158c560333bd8f701ceced77f998fcc4a6d093`

Required implementation contract:
- Future OBD service core must depend on `ReadOnlyGuard` and `Clock`; do not call `DiagnosticTransport::startSend` directly.
- Keep the service Generic Core / brand-independent and free of CAN/TWAI-specific types.
- Allow exactly one outstanding OBD request at a time; second request while active returns a deterministic Busy/in-progress service status without starting another lower TX.
- Support semantic Mode `0x01` single-PID request through the guard.
- Positive response identity for a Mode `0x01` request must match service `0x41` and the requested PID before it can complete that request.
- Support one supported-PID block request at a time for base PID `00/20/40/60/80/A0/C0`; matching response must provide exactly the 4-byte support bitmap after response service/PID and expose the bitmap in deterministic host-readable form. Do not auto-chain blocks in this Stage.
- Support VIN request only as Mode `0x09` PID `0x02`; matching response must be `49 02`, contain NODI `01`, and contain exactly 17 VIN bytes after NODI for this first slice.
- ISO-TP multi-frame assembly remains below this layer; OBD service receives an already reassembled diagnostic payload and must not implement FF/CF/FC logic.
- Use injected `Clock` for a configurable service response timeout. Timeout begins once outbound request transmission is accepted/complete enough to await a response; do not infer peer receipt from lower transport send success.
- Lower guard/transport start/poll failures must propagate as service failure, not success.
- A response with wrong service or PID must not satisfy the active request. It may be reported as unexpected while the original request remains pending until matching response or timeout.
- A matching supported-PID/VIN response with invalid structure/length is terminal invalid-response for that active request.
- Generic Mode `0x01` current-data response exposes raw data bytes only; no PID-specific scale/unit decoding in this Stage.
- Service completion/result API must distinguish at minimum: idle/in-progress/busy, response-ready/success, timeout, invalid/malformed response, unexpected response, policy/invalid request, and lower transport failure. Exact symbol names are implementation detail.

Deterministic host-test minimum:
- Mode `01` single-PID happy path: request goes through `ReadOnlyGuard`; `41 <PID> <data...>` matching response completes and returns raw data.
- Wrong positive response service does not complete active request.
- Correct service with wrong PID does not complete active request.
- Malformed short matching response is rejected deterministically.
- Second request while one is outstanding is Busy and does not create a second lower TX.
- Service-level no-response timeout is driven only by `FakeClock`.
- Lower start status failures from the guard/transport propagate deterministically.
- Lower poll failure while request is active terminates/returns failure deterministically.
- Supported-PID block `01 00` / response `41 00 A B C D` parses exact 32-bit bitmap.
- Representative continuation base such as `20` is accepted; non-block-aligned base for supported-PID helper is rejected before lower TX.
- VIN request forwards semantic `09 02`; matching `49 02 01 <17 bytes>` returns exactly 17 VIN bytes.
- VIN wrong NODI, short/long VIN payload, wrong service/PID are rejected or treated as unexpected according to the contract above.
- Existing ISO-TP and ReadOnlyGuard host regressions remain PASS.
- Source/API inspection confirms no CAN/TWAI-specific types leak into OBD service API and no direct raw `DiagnosticTransport::startSend` dependency bypasses `ReadOnlyGuard`.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, Stage scope, later code/diff review, canonical reconciliation and coordination cleanup.

Validation / completion:
- project host compile/tests PASS with new OBD service tests.
- existing ISO-TP + ReadOnlyGuard deterministic regressions PASS.
- `git diff --check` PASS.
- If ESP32-facing source participation/platform boundary does not change, do not require a new ESP32 compile solely for formality; record evidence scope honestly.
- Bench / Hardware / Vehicle remain Pending.
- canonical state owners may claim Phase 3B Generic OBD-II host service semantics only within this Stage scope; do not claim DTC, UDS, VAG routing, VehicleData, application-facing diagnostic TX or real-vehicle support.
- Codex does not remove this Hot Stage; ChatGPT performs completion reconciliation.

STOP:
- If safe implementation requires bypassing `ReadOnlyGuard`, changing Phase 2A ISO-TP semantics, adding CAN routing/functional broadcast/multi-ECU ownership, introducing PID scaling databases, DTC modes, UDS, scheduler/VehicleData, or broader application TX plumbing, STOP and report instead of expanding scope.
- If current guard/service API cannot express the required one-request-at-a-time semantics without a broader interface redesign, STOP before that redesign and report the minimum required boundary change.

---
