# Upstream Reference Catalog

本輪只建立 catalog，不複製第三方 source code、proprietary database 或受限制資料。

| Repository | 用途 | Reference type | Notes |
|---|---|---|---|
| [meatro/OpenHaldex-S3](https://github.com/meatro/OpenHaldex-S3) | ESP32-S3、VAG UDS、KWP/TP2.0、CAN、Web/API、FreeRTOS | ARCHITECTURE_REFERENCE / ALGORITHM_REFERENCE | 不引入 Haldex control/write；license 使用前確認 |
| [PouzarA/OBDII-ESP32](https://github.com/PouzarA/OBDII-ESP32) | TWAI、ISO-TP、OBD-II、FreeRTOS、WebSocket、unit tests | ARCHITECTURE_REFERENCE / ALGORITHM_REFERENCE | 僅架構/演算法參考；license 使用前確認 |
| [bri3d/mcd-diag-rs](https://github.com/bri3d/mcd-diag-rs) | ASAM-MCD data pipeline、DID、measurement definition | DATA_PIPELINE_REFERENCE | 不加入來源不明或受限制 VW/ODIS database |
| [commaai/opendbc](https://github.com/commaai/opendbc) | VAG PQ / MQB / MQB Evo / MEB passive CAN definitions | DATA_REFERENCE | 逐項確認 license/provenance |
| [baconwaifu/PyVCDS](https://github.com/baconwaifu/PyVCDS) | ECU enumeration、measurement block、tracing | ARCHITECTURE_REFERENCE / HISTORICAL_REFERENCE | archived、偏舊 TP2.0/KWP |
| [mrfixpl/MQB-sniffer](https://github.com/mrfixpl/MQB-sniffer) | MQB UDS sniff / DID research workflow | RESEARCH_METHOD_REFERENCE | 僅研究方法 |
| [CanBusHack/vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage) | VAG measurement-value / signal coverage | DATA_REFERENCE | 使用前確認 provenance |
| [xerootg/esp32_tp20_datalogger](https://github.com/xerootg/esp32_tp20_datalogger) | ESP32 + VW TP2.0 | HISTORICAL_PROTOCOL_REFERENCE | 舊 PQ platform 參考 |
| [TUMFTM/uds-decoder](https://github.com/TUMFTM/uds-decoder) | UDS decoding | ALGORITHM_REFERENCE | 僅演算法參考 |
| [MagnusThome/esp32_obd2](https://github.com/MagnusThome/esp32_obd2) | Arduino ESP32 OBD-II API / PID | ALGORITHM_REFERENCE | 僅 API/PID 參考 |

所有 reference 的 license/provenance 在實際引用前必須再次確認。
