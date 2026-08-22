# Read-only Diagnostic Policy

本專案從 architecture 層級維持 read-only。未來僅可在確認必要性後使用讀取服務，例如 `0x10 DiagnosticSessionControl`（只限讀取所需 session transition）、`0x19 ReadDTCInformation`、`0x22 ReadDataByIdentifier`、`0x3E TesterPresent`。

明確排除：ECU Reset、Clear DTC、Security Access、WriteDataByIdentifier、InputOutputControl、RoutineControl / Basic Settings、Coding、Adaptation、Output Tests、Flashing / Download、actuator control。

## Enforcement requirements

- Future backend 必須有真正的 `ReadOnlyGuard`，不能只隱藏 UI。
- Web、BLE、API 不得提供 arbitrary raw UDS TX bypass。
- 任何 diagnostic TX path 必須在送出前經 guard，並以 allowlist 驗證 service 與 operation。
- 本輪不建立實作；此文件是後續實作的約束。
