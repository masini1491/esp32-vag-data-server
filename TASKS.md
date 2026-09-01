# TASKS

本檔案是本 repository 唯一的 active unfinished-work / executable scoped Prompt queue，只保留 TODO、Blocked、Deferred 與 Pending-validation 工作。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
---
## Deferred

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；等真正需要 frame freshness / long-running VehicleData semantics 時再處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。
- [ ] **ESP32 backend CI coverage** — Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。

---
