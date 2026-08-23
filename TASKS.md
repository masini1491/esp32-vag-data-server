# TASKS

本檔案是 ChatGPT／Codex 共用的暫存工作佇列與可執行 Prompt queue，只保留尚未完成的 TODO、Blocked、Deferred、待驗證項目。正式規格、`AGENTS.md`、architecture、spec、security policy、protocol evidence 與其他 source of truth 優先於本檔案；若衝突，以正式規格／規則為準。

## ChatGPT GitHub write boundary

- ChatGPT 對本 repository 的 GitHub 操作僅可建立、更新或刪除根目錄 `TASKS.md`。
- 除 `TASKS.md` 外，repository 其他檔案對 ChatGPT 一律唯讀；ChatGPT 可讀取 source、tests、`AGENTS.md`、architecture、docs、workflow 等進行分析、code review、規劃與產生 Codex Prompt，但不得直接修改、建立、刪除或提交其他 repository 檔案。
- 實際 source code、tests、docs、workflow、`AGENTS.md` 等修改交由使用者當次明確授權的 Codex 工作處理，除非使用者日後明確變更此限制。

## Queue lifecycle

- `TASKS.md` 只保留尚未完成工作；Git commit/history 才是完成紀錄，不建立 Completed 區塊，也不把本檔案當 changelog。
- Codex 每次只執行使用者當次明確授權的 Task/Stage，不得因看到本檔案就自行完成整份 queue。
- 新發現且確實需要後續執行的 bug、改善或驗證工作可加入本檔案；不要加入沒有實際行動價值的想法。
- 成功完成並驗證一個 Task/Stage 後，直接刪除或更新對應 unfinished item；若該 Stage 的完整 Prompt 已不再需要，也一併移除該 Stage Prompt，避免本檔案累積成歷史紀錄。
- 若 validation 失敗、evidence 不足或遇到環境 blocker，不得移除 task；改為 `Blocked` 或保留 unfinished 狀態，簡短記錄 blocker、已知 evidence 與解除條件。
- 完全沒有任何 unfinished work 時刪除 `TASKS.md`；未來有新工作時再重新建立。

## 模型與執行設定的角色

每個 Stage / Task 中的「推薦模型」「推理強度」「Context」「Execution mode」都是 ChatGPT 規劃時保存的**建議值與回顧依據**，不是 Codex 自動切換模型的指令。

真正執行前：
- 使用者與 ChatGPT 依最新 GitHub / `TASKS.md` 狀態討論要執行的 Stage。
- ChatGPT 提供建議模型與推理強度。
- **使用者在 Codex UI 手動設定模型與推理強度。**
- Codex 不得自行升級 Luna → Terra → Sol，也不得自行提高 Low → Medium → High；遇到 escalation 條件時先 STOP 並回報 evidence，由使用者決定是否換模型/強度重新執行。

模型選擇原則：使用能安全完成目前任務的最低成本模型。
- **Luna**：已知 root cause、機械式修改、Git、文件、搜尋整理、targeted tests、低風險 focused patch。
- **Terra**：一般程式開發、整合、debug、runtime/state ownership，或 Luna 已有 evidence 顯示需要更高階局部推理。
- **Sol**：只考慮高風險跨模組、安全、協議、併發、架構等複雜推理；不得無 evidence 直接升級。
- 預設單一 Agent；不預設 Multi-Agent、Fast、Ultra、最大 Context 或 full-repository scan。

每個新增的 Codex Stage / Task 應盡量包含：
- 推薦模型：Luna / Terra / Sol
- 推理強度：Low / Medium / High
- 推薦理由
- 是否值得先用較便宜模型做前置蒐證
- Context 建議
- Execution mode 建議
- dependency / 觸發條件
- escalation 條件
- 完整但 scoped 的 Codex Prompt
- targeted validation
- 成功後如何更新 `TASKS.md`

## Codex remote-sync bootstrap — 所有 Stage 共通

