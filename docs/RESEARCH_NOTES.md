# Research Notes

## Known / architecture assumptions

- Initial vehicle target: Škoda Kamiq 2024 facelift。
- Expected platform family: MQB-A0。
- ESP32-S3 selected as data-server platform family。
- HUD expected to be a separate client project。
- Server is read-only by design。

## Pending vehicle validation

- 2024 Kamiq 實際 OBD Gateway 可存取的 ECU 清單。
- OBD pins 6/14 是否可看到所需 passive CAN traffic。
- 實車 CAN bitrate / routes。
- J234 Airbag read-only measuring values accessibility。
- Pretensioner resistance exact DID / scaling。
- ACC target/set-speed source。
- Cluster displayed speed source。
- Individual wheel-speed source。
- 2024 facelift 是否與現有 MQB-A0 definitions 有差異。
- SFD / gateway restrictions 對 read-only measurement values 的實際影響。

以上未經實車驗證內容均為 Pending / hypothesis，不能寫成 Hardware PASS。

## Reference Research v0.1

### Upstream evidence

- `CONFIRMED`: ESP32-S3 + Arduino + ESP-IDF TWAI 架構已有成熟 upstream reference。
- `CONFIRMED`: ISO-TP + Generic OBD-II + host-side protocol tests 有成熟 upstream reference。
- `REFERENCE_PATTERN`: PC mock HAL testing 是本專案建議採用方向。
- `REFERENCE_PATTERN`: VAG diagnostic profile 應考慮 PC-side preprocessing/flattening，而非 ESP32 runtime 解析大型 database。
- `UPSTREAM_COVERAGE_CONFIRMED`: Kamiq 2024 在 CanBusHack `vehicle_coverage` 中有直接 vehicle-specific coverage entry。
- `UPSTREAM_COVERAGE_CONFIRMED`: Kamiq 2024 coverage 列出 driver/front-passenger belt tensioner Ohm-value measurement descriptions。
- `UPSTREAM_COVERAGE_CONFIRMED`: Kamiq 2024 passive signal coverage 包含 vehicle speed、displayed speed、wheel speeds、oil temperature、ACC set speed 等值得研究訊號。

以上均為 upstream evidence，不是 Hardware PASS 或 `VEHICLE_CONFIRMED`。

### Pending vehicle validation

- Exact J234 DID for pretensioner resistance。
- Resistance scaling / raw encoding。
- Required diagnostic session。
- SFD / gateway restrictions。
- Exact passive CAN ID / bit / scaling。
- OBD pins 6/14 visibility。
- Actual Kamiq 2024 validation。

## Architecture Research v0.2

### Confirmed architecture decisions

- `CONFIRMED ARCHITECTURE DECISION`: Generic Core 應保持 brand-independent。
- `CONFIRMED ARCHITECTURE DECISION`: Brand-specific semantics 屬於 Brand Layer / Vehicle Profile。
- `CONFIRMED ARCHITECTURE DECISION`: Diagnostic protocol 與 diagnostic transport 必須分離責任邊界。
- `CONFIRMED ARCHITECTURE DECISION`: v1 維持 Classic CAN + ISO-TP + OBD-II/UDS + VAG。
- `CONFIRMED ARCHITECTURE DECISION`: multi-brand support 僅是 future validation，不是目前支援狀態。

### Future vehicle validation sequence

`Kamiq → T-Roc → RAV4 → Wish`

- Kamiq：第一個完整 Hardware / Vehicle validation target。
- T-Roc：VAG Brand Layer + second VAG Vehicle Profile portability validation（Future / Pending）。
- RAV4：Toyota Brand Layer + Profile 的 cross-brand validation（Future / Pending）。
- Wish：Toyota same-brand / different-generation validation（Future / Pending）。

共同 minimum dataset 為 VIN、`vehicle.speed`、`vehicle.rpm`、`vehicle.coolantTemp`、`vehicle.voltage`；optional 為 gear、oilTemp、wheel speeds、steering angle、ACC set speed。不存在的 capability 應標示 `unsupported` / `unavailable`，不得以假值代替。

所有項目直到取得實車 evidence 前均維持 `Pending`，不得宣稱 T-Roc、RAV4 或 Wish supported。

## Architecture Research v0.3

### Confirmed architecture decisions

- `CONFIRMED ARCHITECTURE DECISION`: Multi-brand extensibility 不代表一個 firmware shipping 所有品牌；build/project 可選擇所需 Brand Layer / Profile Set。
- `CONFIRMED ARCHITECTURE DECISION`: Generic Core remains reusable。
- `CONFIRMED ARCHITECTURE DECISION`: Brand implementations 未來可依 project/build selection 加入，其他 contributor 可自行延伸。
- `CONFIRMED ARCHITECTURE DECISION`: Brand Profile Set → Profile Resolver → Active Vehicle Profile 是預期責任模型。
- `CONFIRMED ARCHITECTURE DECISION`: Optional capabilities，包括 Deep Diagnostic，必須顯式宣告 availability state。
- `CONFIRMED ARCHITECTURE DECISION`: SRS / pretensioner diagnostics 是 profile-specific、read-only，優先 on-demand。
- `CONFIRMED ARCHITECTURE DECISION`: Profile file/storage format 未決定；BrandAdapter implementation form 亦待 implementation evidence。

### Deep Diagnostic status

SRS / pretensioner resistance、circuit、status、ABS/ACC extended values 與 ECU-specific identification 可作 normalized optional capability examples。Kamiq exact ECU route、DID、scaling、physical interpretation、availability 與 polling evidence 全部維持 `Pending / Unknown`；不得發明 DID 或由 resistance 單獨推論車輛歷史。
