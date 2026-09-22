# ESP32 VAG Data Server

**專案狀態：** Architecture Freeze v0.4 完成／Phase 1 software foundation PASS／Phase 2A host core PASS／Phase 3A ReadOnlyGuard PASS／Phase 3B OBD host core PASS／Phase 4A UDS ReadOnlyGuard PASS／Phase 4B UDS host core PASS／Phase 5A sample semantics PASS／Phase 5B bounded Store/Cache PASS

## 專案目的

本專案建立一個以 ESP32-S3 family 為平台的 Volkswagen Group 唯讀 vehicle data server，將 Vehicle CAN 的被動與診斷資料正規化為 `VehicleData`，供 BLE、Wi-Fi Web UI 與 Logger 使用。Phase 1 software foundation 與 Phase 2A host-testable Classic CAN ISO-TP / `DiagnosticTransport` core 已完成，但尚未代表實車支援或整個專案完成。

本專案目前仍以 VAG / Kamiq 為第一目標。Generic Core 刻意分離 transport、diagnostic protocol、brand semantics、Vehicle Profile 與 `VehicleData`；未來可能透過 Brand Layer / Vehicle Profile 擴充其他品牌，但目前不宣稱 multi-brand support。

官方 implementation target 仍為 VAG；其他 contributor 或 fork 未來可保留 Generic Core，自行加入 Brand Layer / Vehicle Profile。單一 firmware build 不要求包含所有品牌。

v1 concrete path 維持 ESP32-S3 + Classic CAN/TWAI + ISO-TP + OBD-II/UDS + VAG。Architecture 允許 future diagnostic-link extensions，但目前不支援 K-Line、motorcycle diagnostics 或任何 motorcycle brand/model。

## 初始研究／驗證車型

Škoda Kamiq 2024 facelift（MQB-A0 family）是初始研究／驗證目標。本專案架構不限定 Kamiq-only，目前也不宣稱已支援 Kamiq 2024。

## 已完成的 Phase 1 foundation

- Generic Classic CAN types
- `Board Profile → HardwareConfig → HAL` abstraction
- deterministic Mock CAN / Fake Clock host-test foundation
- ESP32-S3 Classic CAN / TWAI backend
- ESP32 generic S3 compile validation（Arduino CLI / Arduino-ESP32 3.3.11；`src/esp32_twai_can.cpp` participation PASS）
- GitHub Actions host compile / test CI
- CAN foundation edge-case regression tests
- Host-testable Classic CAN ISO-TP / `DiagnosticTransport` core
- Phase 2A deterministic ISO-TP host tests；GitHub Actions run `35688710299` PASS at `43a550de1e835fffad22e63f29e03943b6dea6c5`
- Phase 3A Generic `ReadOnlyGuard` safety gate：semantic OBD Mode `0x01` / `0x09` single-PID allowlist、fail-closed deny、lower transport status propagation，以及 deterministic host tests（implementation `5eefb442bc00e58005cc933fa6d56ab8bbe45ef6`）
- Phase 3B Generic OBD-II read-only host service：one outstanding request、Mode `0x01` raw data / supported-PID bitmap、Mode `0x09` PID `0x02` VIN response validation，以及 deterministic host tests（implementation `5e038fc114b5ea5b1bab6c1b517023f1e43e35c9`）
- Phase 4A Generic `ReadOnlyGuard` UDS extension：只允許 semantic `ReadDataByIdentifier (0x22)` single-DID request，guard 內部建立 `22 DID_hi DID_lo`，以及 deterministic host tests（implementation `4b8609302dc6569af53d7718d4ea75c152c682cb`）
- Phase 4B Generic UDS `ReadDataByIdentifier` host service：one outstanding DID read、`0x62` positive matching、terminal NRC 與 bounded `0x78` response-pending，以及 deterministic host tests（implementation `59a10b0f6453c07bc65f370bd6186b0f46e4a997`）
- Phase 5A normalized VehicleData sample/value semantics：opaque signal ID、normalized unit/source/quality/availability metadata、numeric/boolean/bounded text values、copy-safe storage、64-bit monotonic timestamp，以及 deterministic host tests（implementation `68864ba`）
- Phase 5B bounded VehicleData Store/Cache：fixed-capacity opaque-ID latest-state set、timestamp ordering、whole-sample replacement、copy-based lookup/snapshot，以及 deterministic host tests（implementation `d7b8552`）

