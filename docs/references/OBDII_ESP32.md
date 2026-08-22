# OBDII-ESP32

Repository: [PouzarA/OBDII-ESP32](https://github.com/PouzarA/OBDII-ESP32)

分類：`ARCHITECTURE_REFERENCE`、`ALGORITHM_REFERENCE`、`TEST_ARCHITECTURE_REFERENCE`

- `CONFIRMED`: Arduino Framework、ESP32 TWAI、ISO-TP / ISO 15765、OBD-II、Wi-Fi AP、Web dashboard/WebSocket、FreeRTOS 與 MIT License。
- `CONFIRMED`: `src/isotp/isotp.c/.h` 與 `src/core/obd2*.c/.h` 分離 ISO-TP、OBD-II、PID/diagnostic logic。
- `REFERENCE_PATTERN`: Core 0 network、Core 1 OBD/ISO-TP/CAN，跨 task/core 使用 FreeRTOS queue；可參考 SF/FF/CF/FC、sequence validation、timeouts、ECU discovery `0x7DF`、PID/VIN/DTC parsing、NRC `0x78` 與 BUS_OFF recovery。
- `REFERENCE_PATTERN`: host-side mock hardware layer 測試 TWAI、timing、queues、BUS_OFF、alerts、timeouts、multi-frame、SN wrap、buffer protection、multiple ECU responses 與 parsing。
- `DO_NOT_REUSE`: OBD Mode 04 Clear DTC 與繞過 `ReadOnlyGuard` 的 arbitrary diagnostic TX。

對應本專案 Phase 2、3；未來建議 `real protocol source → mock HAL → PC unit tests`。GPIO 仍須遵守 Board Profile → HardwareConfig → HAL。
