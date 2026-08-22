# Architecture Freeze v0.2

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
   Diagnostic Transport
   └─ ISO-TP over Classic CAN（v1）
         ↓
   Diagnostic Protocol
   ├─ OBD-II
   └─ UDS
         ↓
   Brand Layer
   └─ VAG（v1）
         ↓
   Vehicle Profile
         ↓
   VehicleData Store / Cache
         ↓
   BLE / Web / Logger / future clients
```

硬體邊界：`Board Profile → HardwareConfig → HAL → CAN/TWAI`。

## Generic Core 與 Brand Layer

Generic Core 必須與品牌無關，包含 Board Profile、HardwareConfig、HAL、CAN/TWAI abstraction、ISO-TP、Generic OBD-II、Generic UDS、ReadOnlyGuard、VehicleData、Scheduler、Storage、BLE、Web 與 Logger。

Generic Core 不得直接依賴 VAG CAN ID、DID、ECU address、scaling、MQB assumptions 或 Kamiq-specific definitions。品牌專屬 ECU routing、CAN IDs、DIDs、scaling 與 semantics 必須限制於 Brand Layer / Vehicle Profile。

```text
Generic Core
    ↓
Brand Layer
    ├─ VAG（目前唯一實際規劃）
    └─ future brands（概念 portability example）
         ↓
Vehicle Profile
         ↓
VehicleData
```

`BrandAdapter` 目前只代表 architecture boundary / future interface concept，不建立 virtual-class hierarchy 或 placeholder implementation。Repository 仍為 VAG-first，不宣稱 multi-brand support。

## Diagnostic responsibility boundaries

```text
Physical / Link Layer
        ↓
Diagnostic Transport
        ↓
Diagnostic Protocol
        ↓
Brand Layer
```

v1 實際 scope 為 `ESP32 TWAI / Classic CAN → ISO-TP → OBD-II / UDS → VAG Brand Layer`。Diagnostic Transport 處理 framing、segmentation、flow control、addressing 與 timeout；Diagnostic Protocol 處理 OBD-II / UDS semantics。UDS 不依賴特定品牌，也不把 CAN addressing 當成 application semantics。

未來僅保留 boundary：

```text
DiagnosticTransport
├─ ISO-TP over CAN（v1）
├─ HSFZ（future only）
└─ DoIP（future only）
```

目前不實作 HSFZ、DoIP、Ethernet diagnostics 或 CAN-FD，也不增加 speculative transport framework code。Current v1 hardware scope 是 ESP32-S3 + Classic CAN transceiver；CAN-FD 若未來需要，可能需要 external CAN-FD controller 或不同 hardware architecture，現階段不選定硬體。

## VehicleData contract

`VehicleData` 是 transport-independent、brand-independent、vehicle-independent 的 normalized application interface。Examples 包含 `vehicle.speed`、`vehicle.speedDisplayed`、`vehicle.rpm`、`vehicle.gear`、`vehicle.coolantTemp`、`vehicle.oilTemp`、`vehicle.voltage`、四輪 wheel speed、`vehicle.steering.angle`、`vehicle.acc.active`、`vehicle.acc.setSpeed` 與 seatbelt status。

```text
VAG raw CAN / DID → VAG decoder/profile → VehicleData
Toyota raw CAN / diagnostic data → Toyota decoder/profile → VehicleData（future only）
```

BLE、Web、Logger 與 HUD client 只依賴 VehicleData，不應知道 CAN ID、DID、ECU physical address 或品牌-specific raw format。不存在的 capability 必須回報 `unsupported` / `unavailable`，不能以假值代替。

## Frozen principles

1. Generic CAN / ISO-TP / OBD-II / UDS / VehicleData / BLE / Web / Logger 不得包含 VAG/Kamiq-specific constants 或 semantics。
2. 不同硬體版本以新增 Board Profile 支援，不修改 Generic Core semantics。
3. `VehicleData` 是所有 client 的統一介面；server 統一取得資料並 cache，client 不直接輪詢 ECU。
4. 所有 diagnostic TX 必須經 `ReadOnlyGuard`；read-only policy 不因 Brand Layer 而放寬。
5. Future brand support 必須先有實際 evidence，不預先加入 speculative code。

## Non-goals for v0.2

本輪不實作 CAN、ISO-TP、OBD-II、UDS、BLE、Web server、BrandAdapter、Toyota code、CAN-FD、HSFZ、DoIP 或 firmware。HUD 是另一個 client project。
