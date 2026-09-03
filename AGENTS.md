# Project Instructions

## Language

- Codex 工作回報使用繁體中文。
- Source code symbol / API / protocol name 保持原文；文件以繁體中文為主，必要技術名稱使用英文。

## Common playbook routing

- 本專案以 `masini1491/ai-development-playbook` 作為共通 development baseline；本檔與 repository 正式 technical contracts 保存 project-specific authority。
- Authority hierarchy：user current instruction → latest project governance／technical source of truth → common playbook → `TASKS.md` → stale prompt／cached copy／memory。
- 只依當次 Task route 到最低必要章節，不完整掃描 playbook：Git／repository／permission／external service → `REPOSITORY_EXECUTION.md`；debug／root cause／retry／validation → `DEBUG_VALIDATION.md`；architecture／research／external authority → `RESEARCH_ARCHITECTURE.md`；ESP32／embedded／hardware → `EMBEDDED_PROJECTS.md`；AI Context／Hot-Cold-Evidence responsibility／default-load／routing／retrieval-cost → `AI_CONTEXT.md`；ChatGPT project planning／TASKS admission／Prompt mode／delivery／copy-ready／Codex result reconciliation → `CHATGPT_WORKFLOW.md`；Codex model／Reasoning／Context／Agent／execution mode／cost／tool discipline／reporting → `CODEX_EXECUTION.md`；Windows／PowerShell／local runtime → relevant 時才讀 `TOOLCHAIN.md`。
- External playbook reference 只代表 routing，不代表 execution environment 自動具有 network、filesystem 或 credential capability；需要時仍須通過正式 permission gates。

## Source of truth and Git safety

- GitHub `main` 是 source of truth；開始工作前依任務需要確認 `git status`、branch、HEAD。
- Working tree 若有來源不明修改，停止，不自行清除。
- 除非明確要求，不得 `reset --hard`、force push、discard unknown changes、rewrite history 或自行刪除 unknown files。

## TASKS.md shared queue

- `TASKS.md` 是本 repository 唯一 active unfinished／executable queue；若存在，任務開始時讀取並只執行使用者當次授權的 Task／Stage。
- 成功驗證後移除／更新對應 unfinished item；不建立 Completed 區塊或把 queue 當 changelog。沒有 unfinished work 時保留 `TASKS.md`，只保留最小 `EMPTY` coordination state，不刪除檔案。
- `TASKS.md` 是 Hot/current executable 或 critical-path coordination；普通 bootstrap 讀取 current Hot surface。`BACKLOG.md` 是 Cold Registry，預設不載入 ordinary bootstrap、沒有 execution authority，也不可直接用於 TASKS Short-launch。
- Cold item 的 trigger 成立或被使用者選中後，先重讀 current authority/evidence、reconcile premise，再 promote 到 `TASKS.md`，之後才可依正常 authorization launch；Cold work 不因持久化而取得 execution authority。
- Queue lifecycle、Prompt discipline 與一般 execution semantics 依 common playbook routing；本 repository 的 project-specific queue scope 與 cleanup requirement 以本節為準。

## ChatGPT Coordination Write Allowlist

- ChatGPT direct-write coordination allowlist 僅有 `/TASKS.md` 與 `/BACKLOG.md`。未列入 path（包括 `AGENTS.md`、README/docs、source、tests、tooling、workflow 與 validation authority）對 ChatGPT 仍為 read-only。
- 本 project 不啟用 Hot task dossier 或 evidence staging；新增 allowlisted Cold Registry 不改變其他 path 的寫入邊界。

## Remote-sync bootstrap

- 執行 `TASKS.md` 中的工作前，先確認 repository identity、`git status --short`、branch、HEAD，並執行 `git fetch origin`。
- 只有在預期 branch、working tree clean、沒有 merge／rebase／cherry-pick 進行中，且 local 可由 `origin/main` fast-forward-only 時，才同步到最新 remote。
- 若 dirty、unexpected branch、local ahead／diverged、無法 fast-forward 或存在未完成 Git operation，立即 STOP 並回報；不得自行修復。
- 禁止 `reset --hard`、force push、自行 merge／rebase、stash、delete 或 discard unknown work。
- 同步完成後才讀最新 local `AGENTS.md` 與 `TASKS.md`；若 `TASKS.md` 或指定 Stage 已不存在，不得依舊 prompt、舊 SHA 或記憶繼續執行。

## Permission-Gated Operation

- Permission-gated execution、capability layers、external network／service、credential boundary 與一般 failure handling 依 common playbook routing；本 repository 仍採更嚴格的 Git safety／fast-forward-only 規則。
- `git fetch origin` 遇到 `.git/FETCH_HEAD: Permission denied`、Git lock/ref file 無法建立或 sandbox 阻擋 repository metadata 寫入時，先依 playbook permission gate 處理，不先判定 environment failure。
- 禁止以 permission workaround 繞過安全規則：`sudo`、`chmod -R 777`、`reset --hard`、force push、自行刪除 `.git/FETCH_HEAD`、未確認原因就刪除 `.git/index.lock` 或其他 lock、重新 clone 覆蓋 working tree、stash/delete/discard unknown user work、自行 merge/rebase/cherry-pick，或以另一 repository 繞過目前問題。
- External network、external API／CLI／HTTPS、remote service、package／dependency retrieval 與 credential capability 等 execution boundary，遵守最新版 `masini1491/ai-development-playbook` 的 `REPOSITORY_EXECUTION.md`（Authorization／Capability Layers、Permission-Gated Operation、External network／service boundary、Remote Git Permission Gate）。本 repository 更嚴格的 Git safety、fast-forward-only 與 forbidden workaround 規則繼續適用；任何 permission／network approval 或 credential capability 都不會擴張 Task／Stage authorization。

