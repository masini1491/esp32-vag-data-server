# BACKLOG

本檔案是本 repository 的 Cold Registry，只保存 future／dormant／trigger-based durable memory。Cold item 沒有 execution authority，不可直接執行或用於 TASKS Short-launch；trigger 成立或使用者選中後，必須先重讀 current authority/evidence、reconcile premise，再 promote 到 `TASKS.md`。

## Cold / future-trigger

- **64-bit monotonic timestamp** — Cold / Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；等真正需要 frame freshness / long-running VehicleData semantics 時再處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。未 promotion 前不可執行。
- **Generic namespace 命名** — Cold / Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。未 promotion 前不可執行。
- **ESP32 backend CI coverage** — Cold / Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。未 promotion 前不可執行。