Codex 本機 repository 可能落後 GitHub。每次執行 `TASKS.md` 中的工作前，必須先安全同步 remote，再讀最新版規則與 queue：

1. 確認 repository identity。
2. 記錄 `git status --short`、目前 branch、HEAD。
3. `git fetch origin`。
4. 若目前為預期 branch（通常 `main`）、working tree clean，且 local 可由 `origin/main` fast-forward，才使用 fast-forward-only 同步。
5. 若 local 已與 `origin/main` 相同，直接繼續。
6. 若 dirty、unexpected branch、local ahead/diverged、無法 fast-forward，或 merge/rebase/cherry-pick 未完成，立即 STOP 並回報；不得自行修復。
7. 禁止 `reset --hard`、force push、自行 merge/rebase、stash/delete/丟棄未知 user work。
8. 同步完成後才讀最新 local `AGENTS.md` 與根目錄 `TASKS.md`。
9. 若同步後 `TASKS.md` 已不存在，或指定 Stage 已被移除，不得依舊 prompt、舊 SHA 或聊天記憶繼續執行該工作；STOP 並回報 queue 已變更。

## Codex patch / validation 共通規則

所有程式修改遵循：

`Evidence → Root Cause → Focused Patch → Targeted Validation`

Root cause 僅使用：
- `CONFIRMED ROOT CAUSE`
- `HIGH-CONFIDENCE LIKELY ROOT CAUSE`
- `INSUFFICIENT OBSERVABILITY`

只有前兩者可直接 patch；第三者先增加最小 observability 或 STOP，不做猜測式重構。

Validation 由小到大：static check → targeted verifier → targeted test → relevant build → required build matrix → full regression。只跑足以驗證本次 scope 的最低成本層級，不預設 full regression。

若涉及 build / CI evidence，盡量保存或回報可重現資訊：toolchain/version、board/FQBN、實際 command、tested commit SHA、CI run（若適用）。不得把沒有實體 evidence 的 Bench / Hardware / Vehicle 結果宣稱為 PASS。

成功驗證後：
- 同步更新 `TASKS.md`，刪除/更新本次完成的 task/stage，不留下 Completed 紀錄。
- 若 queue 尚有 unfinished work，保留 `TASKS.md`；若完全清空，刪除 `TASKS.md`。
- commit 並 push 本次授權的變更；若 push 或 remote state 出現異常，STOP 並如實回報，不以 force push 解決。

## Short-launch mode

完整執行 Prompt 保存在本檔案。正常流程是先由使用者與 ChatGPT 討論目前該跑哪個 Stage，以及建議模型 / 推理強度；使用者在 Codex UI 手動設定後，只需貼短啟動指令，例如：

```text
先安全同步最新 origin/main（僅允許 clean、fast-forward-only；異常即 STOP）。同步後讀根目錄 AGENTS.md 與 TASKS.md，完整執行 TASKS.md 的「<Stage 名稱>」，只執行該 Stage。成功驗證後依 TASKS.md 規則更新 queue、commit 並 push。
```

Codex 必須以同步後的最新 `TASKS.md` 為準；短啟動指令不授權執行其他 Stage。

---

## P0 — Phase 1 hardening prerequisite

- [ ] **確認並修正 Arduino build layout / reproducible ESP32 build**：目前 sketch 位於 `src/src.ino`，實作位於 `src/hal/`、`src/core/` 等子目錄。確認標準 Arduino IDE / Arduino CLI build 是否真的會編譯 TWAI backend 與相關 source；若不會，採最小且 library-ready 的 repository layout 修正，並建立可重現的 ESP32-S3 compile validation。此項完成前，不應把既有 ESP32 Compile PASS 視為完整 backend build evidence。

## P1 — Confirmed defects

