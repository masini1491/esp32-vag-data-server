# Project Instructions

## Language

- Codex 工作回報使用繁體中文。
- Source code symbol / API / protocol name 保持原文；文件以繁體中文為主，必要技術名稱使用英文。

## Source of truth and Git safety

- GitHub `main` 是 source of truth；開始工作前依任務需要確認 `git status`、branch、HEAD。
- Working tree 若有來源不明修改，停止，不自行清除。
- 除非明確要求，不得 `reset --hard`、force push、discard unknown changes、rewrite history 或自行刪除 unknown files。

## TASKS.md shared queue

- 若 repository root 存在 `TASKS.md`，任務開始時優先讀取。它是 ChatGPT／Codex 共用、只保留 unfinished work 的暫存 queue。
- 只執行使用者當次明確授權的 Task／Stage；不得因讀到 queue 就自行執行其他項目。
- 成功完成並驗證授權工作後，直接移除或更新對應 unfinished item；不得建立 Completed 區塊或把 `TASKS.md` 當 changelog。
- 若 validation 失敗、evidence 不足或遇到 blocker，保留 task 並記錄 Blocked／Deferred 狀態、evidence 與解除條件。
- 沒有任何 unfinished work 時刪除 `TASKS.md`；有新工作時再建立。

## Remote-sync bootstrap

- 執行 `TASKS.md` 中的工作前，先確認 repository identity、`git status --short`、branch、HEAD，並執行 `git fetch origin`。
- 只有在預期 branch、working tree clean、沒有 merge／rebase／cherry-pick 進行中，且 local 可由 `origin/main` fast-forward-only 時，才同步到最新 remote。
- 若 dirty、unexpected branch、local ahead／diverged、無法 fast-forward 或存在未完成 Git operation，立即 STOP 並回報；不得自行修復。
- 禁止 `reset --hard`、force push、自行 merge／rebase、stash、delete 或 discard unknown work。
- 同步完成後才讀最新 local `AGENTS.md` 與 `TASKS.md`；若 `TASKS.md` 或指定 Stage 已不存在，不得依舊 prompt、舊 SHA 或記憶繼續執行。

## Short-launch queue semantics

- `TASKS.md` 可保存完整且 scoped 的 Codex Prompt；使用者可在 Codex UI 手動設定模型與推理強度後，以短指令指定單一 Stage。
- Codex 只能執行當次指定的 Stage，不得自行執行 queue 中其他 Stage，也不得自行切換模型或提高推理強度。
- 「推薦模型」「推理強度」是規劃與回顧資訊，不是 Codex 自動切換設定。

## Library-ready design

- Generic protocol/data layers 維持未來可抽離 reusable library 的依賴方向，但目前不得為 library 化提前拆 repository、增加 speculative abstraction、package/release infrastructure 或沒有實際 consumer 的 generic API。
- Core、transport abstraction、ISO-TP、OBD、UDS、ReadOnlyGuard、generic VehicleData 與 profile interfaces 不得依賴 Arduino、ESP32/TWAI、Web、BLE、Wi-Fi 或特定品牌／車型實作。
- ESP32／Arduino-specific code 留在 platform／HAL／firmware boundary；Brand-specific code 不得反向滲入 Generic Core。
- UI、network、storage clients 依賴 VehicleData／application-facing interface；核心協議不得反向依賴 client。避免不必要 global mutable state 與 platform singleton，以維持 host testing／dependency injection 能力。
- 等 ISO-TP → OBD／UDS → Brand Layer → Vehicle Profile → VehicleData 的實際資料流穩定後，再評估 library extraction；目前不要建立 `library.properties`、package、另一個 repository、semantic versioning 或未使用的抽象層。

## Repository file roles and update thresholds

