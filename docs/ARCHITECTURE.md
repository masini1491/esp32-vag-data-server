# Architecture Freeze v0.4

本文件凍結概念責任邊界，不代表 firmware 功能已實作。

## High-level architecture

```text
Vehicle
│
├─ Passive Vehicle Network
│        ↓
│   Passive Decoder
│
└─ Diagnostic Network
         ↓
   Vehicle Physical / Link
   ├─ Classic CAN / ESP32 TWAI（v1）
   └─ future non-CAN link（conceptual boundary only）
         ↓
   Diagnostic Framing / Transport
   ├─ ISO-TP（v1）
   └─ future transport family（conceptual boundary only）
         ↓
   Diagnostic Service Semantics
   ├─ OBD-II / UDS（v1）
   └─ future brand/profile-specific semantics
         ↓
   Brand Extension Interface
         ↓
   VAG Brand Layer（v1）
         ↓
   VAG Profile Set
         ↓
   Profile Resolver
         ↓
   Active Vehicle Profile
         ↓
   Capability / Signal Mapping
         ↓
   VehicleData Store / Cache
         ↓
   BLE / Web / Logger / future clients
```

v1 硬體邊界：`Board Profile → HardwareConfig → CAN HAL → ESP32 TWAI`。所有 active diagnostic TX 必須經 `ReadOnlyGuard` / diagnostic policy gate。

## Diagnostic Link / Transport extensibility

Architecture 將 vehicle physical/link、diagnostic framing/transport、diagnostic service semantics 與 Brand Layer / Vehicle Profile 分開。這是 conceptual boundary only；本文件不決定 C++ interface、method signature、ownership、callback、queue 或 async model。

目前唯一 official implementation path 維持：

```text
ESP32-S3
→ Classic CAN / TWAI
→ ISO-TP
→ Generic OBD-II / UDS
→ VAG Brand Layer / VAG Vehicle Profile
→ VehicleData
```

Future transport-extensibility evidence 顯示，部分診斷系統可能採用保守稱為 `K-Line / ISO 9141 / ISO 14230 / KWP2000 family` 的路徑，並透過 UART + K-Line PHY 與 ECU 通訊。此處只證明 future link 不一定是 CAN，不宣稱任何 motorcycle brand/model 已支援，也不凍結精確 OSI layer classification：

```text
K-Line（future / NOT SUPPORTED）
→ UART + K-Line PHY
→ ISO 9141 / ISO 14230 / KWP2000 family
→ brand/profile-specific diagnostic semantics
→ Vehicle Profile
→ VehicleData
```

Diagnostic/application upper layers 不得要求所有 backend 暴露 CAN ID、CAN frame、DLC、TWAI type 或 CAN-specific routing。ISO-TP implementation 本身可以依賴 generic CAN frame 與 TX/RX CAN IDs，但 OBD-II、UDS、future diagnostic service、VehicleData、BLE、Web 與 Logger 不應被迫依賴 CAN-specific implementation types。OBD-II / UDS 是 VAG v1 paths，不是所有 future Brand Layer 的 universal requirement。

既有 `Board Profile → HardwareConfig → CanHal → MockCan / Esp32TwaiCan` 保持有效；不得為 future K-Line 將 CanHal 擴張成巨大 generic bus interface。若未來有 evidence，non-CAN link 應以平行 hardware/link backend 加入，而非重寫 Phase 1 CAN foundation。

## Generic Core 與 Brand Extension

Generic Core 包含可重用的 HAL、CAN/TWAI、diagnostic link/transport implementations、Generic OBD-II / UDS、ReadOnlyGuard、VehicleData、Scheduler、Storage、BLE、Web 與 Logger。CAN abstraction 仍是 v1 Generic Core infrastructure；但 normalized application/data layers 不得假設 vehicle diagnostic link 一定為 CAN。Generic Core 不得知道任何 VAG CAN ID、ECU address、DID、MQB/Kamiq semantics，或其他品牌-specific constants。

Brand Layer 負責 brand-specific vehicle identification strategy、ECU discovery/routing semantics、diagnostic/passive CAN interpretation、raw-to-normalized mapping，以及選擇適當 Vehicle Profile。

```text
                Generic Core
        ┌──────────────────────────┐
        │ HAL / CAN / ISO-TP       │
        │ OBD-II / UDS             │
        │ ReadOnlyGuard / VehicleData │
        │ Scheduler / Storage / BLE / Web / Logger │
        └────────────┬─────────────┘
                     │
              Brand Extension
                     │
        ┌────────────┴─────────────┐
        │                          │
   VAG Brand Layer          Other Brand Layer
   （本專案目標）            （future contributor）
        │                          │
   VAG Profile Set          Brand Profile Set
        │                          │
   Active Profile           Active Profile
```