- [ ] **Classic CAN RX DLC boundary**：`Esp32TwaiCan::receive()` 不得在 `data_length_code > 8` 時讀寫固定 8-byte buffer 範圍外。對 malformed / non-compliant Classic CAN frame 採明確 reject/drop policy，不得靜默把錯誤 frame 交給未來 ISO-TP。
- [ ] **CAN ID range validation**：`CanFrame::isValid()` 應驗證 Standard ID `<= 0x7FF`、Extended ID `<= 0x1FFFFFFF`；real HAL 與 MockCan contract 應一致。補 boundary+1 regression tests，避免 ESP-IDF/TWAI masking 將 invalid ID 轉成其他有效 ID。
- [ ] **禁止複製 `Esp32TwaiCan` ownership state**：delete copy/move constructor 與 assignment（或採同等明確 ownership 設計），避免多個 wrapper instance 同時聲稱擁有同一個 legacy TWAI driver。

## P2 — Phase 1 / Phase 2 boundary hardening

- [ ] **決定並實作 RTR policy**：目前 RX 會丟失 TWAI RTR semantics。Phase 2 ISO-TP 前，明確選擇保留 remote-frame metadata 或在 HAL 邊界 reject/drop RTR；不可讓 RTR 偽裝成一般 data frame。
- [ ] **修正 host test helper malformed-input safety**：`makeFrame()` 不得把超過 8 bytes 的 initializer list 複製進固定 payload；`sameFrame()` 不得在 invalid `length > 8` 時形成 out-of-range iterator。加入 malformed-frame regression coverage。
- [ ] **同步 MockCan lifecycle contract**：`stop()` / reinitialize 後不得保留與 real TWAI lifecycle 不一致的 stale RX frame；MockCan 對 invalid frame 的 send behavior 應與 real HAL contract 一致。
- [ ] **釐清 BUS_OFF stop status semantics**：ESP-IDF 在 BUS_OFF 可 uninstall，但 `twai_stop()` 只接受 RUNNING。若 cleanup 實際成功，不應用誤導性的 `NotInitialized` 表示整體 stop 失敗。只做最小 status 修正；不要提前建立 speculative recovery state machine。
- [ ] **建立最小 RX overflow observability**：目前 TWAI 使用 accept-all 且 default RX queue 很小，alerts 關閉。ISO-TP 前至少要能辨識 RX queue full / FIFO overrun / BUS_OFF 等關鍵狀態，並決定合理 queue sizing / drain strategy；不得因本項提前實作完整 FreeRTOS scheduler 或 Phase 2 concurrency architecture。
- [ ] **Phase 1 hardening 後重新取得 CI / compile evidence**：host tests 與 ESP32-S3 backend compile 都要在 hardening 後重新驗證，並以新的 commit/run 作為 Phase 1 PASS evidence；不要沿用早於 edge-case hardening 的舊 CI run。

## P2 — Project rules

- [ ] **更新 `AGENTS.md` 的 TASKS.md 永久規則**：加入「若 repository root 存在 `TASKS.md`，任務開始時優先讀取；成功完成並驗證授權工作後同步移除/更新對應項目；不得未經授權執行其他 TASKS；沒有未完成工作時刪除 `TASKS.md`」的永久規則，並補上 Codex remote-sync bootstrap 與 short-launch queue semantics，使本檔案即使日後刪除，正式協作規則仍由 `AGENTS.md` 保存。
- [ ] **補上 library-ready design 原則**：Generic protocol/data layers 應維持未來可抽成 reusable library 的依賴方向，但目前不得為 library 化提前拆 repository、加入 speculative abstraction、package/release infrastructure 或無實際 consumer 的 generic API。等 ISO-TP → OBD/UDS → Brand Layer → Vehicle Profile → VehicleData 實際資料流穩定後再評估抽離。

