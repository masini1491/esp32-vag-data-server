# Vehicle Profile

Vehicle Profile 是隔離 vehicle family、platform、model、model year/generation 與品牌差異的資料與規則層，不能把 protocol implementation 寫進 profile。

## Profile responsibilities

Profile 可描述：vehicle family、platform、model、model year / generation、ECU routing、DID mapping、passive CAN signal mapping、scaling、unit、source preference 與 capability availability。

```text
Generic ISO-TP
Generic UDS
        ↓
VAG Brand Layer
        ↓
VAG MQB-A0 Kamiq 2024 Profile
        ↓
VehicleData
```

同品牌未來可使用不同 profile：

```text
VAG
├─ Kamiq profile（v1 target）
└─ T-Roc profile（future validation）
```

跨品牌的 Toyota Brand Layer、RAV4 profile 與 Wish profile 目前只是 future architecture example，不建立 source directory 或 implementation。不同世代不保證使用相同 diagnostic protocol 或 CAN layout，必須依實車 evidence 決定。

## Current status

初始 research / validation target 為 Škoda Kamiq 2024 facelift（MQB-A0 family）。實際 ECU、DID、scaling、CAN route、gateway access 與 capability 均待實車驗證；目前沒有 `VEHICLE_CONFIRMED` 項目。
