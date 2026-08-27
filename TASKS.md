# TASKS

本檔案是本 repository 唯一的 active unfinished-work / executable scoped Prompt queue，只保留 TODO、Blocked、Deferred 與 Pending-validation 工作。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---

## P2 — Phase 1 / Phase 2 boundary hardening

- [ ] **Phase 1 hardening evidence consolidation**：Stage 4R 後 local host 與 ESP32-S3 backend compile 已取得 current evidence，current main 也已有 Host CI PASS；Stage 5 依 material-change / evidence-reuse 規則完成 Phase 1 current evidence consolidation、必要缺口驗證、文件狀態同步與 final gate。不得因進入 Stage 5 本身重跑仍為 CURRENT 的 evidence。

## Deferred

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；等真正需要 frame freshness / long-running VehicleData semantics 時再處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。
- [ ] **ESP32 backend CI coverage** — Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。

---

## Stage 5 — Phase 1 hardening 最終 evidence consolidation / state sync

**推薦模型：** Luna  
**推理強度：** Medium  
**推薦理由：** 主要工作是 canonical evidence inventory、evidence validity 判讀、文件狀態同步與 queue cleanup；需要跨 evidence 核對，但不需要新增 runtime / protocol implementation。  
**是否值得先用較便宜模型做前置蒐證：** 否；Stage 4R 已提供主要 evidence，Stage 5 本身先做 reuse-first inventory。  
**Context 建議：** Level 0→3；`AGENTS.md`、`TASKS.md`、`VALIDATION.md`、`CODEX_PROGRESS.md`、`docs/DEVELOPMENT.md`、relevant host workflow / current GitHub Actions evidence；只有在判斷 evidence validity 時才讀 build layout / backend-participation evidence，README 僅在需要核對 Phase 1 status wording 時讀。  
**Execution mode：** Validation-first consolidation；禁止新增功能。  
**Agent：** 1  
**Dependency / 觸發條件：** Stage 1 + Stage 1B + Stage 2～4 + Stage 4R PASS；目前 Stage 4R dependency 已滿足。Stage 5 可在使用者明確授權後執行，但這不代表 Stage 5 已完成或 Phase 2 已獲准。  
**Escalation 條件：** 若 evidence inventory 顯示 material source / toolchain / validation-backend change、current evidence 無法證明 Phase 1 contract，或問題演變成大型 toolchain / workflow architecture / product-logic修改，STOP 並保留 evidence；不得自行升模型、擴大 Context、增加 Agent 或開始 Phase 2。

### Codex Prompt

```text
本次只執行 Stage 5：Phase 1 hardening 最終 evidence consolidation / state sync。

不要新增 ISO-TP、OBD、UDS、VehicleData 或任何 Phase 2+ 功能。
不要修改 production source / tests。
不要新增或修改 ESP32 backend CI workflow；「ESP32 backend CI coverage」保持 Deferred，除非未來另有明確授權 Stage。

依最新 AGENTS.md 完成 repository identity、workspace/permission、safe remote-sync 等 execution gates；同步後重讀最新 AGENTS.md / TASKS.md，再依 routing 只讀本 Stage 的最低必要 Playbook 章節。

工作：
1. 建立 canonical evidence inventory：確認 current HEAD、current source 與 Stage 4R tested commit `2e4ec1dbeb5bb050014aa610f4c1dc50fff857f6` 的關係、current GitHub Host CI、local host evidence、ESP32 compile / backend-participation evidence，以及 current TASKS / VALIDATION / project-state docs。
2. 對各 evidence 分類為 `CURRENT`、`SUPERSEDED`、`HISTORICAL`、`REVALIDATION_REQUIRED` 或 `Pending`。只有 material change 才使相關 evidence 失效；進入 Stage 5 本身不是 revalidation trigger。
3. Reuse first：若 Stage 4R 後 relevant source、Arduino-ESP32 / toolchain / FQBN、build layout / backend participation contract 與 validation backend均未 material change，直接 reuse Stage 4R 的 local host與 ESP32 compile evidence，以及 current main Host CI；不得為形式重跑相同 validation。
4. 只有在 evidence 缺失、material change、formal repository gate要求，或 canonical evidence 無法證明 current contract 時，才執行最低充分的 targeted revalidation。若需要昂貴 / long-running operation，先做 deterministic preflight並依 Playbook做 bounded supervision與 phase attribution。
5. 依 canonical evidence最小同步 project state：優先檢查 `VALIDATION.md`、`CODEX_PROGRESS.md`、`docs/DEVELOPMENT.md`；README 只有真的存在 stale Phase 1 status wording時才修改。已知 `CODEX_PROGRESS.md` 仍有「Stage 4R runtime correctness follow-up remains Pending」的 stale wording，只有 canonical evidence仍確認它過時時才修正。
6. 確認 Generic Core / read-only / HAL boundary 沒有被既有 Stage 4R變更破壞；這一步是 bounded state review，不授權 repo-wide exploration或 speculative refactor。
7. Bench / Hardware / Vehicle沒有實體 evidence時全部保持 Pending，不得由 host / compile / CI evidence升格。
8. 避免 CI freshness loop：不要為了讓文件永遠記錄最後一個純 docs push所觸發的新 Host CI run，而形成 docs update → push → CI → docs update循環。final post-push CI freshness可在 Completion Evidence Guard / final report以 canonical GitHub evidence確認；source與validation contract未變時，不因純 bookkeeping run再製造另一個 docs commit。

Stage 5 success gate：
- Phase 1 software / hardening evidence closure可由 current canonical evidence支持。
- required state docs與 `VALIDATION.md` 不再有會誤導下一 Stage的 material stale claim。
- Bench / Hardware / Vehicle仍正確維持 Pending。
- Completion Evidence Guard已核對 baseline/final/origin SHA、scoped diff、current validation evidence與 final TASKS state。

只有 success gate成立後，才能回報「Phase 1 software / hardening closure成立」並判斷 Phase 2 ISO-TP gate是否可開放；不得只因 Prompt執行完就自動宣稱 Phase 2可開始，也不得在本 Stage直接開始 Phase 2 implementation。

TASKS.md cleanup：
- Stage 5成功後移除「Phase 1 hardening evidence consolidation」。
- 移除整個 Stage 5 Prompt。
- 保留尚未觸發的 Deferred：64-bit monotonic timestamp、Generic namespace命名、ESP32 backend CI coverage。
- 不自動新增 Phase 2 implementation task；先在 final report回報 Phase 2 gate判定，等待使用者 / ChatGPT後續規劃。
- 若 Stage 5 Blocked或 evidence不足，不得移除 P2 / Stage 5；改為保留 unfinished並記錄最小 blocker / trigger。

最終回報使用繁體中文，至少包含：
- baseline / final HEAD、origin/main
- evidence inventory與各 evidence validity分類
- Host / ESP32 compile / current CI evidence（reused或revalidated）
- 本 Stage是否實際重跑任何 validation，以及原因
- changed files / scoped diff
- Bench / Hardware / Vehicle = Pending或實際 evidence
- Remaining TASKS
- Phase 1 closure判定
- Phase 2 ISO-TP gate判定
- Completion Evidence Guard結果
- push / working tree state

最後一行：
回報時間：YYYY-MM-DD HH:mm (Asia/Taipei)
```
