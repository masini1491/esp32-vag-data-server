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