## Deferred

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；長時間 uptime 約 49.7 天 rollover 的問題留到真正需要 frame freshness / long-running VehicleData semantics 時處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace。等 library extraction 或第一個非 VAG consumer 成為實際工作時，再評估是否改為 brand-neutral namespace；現在不為命名進行大規模 churn。
- [ ] **ESP32 backend CI coverage** — Deferred until build layout is fixed：host CI 目前不編譯真正的 ESP32 TWAI backend。先完成 P0 build-layout/reproducible build，再決定是否加入 Arduino-ESP32 compile CI 或其他最小 backend compile gate。

---

# Codex 執行計畫 — 等流量恢復後逐階段執行

以下 Prompt 是為上述未完成項目預先保存的執行方案。**每次只能在使用者明確授權該 Stage 後執行該 Stage；不得因為讀到後續 Stage 就提前處理。** 每個 Stage 都繼承本檔案前面的 remote-sync bootstrap、patch/validation、queue lifecycle 與 short-launch 共通規則；Stage Prompt 不重複這些內容。

## Stage 1 — 專案永久規則補齊

**推薦模型：** Luna  
**推理強度：** Low  
**推薦理由：** 純文件規則同步，scope 小、無需高階程式推理。  
**是否值得先用較便宜模型做前置蒐證：** 否；Luna 已是最低合理成本，且所需規則已明確。  
**Context 建議：** Level 0→2；只需 `AGENTS.md`、`TASKS.md`，必要時讀 `docs/ARCHITECTURE.md` 驗證措辭不衝突。  
**Execution mode：** Focused docs-only patch。  
**Dependency / 觸發條件：** 無；應優先於其他 Stage 執行。  
**Escalation 條件：** 若現有正式規格與欲加入規則有實質衝突，STOP 並回報；不需要自行升級模型。

### Codex Prompt

```text
本次只執行 Stage 1：補齊專案永久規則。不要處理任何其他 TASKS.md 項目，也不要修改 source code、tests、workflow 或 build layout。

請在 AGENTS.md 加入/整合以下永久規則，避免未來 TASKS.md 被刪除後遺失協作流程：

A. TASKS.md shared queue rule
If TASKS.md exists at the repository root, read it near the beginning of the task. Treat it as the shared temporary queue of unfinished work. After successfully completing and validating an authorized task, remove or update the corresponding entry. Do not execute unrelated TASKS.md items without explicit user authorization. Delete TASKS.md when no unfinished work remains.

B. Remote-sync bootstrap
在執行 TASKS.md 工作前，先 fetch origin；只有預期 branch、clean working tree、可 fast-forward-only 時才同步。dirty / unexpected branch / local ahead or diverged / merge-rebase-cherry-pick in progress 時立即 STOP。禁止 reset --hard、force push、自行 merge/rebase、stash/delete/discard unknown work。同步後才讀最新 AGENTS.md / TASKS.md；若 TASKS.md 或指定 Stage 已不存在，不得依舊 prompt 執行。

C. Short-launch queue semantics
TASKS.md 可保存完整 scoped Codex Prompt；使用者可在 Codex UI 手動設定模型/推理強度後，以短指令指定單一 Stage。Codex 只能執行該 Stage，不能自行執行 queue 其他項目，也不能自行切換模型或推理強度。

D. Library-ready design
核心協議與資料模型維持未來可抽離 reusable library 的依賴方向，但目前禁止為函式庫化提前拆 repository、增加 speculative abstraction、package/release infrastructure 或沒有實際 consumer 的 generic API。

至少明確約束：
- core / transport abstraction / ISO-TP / OBD / UDS / ReadOnlyGuard / generic VehicleData / profile interfaces 不得依賴 Arduino、ESP32/TWAI、Web、BLE、Wi-Fi 或特定品牌/車型實作。
- ESP32/Arduino-specific code 留在 platform/HAL/firmware boundary。
- Brand-specific code 不得反向滲入 Generic Core。
- UI/network/storage clients 依賴 VehicleData/application-facing interface，不讓核心協議反向依賴 client。
- 避免不必要 global mutable state / platform singleton 等妨礙 host testing / dependency injection 的耦合。
- 等 ISO-TP → OBD/UDS → Brand Layer → Vehicle Profile → VehicleData 的實際資料流穩定後，再評估 library extraction。
- 不要因本規則現在建立 library.properties、package、另一個 repo、semantic versioning 或未使用的抽象層。

Validation：
- review diff，確認只改必要文件。
- 確認新規則不凌駕 Architecture Freeze、Read-only policy 或 evidence rules。

成功後同步更新 TASKS.md：移除「更新 AGENTS.md 的 TASKS.md 永久規則」與「補上 library-ready design 原則」兩個已完成項目，並移除整個 Stage 1 Prompt。其他 unfinished TASKS 不得更動，除非只為保持文字一致所必須。

回報使用繁體中文，列出：變更檔案、規則摘要、validation、commit SHA。
```

