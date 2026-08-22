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