## Short-launch queue semantics

- Prompt mode／TASKS Short-launch／copy-ready 等 ChatGPT prompt-generation semantics 依 `CHATGPT_WORKFLOW.md`；model／Reasoning／Context／Agent 與 Codex execution discipline 依 `CODEX_EXECUTION.md`。使用者手動選擇模型與推理強度，Codex 不得自行升級或執行未授權 Stage。

## Library-ready design

- Generic protocol/data layers 維持未來可抽離 reusable library 的依賴方向，但目前不得為 library 化提前拆 repository、增加 speculative abstraction、package/release infrastructure 或沒有實際 consumer 的 generic API。
- Core、transport abstraction、ISO-TP、OBD、UDS、ReadOnlyGuard、generic VehicleData 與 profile interfaces 不得依賴 Arduino、ESP32/TWAI、Web、BLE、Wi-Fi 或特定品牌／車型實作。
- ESP32／Arduino-specific code 留在 platform／HAL／firmware boundary；Brand-specific code 不得反向滲入 Generic Core。
- UI、network、storage clients 依賴 VehicleData／application-facing interface；核心協議不得反向依賴 client。避免不必要 global mutable state 與 platform singleton，以維持 host testing／dependency injection 能力。
- 等 ISO-TP → OBD／UDS → Brand Layer → Vehicle Profile → VehicleData 的實際資料流穩定後，再評估 library extraction；目前不要建立 `library.properties`、package、另一個 repository、semantic versioning 或未使用的抽象層。

## Windows / PowerShell routing

- Windows／PowerShell local runtime contract 僅在 repository-owned tooling 存在或當次 Task 相關時，route 至 common playbook `TOOLCHAIN.md`；目前本 repository 沒有 tracked `.ps1`，不在此重複維護通用 runtime policy。

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

- Progressive reading、debug/root-cause、retry taxonomy 與 validation ladder 依 common playbook routing（分別 route 至 `REPOSITORY_EXECUTION.md`／`DEBUG_VALIDATION.md` 等最低必要章節）；不因本 repository 而完整複製 playbook。
- Operational failure taxonomy 固定為 `SOURCE`、`TOOLCHAIN`、`ENVIRONMENT`、`INFRASTRUCTURE`、`SERVICE`、`AUTHENTICATION`、`AUTHORIZATION`、`HARDWARE_REQUIRED`；permission gate resolution 不計 operational retry，gate 尚未解除前不分類為上述 failure。`AUTHENTICATION`／`AUTHORIZATION` 不得以擴大 sandbox／network permission、blind retry、提高 credential privilege 或 production source patch 猜測修復；既有 non-compile retry cap 與 compile/source-fix override semantics 維持不變。
- 本 repository-specific evidence rule：沒有實體 evidence 的 Bench、Hardware、Vehicle 層級一律標記 Pending，不得由 software／compile evidence 推導。

## Validation and hardware evidence

Validation ladder、evidence recording 與 failure handling route 至 common playbook `DEBUG_VALIDATION.md`。本 repository-specific rule：沒有實體 evidence 的 Bench、Hardware、Vehicle 層級一律標記 Pending，不得由 software／compile evidence 推導。

## Hardware abstraction

所有 GPIO 與 board-specific capability 必須集中於 `Board Profile → HardwareConfig → HAL`。CAN / ISO-TP / OBD / UDS / VAG / BLE / Web / application logic 不得硬編 GPIO number。

Generic Core 必須保持 brand-independent；VAG-specific routing、CAN ID、DID、scaling 與 semantics 只能存在於 Brand Layer / Vehicle Profile。Repository 維持 VAG-first；future brand support 需先有實際 evidence，不預先加入 speculative code。

單一 firmware build 不要求包含所有品牌；Brand Profile Set、Profile Resolver、Active Vehicle Profile 與 capability state 是 architecture boundaries，不在文件階段建立 implementation。Optional / Deep Diagnostic capability 必須顯式標示 `unsupported`、`unavailable` 或 `pending` / `unknown`，不得以假值代替；Deep Diagnostic 預設 on-demand，所有 diagnostic TX 永遠經 `ReadOnlyGuard`。

Diagnostic/application upper layers 不得假設所有 vehicle link 都是 CAN，亦不得直接依賴 CAN/TWAI-specific types；ISO-TP over Classic CAN 是 v1 concrete path，不是唯一 future transport。Future non-CAN evidence 不授權自行實作 K-Line 或 motorcycle support，且任何 non-CAN diagnostic TX 同樣不得繞過 `ReadOnlyGuard`。

## Read-only

不得引入 coding、adaptation、clear DTC、output tests、security access、flashing 或 actuator control。任何 diagnostic TX path 未來都必須經 `ReadOnlyGuard`。

## Reference reuse

實作 protocol / VAG 功能前先讀 `docs/references/SYNTHESIS.md`，再依 `docs/REFERENCES.md` 的 phase-specific reading set 讀必要 note；不要預設讀完整 reference library。除非符合 local Revisit trigger，禁止因「可能有關」重新 discovery upstream。引用或重用 upstream source 前，先確認 license 與 provenance。
