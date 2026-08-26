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

### 一次性小型 maintenance 不必進 TASKS.md

`TASKS.md` 是「未完成、需要追蹤、需要未來再次執行」的 queue，不是所有微小修改都必須先建立 task。

若工作同時具備以下特徵，可不寫入 `TASKS.md`，由 ChatGPT 直接產生一次性的短 Codex Prompt：
- 一次性工作。
- 已知修改位置。
- 已知修改內容或 root cause 已確認。
- scope 極小、風險低。
- 完成後沒有後續追蹤價值，也不需要保留為 unfinished queue。
- 不影響 project behavior、architecture、protocol、security、hardware、persistence、runtime state 或重要 validation state。

典型例子：
- 單行或少量文件 wording 修正。
- 已知位置的 typo / 過期術語修正。
- 單純排序、格式、標題或註解整理。
- 不影響 project behavior / architecture / protocol / security / hardware / validation state 的小型 documentation maintenance。
- 已知 root cause、scope 極小、完成後不需要保留為 unfinished queue 的 maintenance。

這類一次性 maintenance 通常建議：
- **推薦模型：** Luna
- **推理強度：** Low
- **Context：** Level 0→1
- **Execution mode：** Focused patch
- **Agent：** 1

即使不進 `TASKS.md`，Codex 仍必須遵守最新 `AGENTS.md` 的安全 remote-sync、scope、Git、validation 與 commit/push 規則；一次性短 Prompt 不得繞過正式 source of truth 或 Git safety。

若符合以下任一情況，則應寫入 `TASKS.md`：
- 需要後續追蹤。
- 目前為 Blocked / Deferred / Pending-validation。
- 工作分成多個 Stage。
- 有 dependency / trigger。
- root cause 尚未完全確認。
- 可能需要之後接續 implementation。
- 會實質影響 project behavior、architecture、protocol、security、hardware、persistence、runtime state 或重要 validation state。
- 若現在不記錄，之後容易遺漏。

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

## Progressive Repository Reading — 所有 Stage 共通

Repository 讀取採逐級擴張，目標是用完成目前任務所需的最小 Context 取得足夠 evidence：

- **Level 0**：最新 `TASKS.md`、當前 diff / error / log / build evidence、Git preflight 與當次使用者授權。
- **Level 1**：直接相關 symbol / target / test。
- **Level 2**：直接 caller / callee、owner、直接 dependency。
- **Level 3**：完整 relevant file。
- **Level 4**：直接相關 module / directory。
- **Level 5**：repository-wide search / read。

執行要求：
- 預設從 Level 0 → 1 → 2；若在較低 Level 已有足夠 evidence，就停止擴張。
- 只有 evidence 不足，且能指出「目前缺哪一個具體答案／關聯」時，才逐級擴張到下一 Level。
- 不得為了熟悉 repository、建立一般背景知識或「可能有關」而直接做 repo-wide scan。
- repository size、檔案數量或可用 Context 很大，本身都不是擴張讀取範圍或提高 Context 的理由。
- Stage 自己的 `Context 建議` 若更窄，優先遵守更窄 scope；若確需超過建議 Level，必須由 evidence 驅動並在回報說明理由。

## Codex remote-sync bootstrap — 所有 Stage 共通

Codex 本機 repository 可能落後 GitHub。每次執行 `TASKS.md` 中的工作前，必須先安全同步 remote，再讀最新版規則與 queue：

1. 確認 repository identity。
2. 記錄 `git status --short`、目前 branch、HEAD。
3. `git fetch origin`。
4. 若必要 Git / build / test / toolchain / filesystem 操作遭 sandbox / filesystem / execution permission denial，遵守 `AGENTS.md` 的 `Permission-Gated Operation`；可向使用者要求完成目前 Stage 所需的最小權限，不得自行繞過。
5. 若目前為預期 branch（通常 `main`）、working tree clean，且 local 可由 `origin/main` fast-forward，才使用 fast-forward-only 同步。
6. 若 local 已與 `origin/main` 相同，直接繼續。
7. 若 dirty、unexpected branch、local ahead/diverged、無法 fast-forward，或 merge/rebase/cherry-pick 未完成，立即 STOP 並回報；不得自行修復。
8. 禁止 `reset --hard`、force push、自行 merge/rebase、stash/delete/丟棄未知 user work。
9. 同步完成後才讀最新 local `AGENTS.md` 與根目錄 `TASKS.md`。
10. 若同步後 `TASKS.md` 已不存在，或指定 Stage 已被移除，不得依舊 prompt、舊 SHA 或聊天記憶繼續執行該工作；STOP 並回報 queue 已變更。