## Stage 2 — Arduino build layout 與可重現 ESP32 backend compile

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 問題已定位，但涉及 Arduino sketch recursive compilation、repository layout 與實際 backend 是否進入 build，需中等推理與 build evidence。  
**是否值得先用較便宜模型做前置蒐證：** 否；ChatGPT 已完成主要前置蒐證。先由 Luna 用 repository/本機 toolchain 證實，不重新做大範圍 web research。  
**Context 建議：** Level 0→3；先讀 root layout、`src/src.ino`、直接相關 source/header、現有 build/CI 文件。  
**Execution mode：** Evidence → Root Cause → minimal layout patch → reproducible compile。  
**Dependency / 觸發條件：** Stage 1 PASS。  
**Escalation 條件：** 若最小修正會迫使大規模 build-system 重設計、跨 framework 遷移或無法取得足夠 build evidence，STOP 並回報；不要自行升模型。

### Codex Prompt

```text
本次只執行 Stage 2：確認並修正 Arduino build layout，讓 ESP32-S3 compile evidence 能證明真正的 TWAI backend 與相關 source 有被編譯。不要同時修 CAN frame、RTR、BUS_OFF、MockCan 或 ISO-TP。

Evidence first：
1. 檢查 repository/sketch layout，特別是 `src/src.ino` 與 `src/hal/`、`src/core/`、`src/config/` 等。
2. 用標準 Arduino IDE / Arduino CLI 的實際 build semantics 或可重現 compile command，確認現況是否真的編譯 `esp32_twai_can.cpp`；不要只因空 sketch compile success 就判定 backend PASS。
3. 將 root cause 分類成 CONFIRMED ROOT CAUSE / HIGH-CONFIDENCE LIKELY ROOT CAUSE / INSUFFICIENT OBSERVABILITY。

若確認 layout 有問題：
- 採最小、library-ready 的 layout 修正。
- 不改 protocol behavior，不開始 ISO-TP，不做 namespace 大改。
- 優先讓 standard Arduino build 自然包含需要的 source，而不是用脆弱的臨時 copy/script 掩蓋 layout 問題。
- 保持 Board Profile → HardwareConfig → HAL 邊界。

Validation：
- 必須取得可重現的 ESP32-S3 compile evidence。
- evidence 必須能證明 `Esp32TwaiCan` backend 實際參與編譯，而非只有 `.ino`。
- 記錄 toolchain/version、board/FQBN、實際 compile command、tested commit SHA。
- 若 host tests 因 layout 變動受影響，也跑 targeted host build/test。

成功後同步更新 TASKS.md：移除 P0 build-layout task並移除整個 Stage 2 Prompt；若 ESP32 backend CI coverage 已因此變得可直接實作，可將 Deferred 項目改為明確下一步，但本 Stage 不要自行新增大型 CI 系統。

回報繁體中文：Evidence、Root Cause、Focused Patch、Validation、變更檔案、commit SHA。
```

