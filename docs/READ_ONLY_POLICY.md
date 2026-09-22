# Read-only Diagnostic Policy

本專案從 architecture 層級維持 read-only。未來僅可在確認必要性後使用讀取服務，例如 `0x10 DiagnosticSessionControl`（只限讀取所需 session transition）、`0x19 ReadDTCInformation`、`0x22 ReadDataByIdentifier`、`0x3E TesterPresent`。

明確排除：ECU Reset、Clear DTC、Security Access、WriteDataByIdentifier、InputOutputControl、RoutineControl / Basic Settings、Coding、Adaptation、Output Tests、Flashing / Download、actuator control。

## Enforcement requirements

- Future backend 必須有真正的 `ReadOnlyGuard`，不能只隱藏 UI。
- Web、BLE、API 不得提供 arbitrary raw UDS TX bypass。
- 任何 diagnostic TX path 必須在送出前經 guard，並以 allowlist 驗證 service 與 operation。
- Phase 3A 已建立最小 `ReadOnlyGuard` outbound enforcement：只接受 semantic 的 OBD Mode `0x01` 或 `0x09` single-PID request，並由 guard 內部建立兩位元組 payload。所有其他 mode、multi-PID 或 malformed request 都 fail-closed，且不呼叫下層 transport TX。
- 此 safety gate 不代表 OBD-II response parser、UDS、DTC service semantics、VAG routing 或 application-facing diagnostic TX 已實作。
- Phase 3B Generic OBD-II host service 只可經 `ReadOnlyGuard` 發出 Mode `0x01` single-PID request、supported-PID block request，或固定的 Mode `0x09` PID `0x02` VIN request。它不提供 arbitrary raw payload、DTC mode、UDS 或 application-facing diagnostic TX。
- Phase 4A `ReadOnlyGuard` safety extension 只允許 semantic UDS `ReadDataByIdentifier (0x22)` single-DID request，並由 guard 內部建立 `22 DID_hi DID_lo`。此 extension 不包含 UDS response parser、NRC、session lifecycle、DTC、TesterPresent、VAG DID 或 scaling semantics。
- Phase 4B Generic UDS host service 只可經上述 guard 執行 one-request-at-a-time 的 `0x22` single-DID read，並處理 `0x62 DID <raw data>`、terminal `7F 22 NRC` 與 bounded `7F 22 78` response-pending。它不新增其他 UDS service、session／DTC／TesterPresent、VAG DID/scaling 或 application-facing diagnostic TX。
