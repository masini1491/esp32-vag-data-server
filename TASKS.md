# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Governance ownership normalization

Status: Reconciliation correction required

Goal:
- 依 current `masini1491/ai-development-playbook` ownership architecture，對 `AGENTS.md` 做 subtractive normalization。
- 共通 cross-project engineering methodology 上移至 shared Playbook canonical owners；本 repo 僅保留 adoption/mapping、project-specific authority／stricter exceptions、local write surfaces、VAG/embedded/read-only/hardware-evidence invariants與 local reference routing。
- 不改變既有 Phase 2A ISO-TP scope、technical truth、source/tests或 validation state。

Canonical evidence:
- `evidence/inbox/governance-ownership-inventory-2026-09-21.md`
- Reviewed shared baseline: `masini1491/ai-development-playbook@42fcd13205df8c351d41ff7df1a4ff51cede6c85`
- Current adopter patterns cross-checked against `ai-divination-playbook`, `tw-stock-assistant`, and `access-control-system`.

Required normalization:
- 改成 project-native governance/task authority first；只有 activation 需要時才 resolve shared baseline / read shared `CHAT_INIT.md`。不得因 adoption 本身無條件 probe shared baseline。
- 保留唯一 `Playbook baseline: main` 與 `Project AI mode: ChatGPT+Codex`。
- Collapse generic fresh-session/bootstrap procedure、Hot/Cold lifecycle、remote-sync procedure、permission/network handling、Prompt/model/delegation、generic retry/failure taxonomy與 generic validation lifecycle為最低充分 canonical-owner pointers。
- 保留本 repo exact ChatGPT write allowlist：`/TASKS.md`、`/BACKLOG.md`、sanitized `/evidence/inbox/*.md`；不得擴權。
- 保留 GitHub `main` source-of-truth 與 clean fast-forward-only project restriction，但 generic Git procedure由 shared owner負責。
- 保留 Bench/Hardware/Vehicle physical-evidence restriction。
- 保留 `Library-ready design`、hardware abstraction、Generic Core/VAG boundary、future non-CAN boundary、read-only diagnostic policy／`ReadOnlyGuard` invariant、phase-specific reference reuse／Revisit trigger。
- 保留 project file-role mapping，但移除 generic completion/history methodology重述。
- 不建立新的 project-native router/owner，除非 current evidence證明有獨立 retrieval intent。

Writable scope:
- Codex may modify only `AGENTS.md`.
- Codex must not modify `TASKS.md`, `BACKLOG.md`, `evidence/**`, README/docs, `VALIDATION.md`, source, tests, tooling, workflow or roadmap.

Reconciliation finding:
- Final `AGENTS.md` 的 `TASKS.md shared queue` 仍寫「本 repository 的 queue cleanup requirement 以本節為準」，但本節已不再定義任何 local cleanup requirement；shared `REPOSITORY_EXECUTION.md → Hot completion` 已完整擁有通用 completion/EMPTY semantics。這形成 dangling local authority。Codex 應將該句收斂為 shared-owner pointer（例如將 cleanup 一併列入 shared lifecycle/completion semantics），不要重新複製 completion procedure，也不要改變 TASKS persistent-mode behavior。

Validation / completion:
- Final diff only `AGENTS.md`.
- `git diff --check` PASS。
- Adoption/governance structural checks available to this repository remain PASS if applicable.
- Re-read final `AGENTS.md` and prove: baseline/mode remain unique; local write allowlist unchanged; project-specific VAG/read-only/hardware-evidence restrictions survive; shared-methodology duplication is materially reduced; no technical authority or Phase 2A scope changed.
- Commit/push `main` and report final HEAD / changed files / exact Playbook revision used.
- Codex 不清理本 Stage；由 ChatGPT completion reconciliation 後移除。

STOP:
- 若 normalization 需要修改 `AGENTS.md` 以外檔案，或無法區分某條規則是否 project-specific stricter override，STOP並回報，不自行刪除或上移。
- 不得因治理瘦身順手執行 Phase 2A 或其他 implementation work。

---

## HOT — Phase 2A ISO-TP host core

Status: Ready

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