## Stage 3 — Portable CAN contract 與 host-test safety

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 都是已確認的 deterministic boundary/test contract 問題，可在 portable layer 與 host tests 內完成，不需要 Terra。  
**是否值得先用較便宜模型做前置蒐證：** 否；root causes 已知。  
**Context 建議：** Level 0→3；`can_types.h`、`can_hal.h`、`mock_can.h`、`test_helpers.h`、`tests/host/main.cpp` 與 direct caller。  
**Execution mode：** Focused portable patch + targeted host tests。  
**Dependency / 觸發條件：** Stage 2 PASS。  
**Escalation 條件：** 若修正需要改變公開 CAN API 的核心語意或牽涉多個未預期 consumer，STOP 回報 evidence，不自行擴 scope/模型。

### Codex Prompt

```text
本次只執行 Stage 3：修正 portable CAN contract 與 host-test safety。不要修改 ESP32 TWAI runtime semantics、BUS_OFF recovery、RX alert architecture 或開始 ISO-TP。

依 progressive expansion 只讀 CanFrame model、CanHal contract、MockCan、test helpers、host tests 與必要 direct caller。

本 Stage 授權處理：
1. CAN ID range validation：Standard <= 0x7FF、Extended <= 0x1FFFFFFF、length <= 8，補 boundary + boundary+1 tests。
2. MockCan contract 與 real HAL 對 invalid frame 的基本驗證語意一致。
3. `makeFrame()` 對 >8 bytes input 不得 OOB。
4. `sameFrame()` 面對 invalid length 不得形成 out-of-range iterator。
5. MockCan stop / reinitialize 不得保留與 real TWAI lifecycle 不一致的 stale RX frame。

設計要求：
- 不要為 invalid frame 新增過度複雜 error hierarchy；若現有 `CanStatus` 缺乏完美名稱，採最小一致方案並在回報說明 semantic debt。
- 不把 VAG / Kamiq constants 放進 Generic Core。
- 不做與本 Stage 無關的 refactor。

Validation：
- host tests 必須涵蓋 valid min/max、invalid standard 0x800、invalid extended 0x20000000、length 0/8/9、malformed helper input、MockCan stop→initialize stale RX lifecycle。
- 使用現有 host CI/build style；若 toolchain 可用，實際執行 targeted tests。

成功後同步更新 TASKS.md：移除 CAN ID range、host helper malformed safety、MockCan lifecycle 三個完成項目，並移除整個 Stage 3 Prompt。不要移除尚未由本 Stage 處理的 TWAI/RTR/BUS_OFF/overflow tasks。

回報繁體中文：Evidence、Root Cause、Focused Patch、Targeted Validation、commit SHA。
```

## Stage 4 — ESP32 TWAI runtime hardening

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 主要 root causes 已有 evidence，預期只需最小 HAL hardening；先用最低成本模型。  
**是否值得先用較便宜模型做前置蒐證：** 否；不要重新做 repository-wide 或 upstream discovery。只有 local evidence 不足時才精準讀對應 Arduino-ESP32 / ESP-IDF 版本 source。  
**Context 建議：** Level 0→3；`esp32_twai_can.*`、CanFrame/CanStatus、HardwareConfig、直接 tests/build evidence；必要時精準讀對應 ESP-IDF TWAI source。  
**Execution mode：** Focused HAL patch；禁止提前進 Phase 2 concurrency architecture。  
**Dependency / 觸發條件：** Stage 2 + Stage 3 PASS。  
**Escalation 條件：** 若 BUS_OFF / RX overflow 無法以最小 HAL patch 完成，而實際需要 runtime state machine、FreeRTOS concurrency/backpressure 或 ISO-TP-aware scheduling，STOP 並保存 evidence；建議下一輪考慮 Stage 4B，但由使用者決定模型與強度。

### Codex Prompt

