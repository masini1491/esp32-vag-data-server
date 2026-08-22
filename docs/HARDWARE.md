# Hardware Notes

- Platform family: ESP32-S3
- Exact board: TBD
- External or onboard Classic CAN transceiver: TBD
- GPIO: TBD；由 Board Profile / HardwareConfig / HAL 管理
- Current hardware scope: ESP32-S3 + Classic CAN transceiver
- ESP32-S3 TWAI scope: Classic CAN
- Future CAN-FD / Ethernet transport: not part of current hardware scope
- Future K-Line PHY: extension boundary only；目前沒有 implementation 或 transceiver 選型

## Expected OBD wiring

- Pin 6: CAN-H
- Pin 14: CAN-L
- Pin 4/5: ground

固定安裝可視 vehicle/install evidence 使用 OBD/J1962 Pin 16 vehicle power、附近 ACC/IGN switched fuse，或 future brand-adapter supplied vehicle power。External fuse-tap 不是 architecture 強制前提；品牌診斷接頭是否供電及其電壓皆為 vehicle/model/adapter dependent。Vehicle CAN 不額外加入 120Ω termination。

正式汽車電源設計需考慮 fuse、reverse polarity protection、transient/TVS protection 與 automotive-suitable regulation。

Vehicle-side power 必須經適當 automotive input protection / voltage conversion 後才供 ESP32 使用；具體 buck converter、TVS、PTC、fuse、MOSFET 或 reverse-polarity controller 尚未選定。

以上均非實車 wiring 驗證結果；hardware evidence 維持 Pending。

若未來品牌或車型需要 CAN-FD / Ethernet diagnostics，必須重新進行 hardware capability assessment；本輪不選定 external CAN-FD controller，也不實作 CAN-FD、HSFZ 或 DoIP。

同理，future K-Line 若有實際 evidence，應新增平行的 UART + K-Line PHY hardware/link backend；本輪不選 K-Line transceiver、不畫 schematic，Hardware validation 維持 Pending。
