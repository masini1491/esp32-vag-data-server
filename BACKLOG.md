# BACKLOG

本檔案是本 repository 的 Cold Registry，只保存 future／dormant／trigger-based durable memory。Cold item 沒有 execution authority，不可直接執行或用於 TASKS Short-launch；trigger 成立或使用者選中後，必須先重讀 current authority/evidence、reconcile premise，再 promote 到 `TASKS.md`。

## Cold / future-trigger

- **Telltale diagnostic/data-quality reference reconciliation** — Cold / Phase 6 trigger：external reference evidence 已保存於 `evidence/inbox/telltale-reference-2026-10-04.md`。Phase 5C 不因此擴張；等 Phase 5C closure、Phase 6 readiness 啟動時，重讀該 evidence 並 bounded reconcile 至 `docs/REFERENCES.md`／`docs/references/SYNTHESIS.md` 或最低必要 canonical owner。重點僅限：`NO DATA != Unsupported`、multi-ECU disagreement 保持 ambiguity、decode/scaling fail-closed、diagnostic trace provenance、discovery/simulation 不授權 live actuation；GPL-3.0 source 預設不直接重用。未 promotion 前不可執行。
- **64-bit CAN frame-origin timestamp** — Cold / Deferred：目前 `CanFrame.timestamp` 仍沿用 Arduino `millis()` origin。Phase 5 normalized VehicleData timestamp 直接使用既有 64-bit `Clock::nowMs()` semantics，不因此 promotion 本項；只有當 passive CAN／source-frame freshness／frame-origin time 必須跨層保留到 normalized data 時，再評估最小 frame timestamp correction。未 promotion 前不可執行。
- **Generic namespace 命名** — Cold / Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。未 promotion 前不可執行。
- **ESP32 backend CI coverage** — Cold / Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。未 promotion 前不可執行。
