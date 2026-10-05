# BACKLOG

本檔案是本 repository 的 Cold Registry，只保存 future／dormant／trigger-based durable memory。Cold item 沒有 execution authority，不可直接執行或用於 TASKS Short-launch；trigger 成立或使用者選中後，必須先重讀 current authority/evidence、reconcile premise，再 promote 到 `TASKS.md`。

## Cold / future-trigger

- **Logger / export contract** — Cold / Ready：application read model、Phase 8 Web consumer contract與 Phase 7 BLE host contract均已 canonical closure，因此技術 trigger 已成立。下一步若使用者選中，先 fresh-read current authority/evidence，再 promotion成唯一 Hot。範圍仍只允許 bounded normalized log/export record、availability/quality/provenance semantics與host tests，不建立第二套 signal identity，也不把 raw diagnostic log當主要 application contract。
- **Phase 6c Deep Diagnostic on-demand path** — Cold / Evidence-blocked：readiness evidence 位於 `evidence/inbox/phase6c-deep-diagnostic-readiness-2026-10-05.md`。Scheduler OnDemand、ReadOnlyGuard 與 Generic UDS `0x22` 已足夠作為 generic plumbing；目前缺少的是 Kamiq 2024 profile-owned concrete Deep Diagnostic mapping（至少 ECU/route、exact read-only DID/service、raw response validation、decode/scaling/unit、failure/status semantics與相符 evidence authority）。在第一個合法、安全、可驗證的 concrete consumer 出現前，不建立 callback/executor/request framework、不 invent route/DID/scaling、不直接 launch。Trigger 成立後先 fresh-read current authority/evidence，再 promotion 至 `TASKS.md`。
- **64-bit CAN frame-origin timestamp** — Cold / Deferred：目前 `CanFrame.timestamp` 仍沿用 Arduino `millis()` origin。Phase 5 normalized VehicleData timestamp 直接使用既有 64-bit `Clock::nowMs()` semantics，不因此 promotion 本項；只有當 passive CAN／source-frame freshness／frame-origin time 必須跨層保留到 normalized data 時，再評估最小 frame timestamp correction。未 promotion 前不可執行。
- **Generic namespace 命名** — Cold / Deferred：Generic Core 目前仍使用 `vag_data` namespace；等 library extraction 或第一個 non-VAG consumer 成為實際工作時，再評估 brand-neutral namespace，不為命名提前 churn。未 promotion 前不可執行。
- **ESP32 backend CI coverage** — Cold / Deferred：目前已有可重現 ESP32-S3 backend compile evidence，host CI 不編譯真實 TWAI backend。只有當 ESP32 backend 開始持續變更、manual compile validation 成為重複成本，或 repository 明確決定把 ESP32 compile 納入正式 CI / merge gate 時，再獨立評估最小 backend compile CI；Stage 5 不實作此項。未 promotion 前不可執行。
