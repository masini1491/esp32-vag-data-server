# TASKS

本檔案是本 repository 唯一的 active unfinished-work / executable scoped Prompt queue，只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作；Cold／future-trigger 工作在 project governance 正式啟用 Cold Registry 前仍暫留於本檔，migration 完成後依最新 governance 分流。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---

## TODO

- [ ] **Stage — Adopt Multi-Surface coordination / Cold Registry migration**

  **Goal**：依最新版 `masini1491/ai-development-playbook` 的 AI Context / Coordination Write Allowlist 架構，讓本 repository 從目前 default Single-Surface Mode 遷移到最低充分 Multi-Surface Mode：`TASKS.md` 僅保存 Hot/current critical-path coordination，新增 root `BACKLOG.md` 作為 Cold Registry，並把現有 3 個 future-trigger Deferred item 分流到 Cold surface；同時補齊 project `AGENTS.md` 對 `AI_CONTEXT.md` 的 routing 與新的 ChatGPT Coordination Write Allowlist。

  **User authorization / authority premise**：使用者已在本聊天室明確同意執行上述 migration。此授權允許 Codex 在本 Stage scope 內修改 governance 與必要 coordination files；在 remote canonical read-back 證明新 governance 生效前，ChatGPT 仍只能直接寫既有 allowlist 的 `/TASKS.md`，不得先自行建立／寫入 `/BACKLOG.md`。

  **Recommended execution settings**：Luna / Low；Context L0 → L2；Agent 1；Execution mode = focused governance/docs migration。若 concrete evidence 顯示 scope 超出 bounded docs/coordination migration，STOP，不自行升級 model/scope。

  **Required work**：
  1. 依 project governance 做 repository identity、Git state、safe fetch / fast-forward-only sync preflight，並重讀 latest local `AGENTS.md` / `TASKS.md`。
  2. 依最新版 common playbook routing，只讀本 Stage 真正需要的最低 sections：`AI_CONTEXT.md`（Hot/Cold/Cold Registry、default-load、migration semantics）、`REPOSITORY_EXECUTION.md`（Coordination Write Allowlist、Single-Surface → Multi-Surface adoption、ChatGPT/Codex write boundary）、`CHATGPT_WORKFLOW.md`（Persistence/Coordination Admission、Cold/Hot semantics、Prompt mode）與必要 validation/completion sections；不要完整重掃整份 Playbook。
  3. 更新 root `AGENTS.md` 的 common routing，新增 `AI_CONTEXT.md` 對 AI Context／Hot-Cold-Evidence／routing／retrieval-cost 的 authority 指向；保留現有 project-specific authority 與 technical rules。
  4. 在 `AGENTS.md` 明確宣告本 project 的 **ChatGPT Coordination Write Allowlist** 目前只有：`/TASKS.md`、`/BACKLOG.md`。未列入 path（包括 `AGENTS.md`、README/docs、source、tests、tooling、workflow、validation authority 等）對 ChatGPT 仍為 read-only；不得順便啟用 Hot dossier 或 evidence staging。
  5. 將 project coordination semantics 收斂為：
     - `TASKS.md` = Hot/current executable or critical-path coordination；普通 bootstrap 讀 current Hot surface。
     - `BACKLOG.md` = Cold Registry；預設不進 ordinary bootstrap、沒有 execution authority、不可直接 TASKS Short-launch。
     - Cold item 的 trigger 成立或使用者選中後，先重讀 current authority/evidence、reconcile premise，再 promote 到 `TASKS.md` 後才可 launch。
     - Persistent `TASKS.md` 無 Hot work 時保留最低充分 `EMPTY` state；`EMPTY` 不代表沒有 Cold work。
  6. 新增 root `BACKLOG.md`，只承擔 Cold/future-trigger durable memory。把目前 `TASKS.md` 的三個 Deferred item完整搬入，不改變原本 commitment/trigger semantics，不因搬檔升格成 executable/committed obligation：
     - `64-bit monotonic timestamp`
     - `Generic namespace 命名`
     - `ESP32 backend CI coverage`
     Cold surface應保留各 item 的現有 why/trigger/限制，並明確標示未 promotion 前不可執行。
  7. migration 完成後從 `TASKS.md` 移除上述三個 Deferred item；本 Stage 本身在 validation / commit / push 完成後也依正常 queue lifecycle移除，最後將 `TASKS.md` 收斂成最低充分 `EMPTY` Hot coordination state。
  8. 做 bounded derived-dependency audit：確認 README、manifest、project-scale/stat、retention/ignore 或其他 tracked rule是否會因新增 `BACKLOG.md` 產生 material stale/correctness問題。現有 README目前沒有 coordination-surface claim；沒有 direct evidence時不要修改 README或其他 unrelated docs。若發現完成 migration 必須修改本 Stage未列出的 material derived dependency，STOP並回報 exact dependency，不自行擴張 scope。

  **Allowed scope**：`AGENTS.md`、`TASKS.md`、新增 `BACKLOG.md`，以及完成上述 migration所需的最低充分 targeted validation / Git bookkeeping。除非第 8 點出現直接 evidence並先 STOP，不修改其他 tracked files。

  **Forbidden scope**：production source、tests、protocol/architecture behavior、hardware mapping、VALIDATION evidence、CODEX_PROGRESS、CHANGELOG、README cosmetic cleanup、Hot task dossier、evidence staging、CI/tooling、unrelated governance slimming/refactor。

  **Validation / completion**：
  - `AGENTS.md` 有正確 `AI_CONTEXT.md` routing，且 allowlist只新增 `/BACKLOG.md`，未替 ChatGPT擴張其他 path。
  - `TASKS.md` / `BACKLOG.md` Hot-Cold responsibility清楚、沒有雙重 current authority；三個 Deferred只存在 Cold Registry一次，trigger/語意未被升格。
  - Cold item不可直接 execution / Short-launch；Hot promotion contract明確。
  - `TASKS.md` 最終為 persistent `EMPTY` Hot state。
  - targeted stale-reference / contradiction check與 `git diff --check` PASS。
  - scoped diff只包含本 Stage允許的 migration files；commit/push至 `origin/main`。
  - 若任何 governance/derived dependency/working-tree/branch/sync evidence與本 Stage premise不一致，STOP並回報，不自行修復或擴張。

---

## Deferred — migration source state

> 下列三項僅在本 migration 完成前暫留，避免 governance 尚未生效時遺失 durable state。Codex完成本 Stage時必須把它們搬到 `BACKLOG.md`，並從本節移除；不得在兩個 surface重複保留。

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；等真正需要 frame freshness / long-running VehicleData semantics 時再處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。
- [ ] **ESP32 backend CI coverage** — Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。

---