目前 Bench、Hardware 與 Vehicle validation 均為 Pending。

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

預計使用 Arduino IDE、Git、GitHub、Python tools、PC-side protocol/unit tests，以及適當的 Mock HAL / mock TWAI / timing。Windows interactive/local development 若執行 repository-owned PowerShell tooling，正式 runtime 為 PowerShell 7 `pwsh`；Windows PowerShell 5.1 `powershell.exe` 不是正式 validation runtime，且不得 silent fallback。現有 Linux/Ubuntu CI 不受此 local runtime contract 影響。詳細 execution policy 請見 `AGENTS.md`。Python 主要用於 profile building、MCD/data conversion、trace parsing 與 test-vector generation，不會執行於 ESP32 firmware。

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

所有 GPIO 與 board-specific capability 經由 `Board Profile → HardwareConfig → HAL` 管理，不由 protocol/application layer 硬編。ESP32-S3 TWAI backend 已由 standard Arduino build compile validated；exact board、transceiver、GPIO 與實體 CAN hardware validation 仍為 TBD / Pending。詳見 `VALIDATION.md`。

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
Vehicle
→ Passive CAN / Diagnostic Network
→ CAN / DiagnosticTransport
→ ISO-TP
→ OBD-II / UDS
→ Brand Extension / VAG Brand Layer
→ Vehicle Profile
→ VehicleData Store / Cache
→ BLE / Web / Logger / future clients
```

Realtime clients 預計優先讀取 `VehicleData Cache`，不因為瀏覽器 refresh 就直接重複 query ECU。詳見 [Architecture Freeze v0.4](docs/ARCHITECTURE.md)、[Read-only policy](docs/READ_ONLY_POLICY.md) 與 [Vehicle Profile](docs/VEHICLE_PROFILE.md)。

## 目前開發狀態

已完成 Phase 1 software foundation、Phase 2A host core、Phase 3A `ReadOnlyGuard` safety gate、Phase 3B Generic OBD-II read-only host service、Phase 4A UDS `ReadOnlyGuard` safety extension、Phase 4B Generic UDS host service、Phase 5A normalized VehicleData sample/value semantics，以及 Phase 5B bounded VehicleData Store/Cache：Generic CAN model、Board / HardwareConfig / CAN HAL abstraction、deterministic Mock CAN / Fake Clock、ESP32-S3 TWAI Classic CAN backend、host CI regression tests、Classic CAN ISO-TP / `DiagnosticTransport` core、semantic OBD allowlist／host semantics、UDS `0x22` guard、one-request-at-a-time 的 raw DID data / negative NRC / bounded response-pending semantics、brand-independent opaque signal sample/value representation，以及 fixed-capacity latest-state sample ownership。Phase 2A 為 `43a550de`；Phase 3A 為 `5eefb44`；Phase 3B 為 `5e038fc`；Phase 4A 為 `4b86093`；Phase 4B 為 `59a10b0`；Phase 5A 為 `68864ba`；Phase 5B 為 `d7b8552`；以上已有 local host compile/test PASS，Phase 2A GitHub Actions run `35688710299` PASS。

尚未開始或尚未完成：其他 Generic UDS services、session lifecycle、DTC modes、TesterPresent、VehicleData Store eviction/TTL/automatic aging、Scheduler、signal/capability registry、VAG DID/scaling、VAG Brand Layer / Kamiq profile implementation、application-facing diagnostic TX、BLE、Web、passive CAN decoding，以及 real hardware / vehicle validation。Phase 2A / 3A / 3B / 4A / 4B / 5A / 5B 未重新執行 ESP32 compile，因 ESP32-facing source participation / platform boundary 未改變；既有 Phase 1 ESP32 compile evidence 維持原 scope。Bench、Hardware、Vehicle 仍為 Pending。

研究與開發規劃詳見 [Development roadmap](docs/DEVELOPMENT.md)；upstream reference index 見 [REFERENCES.md](docs/REFERENCES.md)。

## 免責聲明

本專案與 Volkswagen AG、Škoda Auto、Ross-Tech 及文件中引用的 upstream projects 均無隸屬、授權或官方合作關係。