## Operational failure taxonomy / retry cap — 所有 Stage 共通

Operational failure 分類固定使用：
- `SOURCE`
- `TOOLCHAIN`
- `ENVIRONMENT`
- `INFRASTRUCTURE`
- `SERVICE`
- `AUTHENTICATION`
- `AUTHORIZATION`
- `HARDWARE_REQUIRED`

規則：
- 本分類用於 operational / execution failure，不取代既有 Root Cause 三分類 `CONFIRMED ROOT CAUSE` / `HIGH-CONFIDENCE LIKELY ROOT CAUSE` / `INSUFFICIENT OBSERVABILITY`。
- 同一 root cause 的**非 compile operational retry 最多 1 次**；第二次仍失敗就 STOP，分類 failure，保存最小可重現 evidence 並回報，不進入無限重試或換模型迴圈。
- Permission gate resolution 不計 operational retry；只有取得必要權限後操作本身仍失敗，才依本節 taxonomy / retry cap 處理。詳細 permission request、approval 與禁止 workaround 規則以 `AGENTS.md` 為準。
- 只有 `SOURCE` failure 可直接支持繼續修改 source；`TOOLCHAIN` / `ENVIRONMENT` / `INFRASTRUCTURE` / `SERVICE` / `HARDWARE_REQUIRED` 必須先處理或等待對應外部條件，不得把非 source failure 猜成 source bug 後繼續 patch。
- `TOOLCHAIN` / `ENVIRONMENT` / `INFRASTRUCTURE` / `SERVICE` / `HARDWARE_REQUIRED` 本身都不是 Luna → Terra → Sol、Low → Medium → High、Multi-Agent 或更大 Context 的升級理由。
- 若 `AGENTS.md`、正式 validation 規則或特定 Stage 已有 compile-fix retry 上限，原規則完整保留；本節的 non-compile operational retry cap 不覆蓋、不放寬也不取代 compile-fix retry 規則。

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
先安全同步最新 origin/main（僅允許 clean、fast-forward-only；異常即 STOP）。若執行所需的 Git／filesystem／sandbox 權限不足，先向我明確要求最小必要權限／approval，說明 command、resource 與原因；未經授權不得自行繞過。同步後讀根目錄 AGENTS.md 與 TASKS.md，完整執行 TASKS.md 的「<Stage 名稱>」，只執行該 Stage。成功驗證後依 TASKS.md 規則更新 queue、commit 並 push。
```

Codex 必須以同步後的最新 `TASKS.md` 為準；短啟動指令不授權執行其他 Stage。ChatGPT 未來產生一次性短 Codex Prompt 或 Stage launcher 時，也應包含等價的 permission-escalation 句子，避免把可授權解決的 permission denial 直接當成最終 blocker。

---

## P1 — Confirmed defects

- [ ] **Classic CAN RX DLC boundary**：`Esp32TwaiCan::receive()` 不得在 `data_length_code > 8` 時讀寫固定 8-byte buffer 範圍外。對 malformed / non-compliant Classic CAN frame 採明確 reject/drop policy，不得靜默把錯誤 frame 交給未來 ISO-TP。
- [ ] **禁止複製 `Esp32TwaiCan` ownership state**：delete copy/move constructor 與 assignment（或採同等明確 ownership 設計），避免多個 wrapper instance 同時聲稱擁有同一個 legacy TWAI driver。

## P2 — Phase 1 / Phase 2 boundary hardening

- [ ] **決定並實作 RTR policy**：目前 RX 會丟失 TWAI RTR semantics。Phase 2 ISO-TP 前，明確選擇保留 remote-frame metadata 或在 HAL 邊界 reject/drop RTR；不可讓 RTR 偽裝成一般 data frame。
- [ ] **釐清 TWAI stop / cleanup / reinitialize lifecycle semantics**：ESP-IDF 在 BUS_OFF 可 uninstall，但 `twai_stop()` 只接受 RUNNING。若 cleanup 實際成功，不應用誤導性的 `NotInitialized` 表示整體 stop 失敗；若 uninstall / cleanup 真正失敗，wrapper 不得無條件把 `installed_` / `started_` 宣稱清空，reinitialize 也不得忽略 cleanup failure 後繼續 install。只做最小 lifecycle / status invariant 修正；不要提前建立 speculative recovery state machine。
- [ ] **建立最小 RX overflow observability**：目前 TWAI 使用 accept-all 且 default RX queue 很小，alerts 關閉。ISO-TP 前至少要能辨識 RX queue full / FIFO overrun / BUS_OFF 等關鍵狀態，並決定合理 queue sizing / drain strategy；不得因本項提前實作完整 FreeRTOS scheduler 或 Phase 2 concurrency architecture。
- [ ] **釐清 CAN TX acceptance / completion / backpressure contract**：目前 `twai_transmit(..., 0)` 的 `ESP_OK` 只代表 frame 已 accepted / queued 或開始 transmission，不等同 on-wire success。Phase 2 ISO-TP 前必須明確定義 `CanHal::send() == Ok` 的語意，並讓 TX queue full、post-enqueue TX failure、BUS_OFF 等至少具有足夠且不誤導 upper layer 的 status / observability contract。不要因此提前建立完整 TX callback framework、FreeRTOS TX task、async completion system 或 ISO-TP scheduler。
- [ ] **Phase 1 hardening 後重新取得 CI / compile evidence**：host tests 與 ESP32-S3 backend compile 都要在 hardening 後重新驗證，並以新的 commit/run 作為 Phase 1 PASS evidence；不要沿用早於 edge-case hardening 的舊 CI run。

## Deferred

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；長時間 uptime 約 49.7 天 rollover 的問題留到真正需要 frame freshness / long-running VehicleData semantics 時處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace。等 library extraction 或第一個非 VAG consumer 成為實際工作時，再評估是否改為 brand-neutral namespace；現在不為命名進行大規模 churn。
- [ ] **ESP32 backend CI coverage** — Deferred：host CI 目前不編譯真正的 ESP32 TWAI backend；後續再決定是否加入 Arduino-ESP32 compile CI 或其他最小 backend compile gate。

---

# Codex 執行計畫 — 等流量恢復後逐階段執行

以下 Prompt 是為上述未完成項目預先保存的執行方案。**每次只能在使用者明確授權該 Stage 後執行該 Stage；不得因為讀到後續 Stage 就提前處理。** 每個 Stage 都繼承本檔案前面的 remote-sync bootstrap、patch/validation、queue lifecycle 與 short-launch 共通規則；Stage Prompt 不重複這些內容。

## Stage 4 — ESP32 TWAI runtime hardening

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 主要 root causes 已有 evidence，預期只需最小 HAL hardening；先用最低成本模型。  
**是否值得先用較便宜模型做前置蒐證：** 否；不要重新做 repository-wide 或 upstream discovery。只有 local evidence 不足時才精準讀對應 Arduino-ESP32 / ESP-IDF 版本 source。  
**Context 建議：** Level 0→3；`esp32_twai_can.*`、CanFrame/CanStatus、HardwareConfig、直接 tests/build evidence；必要時精準讀對應 ESP-IDF TWAI source。  
**Execution mode：** Focused HAL patch；禁止提前進 Phase 2 concurrency architecture。  
**Dependency / 觸發條件：** Stage 2 + Stage 3 PASS。  
**Escalation條件：** 若 TWAI lifecycle、RX overflow 或 TX completion/backpressure 無法以最小 HAL/contract patch 完成，而實際需要 runtime state machine、FreeRTOS concurrency/backpressure、async TX completion 或 ISO-TP-aware scheduling，STOP 並保存 evidence；建議下一輪考慮 Stage 4B，但由使用者決定模型與強度。

### Codex Prompt

```text
本次只執行 Stage 4：ESP32 TWAI runtime hardening。不要開始 ISO-TP / OBD / UDS / Scheduler。