```text
本次只執行 Stage 4：ESP32 TWAI runtime hardening。不要開始 ISO-TP / OBD / UDS / Scheduler。

本 Stage 授權處理：
1. Classic CAN RX DLC > 8：在 repository boundary 安全 reject/drop malformed/non-compliant frame，任何 copy 前先確保不會超過 8-byte buffer；不得靜默 clamp 後交給未來 ISO-TP 當正常 frame。
2. `Esp32TwaiCan` ownership：禁止不安全 copy/move，避免多 wrapper 同時聲稱擁有 legacy global TWAI driver。
3. RTR policy：對 v1 ISO-TP path 明確 reject/drop RTR 或以最小方式保留 metadata；不可把 RTR 偽裝成普通 data frame。優先採最小且不污染 upper layer 的方案。
4. BUS_OFF stop status semantics：ESP-IDF BUS_OFF 可 uninstall、`twai_stop()` 只接受 RUNNING。若 cleanup 實際成功，避免回傳誤導性 failure。只修 status/cleanup semantics，不建立 speculative recovery state machine。
5. 最小 RX overflow observability：至少能辨識或取得 RX queue full / FIFO overrun / BUS_OFF 關鍵 evidence，並決定合理的 queue sizing / drain baseline。不要因此建立完整 FreeRTOS scheduler、background diagnostic task 或 ISO-TP concurrency architecture。

若第 4 或第 5 項無法用最小 patch 完成，而必須設計跨模組 recovery state machine、FreeRTOS concurrency/backpressure、ISO-TP-aware scheduling，立即 STOP，不自行擴張 scope，也不自行切換模型。將 root cause、需要的 state transitions、涉及模組與最小待決策問題寫入回報，供使用者決定是否執行 Stage 4B。

Validation：
- ESP32-S3 backend compile，記錄可重現 build evidence。
- 能 host-test 的 boundary 盡量補 deterministic tests；硬體專屬行為無實機 evidence 標 Pending，不偽稱 Hardware PASS。
- 確認沒有引入 active diagnostic behavior 或 read-only policy violation。

成功後同步更新 TASKS.md：移除 DLC boundary、Esp32TwaiCan ownership、RTR policy、BUS_OFF semantics、RX overflow observability 中已真正完成並驗證的項目，並移除整個 Stage 4 Prompt；若某項僅部分完成，改寫 blocker/remaining work，不可假裝完成。若 Stage 4 全部完成且不需 fallback，一併移除 Stage 4B Prompt。

回報繁體中文：Evidence、Root Cause 分類、Focused Patch、Validation、Pending hardware evidence、commit SHA。
```

## Stage 4B — 條件式 fallback（只有 Stage 4 明確觸發才考慮）

**推薦模型：** Terra  
**推理強度：** Medium  
**推薦理由：** 只有 Stage 4 evidence 證明最小 HAL patch 不足、問題實際牽涉 runtime state machine / concurrency 時，才值得考慮較高階模型。  
**是否值得先用較便宜模型做前置蒐證：** 是；Stage 4 Luna 的 evidence package 就是前置蒐證，禁止重新從 repository-wide discovery 開始。  
**Context 建議：** 只載入 Stage 4 blocker/evidence、直接 HAL/state modules、必要 ESP-IDF semantics。  
**Execution mode：** Evidence-constrained design/patch。  
**Dependency / 觸發條件：** Stage 4 必須已明確 STOP 並留下需要更高階 runtime reasoning 的 evidence；是否真的使用 Terra / Medium 由使用者決定。  
**Escalation 條件：** 若 Terra 分析證明問題已進入非平凡跨模組 concurrency + protocol/lifecycle safety 或高風險架構設計，STOP；可建議使用者考慮 Sol / Medium～High，但不得自行切換。

### Codex Prompt

