# OpenHaldex-S3

Repository: [meatro/OpenHaldex-S3](https://github.com/meatro/OpenHaldex-S3)

分類：`ARCHITECTURE_REFERENCE`、`ALGORITHM_REFERENCE`

## Evidence

- `CONFIRMED`: ESP32-S3、Arduino Framework、PlatformIO `framework = arduino`、ESP-IDF TWAI / FreeRTOS API 與 MIT License。
- `CONFIRMED`: CAN abstraction、TWAI initialization、RX/TX、alerts、bus recovery；`src/functions/diag/uds.cpp` 涵蓋 UDS `0x22`、`0x19`、negative response `0x7F`、NRC `0x78`、ISO-TP single/multi-frame、Flow Control、11/29-bit routes、queues、mutex、timeout/result handling 與 KWP/TP2.0。
- `REFERENCE_PATTERN`: 可參考 CAN → TWAI、diagnostic queue/mutex/timeout/response routing/NRC handling，對應本專案 Phase 1、4。
- `DO_NOT_REUSE`: Haldex drivetrain control、signal modification、lock/control maps、actuator behavior、`0x14 Clear DTC` 與任何 write/control path。

本專案仍須維持 `Board Profile → HardwareConfig → HAL`，不得照搬固定 pins architecture。OpenHaldex-S3 是主動 controller，本專案是 read-only data server，安全邊界不同。
