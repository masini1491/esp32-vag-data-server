# Upstream Evidence Synthesis

Reviewed: 2026-08-22

本文件是 Phase-specific reference reading 的第一入口。它是本 repository 的工程摘要，不是第三方 source 或 database；除非符合 Revisit trigger，不要重新 discovery upstream。

## Phase 2 — ISO-TP

Primary references: [OBDII-ESP32](can/OBDII_ESP32.md)、[OpenHaldex-S3](can/OPENHALDEX_S3.md)。

可參考 SF/FF/CF/FC、payload/buffer bounds、sequence、Flow Control、Block Size、STmin、timeout、response matching、malformed frames、11/29-bit considerations 與 deterministic `MockCan` / `FakeClock` tests。Phase 2 應依賴本專案 `CanHal`、`CanFrame`、`Clock`，不可讓 `twai_message_t` 或其他 ESP-IDF type 洩漏至上層；不要直接複製 upstream task/queue architecture。

## OBD-II / UDS

Read-only concepts 可參考 OBD PID/VIN/DTC read、UDS `0x22`、`0x19`、`0x3E`、必要的 `0x10`、negative response `0x7F` 與 NRC `0x78`。禁止 Mode 04、`0x14`、ECU Reset、SecurityAccess、WriteDataByIdentifier、IO Control、RoutineControl、coding、adaptation、flashing、output tests 與 actuator control。所有 active diagnostic TX 必須經 `ReadOnlyGuard`。

## VAG / Profile data

[mcd-diag-rs](vag/MCD_DIAG_RS.md) 提供 PC preprocessing/flattening pattern；不得 redistribute proprietary MCD/ODIS data。[opendbc](vag/OPENDBC.md) 只作 passive CAN reference；[MQB-sniffer](vag/MQB_SNIFFER.md) 只作 research methodology；[vehicle_coverage](vag/VEHICLE_COVERAGE.md) 只確認 Kamiq 2024 signal-description coverage；[PyVCDS](vag/PYVCDS.md) 是 historical workflow。`VEHICLE_CONFIRMED = none`。

### Current Kamiq validation backlog

以下全部是 `Pending / hypothesis`，不是 Hardware/Vehicle PASS：

- 2024 Kamiq OBD Gateway 可存取的 ECU 清單
- OBD pins 6/14 是否可看到所需 passive CAN traffic
- 實車 CAN bitrate / routes
- J234 Airbag read-only measuring values accessibility
- pretensioner resistance exact DID / scaling
- ACC target/set-speed source、cluster displayed speed source、individual wheel-speed source
- 2024 facelift 與既有 MQB-A0 definitions 的差異
- SFD / gateway restrictions 對 read-only measurement values 的實際影響

## Future K-Line

K-Line is `NOT SUPPORTED`。可參考 [OBD9141](kline/OBD9141.md)、[Keyword-Protocol-2000](kline/KEYWORD_PROTOCOL_2000.md)、[sv650overlay](kline/SV650OVERLAY.md)、[Yamaha-DataLogger](kline/YAMAHA_DATALOGGER.md)、[suzuki-sds-obdii-gateway](kline/SUZUKI_SDS_OBDII_GATEWAY.md) 與 [MotoLink](kline/MOTOLINK.md) 的 link portability / testing patterns；不要新增 K-Line implementation phase，不改 `Kamiq → T-Roc → RAV4 → Wish`。

## Revisit triggers

優先使用 local summaries。只有在以下情況才重新研究 external source：考慮實際 source reuse 需重新確認 license/provenance、upstream materially changed、實車 evidence 與摘要矛盾、新 brand/model 成為實際 target，或 local evidence 不足以回答具體 implementation decision。
