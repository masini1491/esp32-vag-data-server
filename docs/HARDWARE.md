# Hardware Notes

- Platform family: ESP32-S3
- Exact board: TBD
- External or onboard Classic CAN transceiver: TBD
- GPIO: TBD；由 Board Profile / HardwareConfig / HAL 管理
- Current hardware scope: ESP32-S3 + Classic CAN transceiver
- ESP32-S3 TWAI scope: Classic CAN
- Future CAN-FD / Ethernet transport: not part of current hardware scope

## Expected OBD wiring

- Pin 6: CAN-H
- Pin 14: CAN-L
- Pin 4/5: ground

固定安裝時預計可由附近 ACC/IGN switched fuse 供電，不要求使用 OBD Pin 16。Vehicle CAN 不額外加入 120Ω termination。

正式汽車電源設計需考慮 fuse、reverse polarity protection、transient/TVS protection 與 automotive-suitable regulation。

以上均非實車 wiring 驗證結果；hardware evidence 維持 Pending。

若未來品牌或車型需要 CAN-FD / Ethernet diagnostics，必須重新進行 hardware capability assessment；本輪不選定 external CAN-FD controller，也不實作 CAN-FD、HSFZ 或 DoIP。
