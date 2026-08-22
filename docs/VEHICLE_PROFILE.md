# Vehicle Profile

Vehicle Profile 是隔離 vehicle family、platform、model、generation 與品牌差異的資料與規則層；不包含 protocol implementation。

## Profile responsibilities

Profile 可描述：

- platform、model、generation
- ECU route、DID
- passive CAN mapping
- scaling、units
- capability availability
- polling policy
- signal source priority
- evidence / validation state

```text
Brand Layer
    ↓
Brand Profile Set
    ↓
Profile Resolver
    ↓
Active Vehicle Profile
    ↓
Capability / Signal Mapping
    ↓
VehicleData
```

Profile Set 應能重用 common/platform definitions，例如 `VAG Common → MQB Common → MQB-A0 Common → Kamiq Overlay`，但目前不設計完整 inheritance engine、parser、schema 或 file/storage format。

Resolver 的可能 evidence 包含 VIN、ECU identification、firmware fingerprints、platform identifiers、CAN fingerprints 與 manually selected profile。Evidence 不足時必須回報 `Unknown`、`Ambiguous` 或 `Manual selection required`；不得亂猜。Runtime 只需要 Active Vehicle Profile。

## Capabilities

不同 profile 的 capabilities 可以不同，包含 Realtime Telemetry、Generic OBD-II、ECU Identification、Read DTC、Passive CAN、ACC Data、SRS Live Data 或其他 optional diagnostics。不存在的 capability 為 `unsupported`；暫時讀不到為 `unavailable`；尚未驗證為 `pending` / `unknown`，不得用假值代替。

## Current VAG target

```text
Generic ISO-TP / Generic UDS
        ↓
VAG Brand Layer
        ↓
VAG Profile Set
        ↓
VAG MQB-A0 Kamiq 2024 Active Profile（future runtime concept）
        ↓
VehicleData
```

初始 research / validation target 為 Škoda Kamiq 2024 facelift（MQB-A0 family）。實際 ECU route、DID、scaling、passive CAN mapping、SRS/pretensioner availability 與 capability 均待實車驗證；目前沒有 `VEHICLE_CONFIRMED` 項目。
