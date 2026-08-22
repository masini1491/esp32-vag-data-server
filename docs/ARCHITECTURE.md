# Architecture Freeze v0.1

本文件凍結概念架構，不代表 firmware 功能已實作。

```text
Vehicle
├─ Passive CAN
└─ Diagnostic CAN
   └─ ISO-TP
      ├─ OBD-II
      └─ UDS
         ↓ VAG Data Layer
         ↓ Vehicle Profile
         ↓ VehicleData Store / Cache
         ↓ BLE / Web / Logger
```

ESP32-S3 是 platform family，不綁定特定 board。Arduino Framework 為主要框架，必要時可使用 Arduino-ESP32 提供的 ESP-IDF TWAI / FreeRTOS API。

## Frozen principles

1. Board-specific capability 只能經 `Board Profile → HardwareConfig → HAL` 暴露；protocol/application layer 不硬編 GPIO。
2. 開發階段可由 Web UI 將經 validation 的 GPIO 設定寫入 NVS；reboot 後經 HardwareConfig 生效，不做 runtime hot switching。硬體定案後可關閉此能力，但 abstraction 永久保留。
3. 不同硬體版本以新增 Board Profile 支援，不修改 OBD/UDS/VAG/Application logic。
4. `VehicleData` 是所有 client 的統一介面；server 統一取得資料並 cache，client 不直接輪詢 ECU。
5. Passive CAN、OBD-II、UDS 都可成為 VehicleData source。每筆 signal 預計包含 signal ID、value、unit、timestamp、source、quality/status。
6. Scheduler 未來控制 polling frequency；DTC、SRS measuring values 等 diagnostic data 可採 on-demand。

## Non-goals for v0.1

本輪不實作 CAN、ISO-TP、OBD-II、UDS、BLE、Web server、Vehicle Profile parser 或 framework code；HUD 是另一個 client project。
