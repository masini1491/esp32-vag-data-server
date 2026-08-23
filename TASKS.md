# TASKS

本檔案是 ChatGPT／Codex 共用的暫存工作佇列，只保留尚未完成的工作。正式規格、`AGENTS.md`、architecture、security policy、protocol evidence 與其他 source of truth 優先於本檔案。

Codex 不得因為看到本檔案就自行處理所有項目；每次只執行使用者當次明確授權的 scope。工作成功完成並驗證後，應移除或更新對應項目；沒有任何未完成工作時刪除本檔案。

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

- [ ] **更新 `AGENTS.md` 的 TASKS.md 永久規則**：加入「若 repository root 存在 `TASKS.md`，任務開始時優先讀取；成功完成並驗證授權工作後同步移除/更新對應項目；不得未經授權執行其他 TASKS；沒有未完成工作時刪除 `TASKS.md`」的永久規則。
- [ ] **補上 library-ready design 原則**：Generic protocol/data layers 應維持未來可抽成 reusable library 的依賴方向，但目前不得為 library 化提前拆 repository、加入 speculative abstraction、package/release infrastructure 或無實際 consumer 的 generic API。等 ISO-TP → OBD/UDS → Brand Layer → Vehicle Profile → VehicleData 實際資料流穩定後再評估抽離。

## Deferred

- [ ] **64-bit monotonic timestamp** — Deferred：目前 `frame.timestamp` 使用 Arduino `millis()`；長時間 uptime 約 49.7 天 rollover 的問題留到真正需要 frame freshness / long-running VehicleData semantics 時處理。Phase 2 timeout 優先使用獨立 `Clock` abstraction。
- [ ] **Generic namespace 命名** — Deferred：Generic Core 目前仍使用 `vag_data` namespace。等 library extraction 或第一個非 VAG consumer 成為實際工作時，再評估是否改為 brand-neutral namespace；現在不為命名進行大規模 churn。
- [ ] **ESP32 backend CI coverage** — Deferred until build layout is fixed：host CI 目前不編譯真正的 ESP32 TWAI backend。先完成 P0 build-layout/reproducible build，再決定是否加入 Arduino-ESP32 compile CI 或其他最小 backend compile gate。