本 Stage 授權處理：
1. Classic CAN RX DLC > 8：在 repository boundary 安全 reject/drop malformed/non-compliant frame，任何 copy 前先確保不會超過 8-byte buffer；不得靜默 clamp 後交給未來 ISO-TP 當正常 frame。
2. `Esp32TwaiCan` ownership：禁止不安全 copy/move，避免多 wrapper 同時聲稱擁有 legacy global TWAI driver。
3. RTR policy：對 v1 ISO-TP path 明確 reject/drop RTR 或以最小方式保留 metadata；不可把 RTR 偽裝成普通 data frame。優先採最小且不污染 upper layer 的方案。
4. TWAI stop / cleanup / reinitialize lifecycle semantics：ESP-IDF BUS_OFF 可 uninstall、`twai_stop()` 只接受 RUNNING。若 cleanup 實際成功，避免回傳誤導性 failure；若 uninstall / cleanup 真正失敗，不得無條件把 wrapper ownership state 清空，也不得在 reinitialize 時忽略 cleanup failure 後繼續 install。只修最小 lifecycle / status invariant，不建立 speculative recovery state machine。
5. 最小 RX overflow observability：至少能辨識或取得 RX queue full / FIFO overrun / BUS_OFF 關鍵 evidence，並決定合理的 queue sizing / drain baseline。不要因此建立完整 FreeRTOS scheduler、background diagnostic task 或 ISO-TP concurrency architecture。
6. CAN TX acceptance / completion / backpressure contract：明確定義 `CanHal::send() == Ok` 只代表 driver accepted / queued 或 initiated，除非有額外 evidence 不得宣稱 on-wire success；對 TX queue full、post-enqueue TX failure、BUS_OFF 至少提供足夠且不誤導 upper layer 的 status / observability contract。優先最小 contract / HAL patch，不建立完整 callback framework、TX worker task 或 ISO-TP scheduler。

