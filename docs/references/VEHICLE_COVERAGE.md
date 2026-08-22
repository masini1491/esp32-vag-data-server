# vehicle_coverage

Repository: [CanBusHack/vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage)

分類：`DATA_REFERENCE`、`VEHICLE_CAPABILITY_REFERENCE`

- `UPSTREAM_COVERAGE_CONFIRMED`: repository 中存在 `yaml_lists/skoda/kamiq/skoda__kamiq__2024.txt` 與 `dbc_lists/skoda/kamiq/skoda__kamiq__2024.txt`，並有 Kamiq 2020–2023 entries。
- `UPSTREAM_COVERAGE_CONFIRMED`: Kamiq 2024 signal descriptions 包含 driver/front-passenger belt tensioner Ohm Value，以及 speed/displayed speed、四輪 wheel speed、engine/oil temperature、ACC set speed、gear/PRNDL、steering、brake、seatbelt、acceleration、yaw、odometer、battery voltage 等類型。
- `VEHICLE_HYPOTHESIS`: 上述類型值得作為 Kamiq/HUD telemetry 研究方向；不代表 exact mapping。
- `Pending`: exact J234 DID、raw format、scaling、session、SFD/gateway behavior、CAN ID/bit/length、OBD pins 6/14 visibility 與 real-car validation。
- `DO_NOT_REUSE`: 不複製完整 list、raw coverage/database 或其他第三方資料；只保留本文件工程摘要與 attribution。

Repository metadata 沒有明確 license，coverage file 標示 Copyright CanBusHack；因此為 `REFERENCE ONLY`，不得在 Public Repository 直接 redistribute raw data。