- `AGENTS.md` 是永久工作規則。
- `TASKS.md` 是唯一 active unfinished work／executable scoped Prompt queue。
- `CODEX_PROGRESS.md` 是 human-readable 歷史／project-state 摘要，不是 active queue。
- `VALIDATION.md` 是 validation contract、evidence 與 current Pending authority，不是 task queue。
- `docs/DEVELOPMENT.md` 是 roadmap／phase definitions，不作詳細 validation evidence ledger。
- `CHANGELOG.md` 是 release／change summary，不作 active queue 或 validation authority。
- Git history 是實際完成修改的最終權威。
- 只有 material project-state、重要 validation 或 Pending 狀態實質變更時，才更新 `CODEX_PROGRESS.md`／`VALIDATION.md`；純 queue bookkeeping、wording、排序、格式或小 maintenance 不應把歷程文件寫胖。
- 目前不要建立 `CODEX_TASKS.md`。未來只有真的累積值得永久保留的 archived Prompt/specification series 時，才另行評估；若建立，只能是 historical specification/index，不得成為第二個 active queue。

## Repository reading and evidence

採 progressive expansion：Level 0（diff/error/log/evidence）→ Level 1（direct symbol）→ Level 2（caller/callee）→ Level 3（完整相關檔案）→ Level 4（module/directory）→ Level 5（repository-wide）。只有上一級不足時才擴大。

Debug 流程：Evidence → Root Cause → Focused Patch → Targeted Validation。Root cause 分類為 `CONFIRMED ROOT CAUSE`、`HIGH-CONFIDENCE LIKELY ROOT CAUSE` 或 `INSUFFICIENT OBSERVABILITY`；後者先增加最小 diagnostics，不直接重構。

同一 root cause 最多 retry 一次；第二次失敗需重新分類為 SOURCE、TOOLCHAIN、ENVIRONMENT、INFRASTRUCTURE 或 SERVICE。Infrastructure/service error 重複兩次時停止 coding loop。

## Validation and hardware evidence

優先順序：static check → targeted verifier → targeted test → relevant build → required build matrix → full regression。明確區分 Software、Compile、Static/Test、Bench、Hardware、Network、Production PASS；沒有實體 evidence 的層級標記 Pending。

## Hardware abstraction

所有 GPIO 與 board-specific capability 必須集中於 `Board Profile → HardwareConfig → HAL`。CAN / ISO-TP / OBD / UDS / VAG / BLE / Web / application logic 不得硬編 GPIO number。

Generic Core 必須保持 brand-independent；VAG-specific routing、CAN ID、DID、scaling 與 semantics 只能存在於 Brand Layer / Vehicle Profile。Repository 維持 VAG-first；future brand support 需先有實際 evidence，不預先加入 speculative code。

單一 firmware build 不要求包含所有品牌；Brand Profile Set、Profile Resolver、Active Vehicle Profile 與 capability state 是 architecture boundaries，不在文件階段建立 implementation。Optional / Deep Diagnostic capability 必須顯式標示 `unsupported`、`unavailable` 或 `pending` / `unknown`，不得以假值代替；Deep Diagnostic 預設 on-demand，所有 diagnostic TX 永遠經 `ReadOnlyGuard`。

Diagnostic/application upper layers 不得假設所有 vehicle link 都是 CAN，亦不得直接依賴 CAN/TWAI-specific types；ISO-TP over Classic CAN 是 v1 concrete path，不是唯一 future transport。Future non-CAN evidence 不授權自行實作 K-Line 或 motorcycle support，且任何 non-CAN diagnostic TX 同樣不得繞過 `ReadOnlyGuard`。

## Read-only

不得引入 coding、adaptation、clear DTC、output tests、security access、flashing 或 actuator control。任何 diagnostic TX path 未來都必須經 `ReadOnlyGuard`。

## Reference reuse

實作 protocol / VAG 功能前先讀 `docs/references/SYNTHESIS.md`，再依 `docs/REFERENCES.md` 的 phase-specific reading set 讀必要 note；不要預設讀完整 reference library。除非符合 local Revisit trigger，禁止因「可能有關」重新 discovery upstream。引用或重用 upstream source 前，先確認 license 與 provenance。
