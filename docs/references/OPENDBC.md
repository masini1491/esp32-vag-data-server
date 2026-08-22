# opendbc

Repository: [commaai/opendbc](https://github.com/commaai/opendbc)

分類：`DATA_REFERENCE`、`PASSIVE_CAN_REFERENCE`

- `CONFIRMED`: 可作 VAG PQ、MQB、MQB Evo、MEB 等 passive CAN signal definition 的研究入口。
- `REFERENCE_PATTERN`: 對應 `src/passive_can/`、`src/profiles/`、`data/signals/`、`data/dbc/`；可研究 speed、wheel speed、ACC、steering、engine、airbag/seatbelt、brake、gateway-related signals。
- `UPSTREAM_COVERAGE_CONFIRMED`: upstream 存在某個 MQB signal/definition，不等於 Kamiq 2024 的 CAN ID、bit、scaling 已確認。
- `VEHICLE_HYPOTHESIS`: 某些 definition 可能適用 Kamiq / MQB-A0。
- `VEHICLE_CONFIRMED`: 本專案目前沒有任何項目可標記。
- `DO_NOT_REUSE`: 本輪不複製 DBC；任何個別 file 的 license/provenance 必須在實際 reuse 前重新確認。