`BrandAdapter` 目前只是 architecture concept name；不決定 C++ interface、virtual class、method signature 或 runtime plugin system。單一 firmware build 不要求同時包含所有品牌：未來可採 `Generic Core + VAG Brand Layer + VAG Profile Set` 的 project/build-selected 組合；其他 contributor 可另做其他品牌組合。本專案目前只規劃 VAG build，仍是 VAG-first，不宣稱 multi-brand support。

## Brand Profile Set、Resolver 與 Active Profile

```text
Brand Layer
    ↓
Brand Profile Set
    ↓
Profile Resolver
    ↓
Active Vehicle Profile
    ↓
Signal / capability mapping
    ↓
VehicleData
```

同一品牌可重用 common/platform definitions，例如 `VAG Common → MQB Common → MQB-A0 Common → Kamiq Overlay`，避免每個車型完整複製 mappings；但本輪不設計 inheritance engine、parser、schema 或 storage format。

Profile Resolver 未來可參考 VIN、ECU identification、firmware fingerprints、platform identifiers、CAN fingerprints 或 manually selected profile，但不定義 detection algorithm，也不假設 VIN 一定足以辨識。Evidence 不足時必須是 `Unknown`、`Ambiguous` 或 `Manual selection required`，不得亂猜。ESP32 runtime 只需使用一個 Active Vehicle Profile，不需將所有 profile 同時展開至 RAM。

## Capability-driven VehicleData

`VehicleData` 是可擴充的 normalized application interface，不是要求所有品牌/車型具備所有欄位的巨大固定結構。Signal semantics 可包含：Signal ID、Value、Unit、Timestamp、Source、Quality、Availability。

只有 Active Vehicle Profile 支援的 signal 才能成為有效 capability；`unsupported`、`unavailable`、`pending` / `unknown` 不得用 `0`、false、empty value 或其他假值代替。

Examples：`vehicle.speed`、`vehicle.rpm`、`vehicle.coolantTemp`、`vehicle.acc.setSpeed`、`vehicle.srs.driverPretensioner.resistanceOhm`。BLE、Web、Logger、HUD 只依賴 VehicleData + Capability model，不直接知道品牌 raw mapping、DID、CAN frame ID、ECU address 或 Brand Layer internal types。

## Deep Diagnostic

Deep Diagnostic 是非高頻 realtime telemetry、通常由特定 ECU 提供，且依 brand/platform/profile 定義的 read-only diagnostics，例如 SRS/Airbag measuring values、seatbelt pretensioner circuit/resistance/status、igniter circuit、crash/deployment information、ABS/ACC extended values、ECU identification 與 extended DTC information。

SRS / pretensioner 可使用下列 normalized namespace examples：

```text
vehicle.srs.driverPretensioner.present
vehicle.srs.driverPretensioner.resistanceOhm
vehicle.srs.driverPretensioner.status
vehicle.srs.driverPretensioner.fault
vehicle.srs.passengerPretensioner.present
vehicle.srs.passengerPretensioner.resistanceOhm
vehicle.srs.passengerPretensioner.status
vehicle.srs.passengerPretensioner.fault
```

這些只是 capability examples。Kamiq 的 ECU route、DID、scaling、physical interpretation 與 availability 全部維持 Pending；不得由 resistance 單獨推論原裝、未引爆、未維修或沒有 simulator/resistor。

Deep Diagnostic 預設 `ON_DEMAND`，必要時才依 evidence 採 `LOW_FREQUENCY`，不做無依據高頻 background polling。責任流程為：

```text
Web / BLE / client
  ↓ capability check
Scheduler / on-demand request
  ↓
ReadOnlyGuard / diagnostic policy gate
  ↓
diagnostic service
  ↓
transport / link → ECU → decode → VehicleData / diagnostic result
```

Scheduler 未來需區分 realtime、periodic、startup、on-demand、unsupported；本輪不實作。

## Frozen rules and non-goals

1. Brand-specific routing、CAN、DID、scaling、ECU semantics 只能位於 Brand Layer / Vehicle Profile。
2. 新品牌不得透過修改 Generic Core special-case 接入；未有 evidence 不加入 speculative implementation。
3. Client 不得直接知道 brand-specific raw diagnostic mapping。
4. 所有 diagnostic TX 永遠經 ReadOnlyGuard。
5. Profile implementation 應支持 composition/reuse，但 profile file/storage format 保持 undecided。
6. Diagnostic/application upper layers 不得直接依賴 CAN/TWAI-specific types；future non-CAN path 亦不得繞過 ReadOnlyGuard。
7. K-Line / motorcycle diagnostics 僅為 Future / out-of-scope extension evidence，不得據此加入 speculative implementation 或 support claim。

v1 仍限定 ESP32 TWAI / Classic CAN → ISO-TP → OBD-II / UDS → VAG。K-Line、ISO 9141、ISO 14230、KWP2000、motorcycle support、HSFZ、DoIP、Ethernet diagnostics、CAN-FD、BrandAdapter、Profile Resolver 與 Signal Registry implementation 均為本輪 non-goals。
