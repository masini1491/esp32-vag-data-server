# ESP32 VAG Data Server

**專案狀態：** 架構設計／研究階段

## 專案目的

本專案規劃建立一個以 ESP32-S3 family 為平台的 Volkswagen Group 唯讀 vehicle data server，將 Vehicle CAN 的被動與診斷資料正規化為 `VehicleData`，供 BLE、Wi-Fi Web UI 與 Logger 使用。

本專案目前仍以 VAG / Kamiq 為第一目標。Generic Core 刻意分離 transport、diagnostic protocol、brand semantics、Vehicle Profile 與 `VehicleData`；未來可能透過 Brand Layer / Vehicle Profile 擴充其他品牌，但目前不宣稱 multi-brand support。

官方 implementation target 仍為 VAG；其他 contributor 或 fork 未來可保留 Generic Core，自行加入 Brand Layer / Vehicle Profile。單一 firmware build 不要求包含所有品牌。

## 初始研究／驗證車型

Škoda Kamiq 2024 facelift（MQB-A0 family）是初始研究／驗證目標。本專案架構不限定 Kamiq-only，目前也不宣稱已支援 Kamiq 2024。

## 預計技術棧

### Firmware

- **主要語言：** C++ / Arduino
- **Framework：** Arduino Framework for ESP32
- **底層 API：** ESP-IDF TWAI / FreeRTOS APIs
- **目標 MCU family：** ESP32-S3
- **CAN：** ESP32 TWAI（Classic CAN）
- **Transport：** ISO-TP / ISO 15765-2
- **通用診斷：** OBD-II
- **VAG 診斷：** UDS
- **設定儲存：** NVS
- **無線連線：** BLE + Wi-Fi SoftAP

### Arduino Framework 與 ESP-IDF 分工

本專案規劃維持 Arduino Framework / C++ 的主要開發流程，不是 pure ESP-IDF project：

```text
Arduino Framework / C++
├─ Arduino ecosystem
│  ├─ BLE
│  ├─ Wi-Fi
│  ├─ Web
│  └─ NVS / application integration
│
└─ ESP-IDF APIs
   ├─ TWAI
   └─ FreeRTOS
```

主要 application integration、BLE、Wi-Fi、Web 與 NVS 預計使用 Arduino ecosystem；CAN/TWAI 以及 task、queue 等需要底層控制的部分，可直接使用 Arduino-ESP32 所提供的 ESP-IDF APIs。不需要為了 TWAI / FreeRTOS 改成 pure ESP-IDF project。

### Web UI

預計由 ESP32-hosted Web UI 提供：

- HTML
- CSS
- JavaScript
- REST/API 或其他適合 ESP32 的資料介面

實際 Web library 尚未固定，不在架構階段寫死特定 library。

### Vehicle data / profiles

預計使用 C++ runtime data model、normalized `VehicleData`、Vehicle Profile abstraction，以及適當的 JSON-based development/profile data。大型或複雜的 profile data 預計先由 PC-side tools preprocessing，再提供 ESP32 runtime 使用。

Client 不應直接依賴 raw CAN ID 或 raw UDS DID。資料流程規劃為：

```text
CAN / OBD-II / UDS / Passive CAN
→ Vehicle Profile / decoder
→ normalized VehicleData
→ BLE / Web / Logger / future HUD
```

### Development tools

預計使用 Arduino IDE、Git、GitHub、Python tools、PC-side protocol/unit tests，以及適當的 Mock HAL / mock TWAI / timing。Python 主要用於 profile building、MCD/data conversion、trace parsing 與 test-vector generation，不會執行於 ESP32 firmware。

## 預計輸出介面

- BLE 即時遙測資料
- Wi-Fi Web UI
- Logger
- 未來 HUD Client（獨立專案）

HUD 不屬於本 Repository；未來會是另一個 client project。

## 硬體狀態

- MCU family：ESP32-S3
- 實際開發板：TBD
- CAN transceiver：TBD
- GPIO 配置：TBD

所有 GPIO 與 board-specific capability 預計經由 `Board Profile → HardwareConfig → HAL` 管理，不由 protocol/application layer 硬編。

## 唯讀範圍與安全邊界

本專案從 architecture 層級維持 read-only。所有 diagnostic TX 最終都必須經 `ReadOnlyGuard`；Web、BLE、API 不得提供 arbitrary raw UDS TX bypass。

明確不包含：

- Coding
- Adaptation
- Clear DTC
- Security Access
- Output Tests
- Basic Settings
- actuator control
- flashing

## 系統架構

```text
Vehicle CAN
→ Passive CAN / Diagnostic CAN
→ ISO-TP
→ OBD-II / UDS
→ VAG Data Layer
→ Vehicle Profile
→ VehicleData Store / Cache
→ BLE / Web / Logger
```

Realtime clients 預計優先讀取 `VehicleData Cache`，不因為瀏覽器 refresh 就直接重複 query ECU。詳見 [Architecture Freeze v0.2](docs/ARCHITECTURE.md)、[Read-only policy](docs/READ_ONLY_POLICY.md) 與 [Vehicle Profile](docs/VEHICLE_PROFILE.md)。

## 目前開發狀態

目前仍處於 Architecture / Research stage。本輪尚未實作 CAN、ISO-TP、OBD-II、UDS、BLE、Web server 或 firmware；上述技術棧均為預計／規劃方向。

研究與開發規劃詳見 [Development roadmap](docs/DEVELOPMENT.md)；upstream reference index 見 [REFERENCES.md](docs/REFERENCES.md)。

## 免責聲明

本專案與 Volkswagen AG、Škoda Auto、Ross-Tech 及文件中引用的 upstream projects 均無隸屬、授權或官方合作關係。
