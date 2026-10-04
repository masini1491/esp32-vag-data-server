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

不同 profile 的 capabilities 可以不同，包含 Realtime Telemetry、Generic OBD-II、ECU Identification、Read DTC、Passive CAN、ACC Data、SRS Live Data 或其他 optional diagnostics。Capability support authority 必須與 transient runtime availability 分開：只有單次 read failure、timeout 或 `NO DATA` 不足以判定 `unsupported`；support 未驗證時維持 `pending` / `unknown`，暫時無法讀取則表達為 `unavailable`。

多個 ECU/controller 回應互相矛盾時，在 Profile/source 尚無明確 arbitration authority 前必須保留 `Ambiguous` / unresolved，不得依 first/last response、registration order 或數值看似合理來選值。Malformed payload、unsupported decode/scaling、out-of-range data 或語意不足必須 fail closed；不得產生看似正常的 VehicleData value，也不得以零值、猜測或近似 scaling 暗中代替。

## Current VAG target

Phase 6b 的 `src/profiles/signal_registry.h` 擁有 fixed immutable normalized metadata lookup。`CapabilitySupport`（Supported／Unsupported／Pending／Unknown）與 sample `VehicleAvailability` 是獨立型別；不含 transient Unavailable，不接受 runtime failure 作 support mutation。未知 signal 回傳 not-found，不推導 Unsupported。`src/vag/kamiq_nw4_capabilities.h` 將現有 Kamiq identity 關聯到下列 Numeric candidate metadata，全部 Pending；selection 不提升 support，lookup 不產生 sample 或更新 Store。

| Existing normalized SignalId | Expected value type | Unit | Support |
|---|---|---|---|
| `vehicle.speed` | Numeric | `km/h` | Pending |
| `vehicle.rpm` | Numeric | `rpm` | Pending |
| `vehicle.coolantTemp` | Numeric | `degC` | Pending |
| `vehicle.voltage` | Numeric | `V` | Pending |

這些 unit 是 normalized metadata，不是 raw encoding/scaling；沒有 ECU/source routing、arbitration、polling 或 diagnostic execution 定義。

Phase 6a 的 host-testable implementation 位於 `src/profiles/active_vehicle_profile.h` 與 `src/vag/profile_set.h`。前者僅擁有 opaque identity、固定 admitted identities 與 manual selection；`Unknown`、`Ambiguous`、`ManualSelectionRequired` 不暴露 active identity，只有 `Selected` 暴露一個 admitted identity。後者只有 Pending 的 Kamiq 2024 facelift target reference `Kamiq_NW4`；選中它不建立 capability、VehicleData value 或實車支援。Automatic resolver、routing/mapping 尚未實作。

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