```text
只有在 Stage 4 已明確留下「最小 HAL patch 不足」的具體 evidence / blocker 時才執行本 Stage。若沒有，STOP。

只處理 Stage 4 未完成且被證明需要較高階推理的 runtime 問題，例如：
- BUS_OFF / RUNNING / STOPPED cleanup/recovery ownership semantics；或
- RX overflow / queue / drain 需要的最小 concurrency boundary。

目標仍是「Phase 1 hardening 的最小安全設計」，不是提前做 Phase 2 ISO-TP scheduler。

要求：
- 明確列 state/invariant。
- 保持 CanHal / Clock / Generic Core 邊界。
- 不讓 TWAI/FreeRTOS types 洩漏到 diagnostic/application layer。
- 若真正需要非平凡跨模組 FreeRTOS task architecture、protocol scheduling 或高風險 API 改造，STOP 並回報；不要自行切換 Sol。

完成後做 targeted validation，並只更新 TASKS.md 中本 Stage 實際完成的對應項目；若全部完成，移除整個 Stage 4B Prompt。

回報繁體中文：Evidence、state/invariant、Root Cause、patch、validation、remaining risks、commit SHA。
```

## Stage 5 — Phase 1 hardening 最終驗證、CI 與狀態同步

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 主要工作是驗證、CI/build evidence、文件狀態同步與 TASKS cleanup；需要跨結果核對但不需高階架構推理。  
**是否值得先用較便宜模型做前置蒐證：** 否；前面 Stage 已提供 evidence。  
**Context 建議：** Level 0→3；TASKS、AGENTS、host tests/workflow、build layout、DEVELOPMENT/README 中 Phase 1 status。  
**Execution mode：** Validation-first consolidation；禁止新增功能。  
**Dependency / 觸發條件：** Stage 1～4 PASS；若 Stage 4B 曾被觸發，則 Stage 4B 也必須 PASS 或其剩餘 blocker 已被明確 Deferred。  
**Escalation 條件：** 若 CI/build 問題演變為大型 toolchain migration、複雜 workflow architecture 或與 product logic 無關的 infra 問題，STOP 並保留 Blocked/Deferred，不自行升模型。

### Codex Prompt

```text
本次只執行 Stage 5：Phase 1 hardening 最終驗證與狀態同步。不要新增 ISO-TP、OBD、UDS、VehicleData 或任何 Phase 2+ 功能。

工作：
1. 跑完整 relevant host tests。
2. 跑可重現 ESP32-S3 compile，確認真正包含 ESP32 TWAI backend。
3. 檢查 GitHub Actions / compile gate 是否足以作為新的 Phase 1 evidence。
4. 若 build layout 已固定且加入最小 ESP32 backend compile CI 明顯低風險、符合現有 workflow，可在本 Stage 實作；若會引入大型 toolchain/cache/CI complexity，保留 Deferred 並寫明原因。
5. Review Phase 1 foundation docs/README/DEVELOPMENT status，只修正與實際 validation evidence 不一致的文字。Hardware/Bench/Vehicle 沒有實體 evidence 必須維持 Pending。
6. 確認沒有 brand leakage、read-only violation、TWAI type 上滲或 speculative Phase 2 implementation。

Build / CI evidence 至少回報：toolchain/version、board/FQBN、實際 command、tested commit SHA、GitHub Actions run/status（若適用）。

TASKS.md cleanup：
- 成功驗證後移除「Phase 1 hardening 後重新取得 CI / compile evidence」。
- 若 ESP32 backend CI coverage 已完成，移除該 Deferred 項目；若未做，保留並清楚寫 blocker/trigger。
- 其他已由前面 Stage 完成但仍殘留的 task，只有在 current main + validation 可證實完成時才移除。
- Deferred timestamp / namespace 若觸發條件尚未發生，保留。
- 完成本 Stage 後移除整個 Stage 5 Prompt。
- 若最後仍有任何 unfinished/deferred task，TASKS.md 必須保留；只有真的完全沒有 unfinished work 時才刪除 TASKS.md。

最終回報繁體中文：
- current HEAD / commit
- Host Test PASS/FAIL
- ESP32 Compile PASS/FAIL
- CI evidence
- Bench / Hardware / Vehicle = Pending 或實際 evidence
- Remaining TASKS
- 是否可以進 Phase 2 ISO-TP
```
