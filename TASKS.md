# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 2A ISO-TP host core

Status: Reconciliation correction required

Goal:
- 建立 host-testable、Classic CAN v1 的 ISO-TP core / DiagnosticTransport boundary。
- 僅處理 transport framing/state-machine contract，不接 OBD-II、UDS、VAG、VehicleData 或任何 application-facing live diagnostic request path。

ChatGPT-readiness evidence:
- 現有 foundation：`CanHal`、`CanFrame`、`Clock`、`MockCan`、`FakeClock`。
- Local Phase 2 references：`docs/references/SYNTHESIS.md`、`docs/references/can/OBDII_ESP32.md`、`docs/references/can/OPENHALDEX_S3.md`。
- Bounded revisit evidence：`evidence/inbox/phase2-isotp-revisit-2026-09-20.md`。
- `CanHal::send() == Ok` 僅代表 backend accepted into transmission path，不代表 on-wire completion/ACK。
- `CanHal::receive()` 為 destructive FIFO；Stage 2A 不得建立會默默吞掉 unrelated CAN traffic 的 shared-bus architecture。
- `Clock::nowMs()` 僅有 millisecond resolution；Stage 2A 必須對 ISO-TP sub-ms STmin encoding 採明確保守 policy，不得假裝具備 microsecond scheduling。

Required implementation contract:
- 支援 v1 Classic CAN normal addressing的 Single Frame、First Frame、Consecutive Frame、Flow Control 基本 semantics。
- TX/RX CAN ID / frame format 必須由 transport configuration 明確提供；不要把 VAG ECU route、OBD/UDS service semantics 或 TWAI type 寫進 core。
- Receive reassembly需有 bounded payload capacity / overflow handling、sequence-number validation、malformed-frame rejection、timeout handling。
- Transmit segmentation需正確處理 peer Flow Control、Block Size、STmin、Wait/Overflow，以及 `CanStatus` failures/backpressure。
- state machine / polling API 應可由 host test外部驅動；禁止 busy wait / sleep-based test。
- timeout 使用 `Clock`；不要 promotion 64-bit frame timestamp backlog。
- v1 首輪不實作 CAN FD、extended/mixed addressing、K-Line、DoIP、OBD-II、UDS、ReadOnlyGuard runtime、brand routing。
- 不得建立 arbitrary raw diagnostic TX API 給 application/client 使用。

Deterministic host-test matrix minimum:
- SF RX / TX happy path與最大 classic-CAN SF payload boundary。
- FF + CF reassembly；多個 CF sequence progression與 wrap behavior。
- malformed PCI / invalid length / truncated frame。
- unexpected CF / wrong sequence number。
- RX payload overflow / declared length beyond configured capacity。
- RX timeout。
- TX multi-frame segmentation。
- FC CTS with BS=0 and finite BS。
- FC Wait bounded handling / timeout；FC Overflow failure。
- STmin millisecond values；sub-ms code path依 Stage 2A 明確 policy deterministic 驗證。
- CAN send `TxBusy` / `TxFailed` / `BusOff` / `NotInitialized` 等 relevant propagation。
- RX filtering/routing不得把非 configured RX ID frame誤當 ISO-TP payload；若 current `CanHal` abstraction不足以保證 shared-bus preservation，STOP並回報所需最小 architecture change，不自行重寫 CAN HAL。

Actor ownership:
- Codex：`src/**`、`tests/**` 與本 Stage 必要的 canonical docs/validation mutation。
- ChatGPT：Stage scope/research、後續 code/diff review、evidence reconciliation、必要的 TASKS/BACKLOG/evidence 維護與 completion cleanup。

Reconciliation finding:
- Implementation / host tests / CI at `43a550de1e835fffad22e63f29e03943b6dea6c5` satisfy the Phase 2A software contract, but current canonical project-state owners are stale:
  - `VALIDATION.md` still records Phase 1 / Stage 4T host+CI evidence and does not record current Phase 2A host/CI evidence.
  - `CODEX_PROGRESS.md` still says Phase 2 ISO-TP implementation has not started.
  - `docs/DEVELOPMENT.md` still says Phase 2 ISO-TP has not started.
  - `README.md` still lists ISO-TP as not started / next stage.
- This is residual completion closure inside the same admitted Stage, not a new feature Stage. Codex may update only those four canonical state/documentation owners as needed to reflect the already-completed Phase 2A boundary and evidence; do not change source/tests or expand Phase 2 scope.
- Preserve Bench / Hardware / Vehicle as Pending; do not promote ESP32 compile evidence beyond its existing Phase 1 scope, and explicitly note Phase 2A did not require a new ESP32 compile because platform participation/boundary did not change.

Validation / completion:
- project-required host compile/tests PASS。
- 新 ISO-TP tests 全部 deterministic PASS。
- `git diff --check` PASS。
- 若 Stage 修改 ESP32-facing compile participation或 existing platform boundary，再依 current `VALIDATION.md` / Playbook 執行相應 targeted compile；否則不要為形式宣稱 Hardware/Vehicle evidence。
- Bench / Hardware / Vehicle 保持 Pending。
- Codex completion後不要自行清理本 Hot Stage；由 ChatGPT canonical reconciliation 後處理。

STOP:
- 若安全實作需要先改變 `CanHal` shared-consumer semantics、引入新的 bus dispatcher/ownership contract、提前啟用 application diagnostic TX、或需要超出 Phase 2A 的 architecture mutation，STOP並回報，不自行擴張。
- 若現有 local reference不足以決定 material protocol behavior，先回報 evidence gap，不以猜測補 RFC/standard semantics。

---