若第 4、5 或 6 項無法用最小 patch 完成，而必須設計跨模組 recovery state machine、FreeRTOS concurrency/backpressure、async TX completion 或 ISO-TP-aware scheduling，立即 STOP，不自行擴張 scope，也不自行切換模型。將 root cause、需要的 state transitions / completion semantics、涉及模組與最小待決策問題寫入回報，供使用者決定是否執行 Stage 4B。

Validation：
- ESP32-S3 backend compile，記錄可重現 build evidence。
- 能 host-test 的 boundary 盡量補 deterministic tests；硬體專屬行為無實機 evidence 標 Pending，不偽稱 Hardware PASS。
- 確認沒有引入 active diagnostic behavior 或 read-only policy violation。

成功後同步更新 TASKS.md：移除 DLC boundary、Esp32TwaiCan ownership、RTR policy、TWAI lifecycle semantics、RX overflow observability、CAN TX acceptance/completion/backpressure contract 中已真正完成並驗證的項目，並移除整個 Stage 4 Prompt；若某項僅部分完成，改寫 blocker/remaining work，不可假裝完成。若 Stage 4 全部完成且不需 fallback，一併移除 Stage 4B Prompt。

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
**Escalation條件：** 若 Terra 分析證明問題已進入非平凡跨模組 concurrency + protocol/lifecycle safety 或高風險架構設計，STOP；可建議使用者考慮 Sol / Medium～High，但不得自行切換。

### Codex Prompt

```text
只有在 Stage 4 已明確留下「最小 HAL patch 不足」的具體 evidence / blocker 時才執行本 Stage。若沒有，STOP。

只處理 Stage 4 未完成且被證明需要較高階推理的 runtime 問題，例如：
- BUS_OFF / RUNNING / STOPPED cleanup/recovery ownership semantics；
- RX overflow / queue / drain 需要的最小 concurrency boundary；或
- TX acceptance / completion / backpressure 需要的最小 async completion 或 concurrency boundary。

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
**Dependency / 觸發條件：** Stage 1 + Stage 1B + Stage 2～4 PASS；若 Stage 4B 曾被觸發，則 Stage 4B 也必須 PASS 或其剩餘 blocker 已被明確 Deferred。  
**Escalation條件：** 若 CI/build 問題演變為大型 toolchain migration、複雜 workflow architecture 或與 product logic 無關的 infra 問題，STOP 並保留 Blocked/Deferred，不自行升模型。

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
