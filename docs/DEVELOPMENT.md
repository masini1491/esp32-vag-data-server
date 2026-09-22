# Development Roadmap

- Phase 0: Repository / Architecture Freeze v0.4
- Phase 1: Board abstraction + HAL + basic TWAI
- Phase 1A-1: Host-testable HAL / CAN foundation（本輪；不含 TWAI backend）
- Phase 1A-2: Deterministic Mock CAN / Fake Clock / host tests（本輪；不含 ISO-TP）
- Phase 1A-3: ESP32-S3 Classic CAN / TWAI backend（本輪；不含 protocol behavior）
- Phase 1A-4: Foundation consolidation / Phase 1 implementation gate（本輪）
- Phase 2A: Host-testable Classic CAN ISO-TP / DiagnosticTransport boundary（PASS；後續 service/application layers 尚未開始）
- Phase 3: Generic OBD-II read-only（Phase 3A `ReadOnlyGuard` safety gate PASS；OBD-II service semantics 尚未開始）
- Phase 4: Generic UDS read-only
- Phase 5: VehicleData + Scheduler
- Phase 6: VAG Brand Layer + Kamiq Profile
- Phase 6a: Brand extension boundary + Brand Profile Set / Active Profile（future implementation planning）
- Phase 6b: Capability registry + normalized signal registry（future implementation planning）
- Phase 6c: Deep Diagnostic on-demand path（future implementation planning）
- Phase 7: BLE
- Phase 8: Web
- Phase 9: Passive CAN
- Phase 10: Kamiq hardware/vehicle validation
- Phase 11: **Future / Pending** — T-Roc VAG portability validation
- Phase 12: **Future / Pending** — RAV4 cross-brand architecture validation
- Phase 13: **Future / Pending** — Wish Toyota cross-generation validation

## Validation sequence

```text
Kamiq → T-Roc → RAV4 → Wish
```

### Kamiq — first Hardware / Vehicle PASS

Required minimum dataset：VIN、`vehicle.speed`、`vehicle.rpm`、`vehicle.coolantTemp`、`vehicle.voltage`。若取得，再加入 oil temperature、gear、wheel speeds、steering angle、ACC set speed。Status：`Pending`，直到取得真正的 Hardware / Vehicle evidence。

### T-Roc — same-brand portability（Future / Pending）

驗證 VAG Brand Layer + 不同 VAG Vehicle Profile 是否成立。理想上只需變更 VAG routing、profile 與 vehicle-specific CAN/DID/scaling，不大量修改 ISO-TP、OBD-II、UDS、VehicleData、BLE、Web 或 Logger。若需大幅修改 Generic Core，應視為 architecture feedback。

### RAV4 — cross-brand portability（Future / Pending）

驗證 Generic Core 是否可保留，只新增 Toyota Brand Layer + RAV4 Vehicle Profile；不預先實作 Toyota code。

### Wish — cross-generation portability（Future / Pending）

驗證 Toyota Brand Layer 能否透過不同 Vehicle Profile 支援另一世代；不假設 Wish 與 RAV4 使用相同 protocol 或 CAN layout。

## Architecture Portability PASS

條件是 Kamiq → VAG Profile、T-Roc → second VAG Profile、RAV4 → future Toyota Brand Layer + Profile 都能在不修改 Generic Core semantics 的情況下輸出共同 normalized VehicleData。實車完成前狀態維持 `Pending`。不同車款若無法提供某項 normalized signal，應回報 `unsupported` / `unavailable`，不以假值代替。

## Phase 1 foundation status

- Software PASS：Generic CAN model、Board Profile → HardwareConfig → HAL、Mock CAN、Fake Clock 與 ESP32-S3 TWAI backend 已完成。
- Host Test PASS：Stage 4T 已以 `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp` local revalidation，tested commit `0c699d6`；current evidence authority：`VALIDATION.md`。
- CAN foundation edge-case regression coverage 已建立。
- ESP32 Compile：PASS。Stage 4T 已以 Arduino CLI 1.5.1、Arduino-ESP32 3.3.11、ESP-IDF 5.5.5、`esp32:esp32:esp32s3` 與獨立 TEMP build path 驗證 `src/esp32_twai_can.cpp`，並確認 backend participation；tested commit `0c699d6`。詳見 `VALIDATION.md`。
- Bench PASS：Pending。
- Hardware PASS：Pending。
- Vehicle PASS：Pending。

這代表 Phase 1 software foundation components、Stage 4R / Stage 4T hardening、Stage 5 evidence consolidation、Phase 2A host-testable ISO-TP / DiagnosticTransport core，以及 Phase 3A `ReadOnlyGuard` safety gate 已完成。Phase 3A 僅建立 Mode `0x01` / `0x09` semantic single-PID outbound enforcement；OBD-II service semantics、UDS、VAG routing、VehicleData 與 application-facing diagnostic TX 尚未開始。Phase 2A / 3A 均未重新執行 ESP32 compile，因 ESP32-facing source participation / platform boundary 未改變；這不代表實體 TWAI receive 或 vehicle validation 已完成。

Phase 2 v1 implementation 是 ISO-TP over Classic CAN；future non-CAN transports 僅為 architecture boundaries，不新增 K-Line implementation phase，也不改變 Kamiq → T-Roc → RAV4 → Wish validation sequence。
