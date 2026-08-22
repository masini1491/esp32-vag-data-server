# Upstream Reference Index

本頁是 canonical entry point。Start here: [docs/references/SYNTHESIS.md](references/SYNTHESIS.md)。Reference notes 是 local evidence cache；不複製第三方 source、raw database 或 proprietary data。

## CAN / ISO-TP / OBD

| Project | Role | Phase / relevance | Evidence status | Reuse / license | Detailed note |
|---|---|---|---|---|---|
| [OBDII-ESP32](https://github.com/PouzarA/OBDII-ESP32) | ISO-TP algorithm / host tests | Phase 2, 3 | `CONFIRMED` | MIT; selective reference | [note](references/can/OBDII_ESP32.md) |
| [OpenHaldex-S3](https://github.com/meatro/OpenHaldex-S3) | CAN/TWAI runtime / VAG patterns | Phase 1, 4 | `CONFIRMED` | MIT; selective reference | [note](references/can/OPENHALDEX_S3.md) |

## VAG / Data / Research

| Project | Role | Phase / relevance | Evidence status | Reuse / license | Detailed note |
|---|---|---|---|---|---|
| [mcd-diag-rs](https://github.com/bri3d/mcd-diag-rs) | Profile/data pipeline | Phase 6 / tools | `CONFIRMED` | BSD-3-Clause; no proprietary DB | [note](references/vag/MCD_DIAG_RS.md) |
| [opendbc](https://github.com/commaai/opendbc) | Passive CAN data reference | Phase 9 | `CONFIRMED` / `VEHICLE_HYPOTHESIS` | MIT root; check data provenance | [note](references/vag/OPENDBC.md) |
| [PyVCDS](https://github.com/baconwaifu/PyVCDS) | Historical VW workflow | Research | `REFERENCE_ONLY` | No source reuse; provenance unclear | [note](references/vag/PYVCDS.md) |
| [MQB-sniffer](https://github.com/mrfixpl/MQB-sniffer) | Read-only research method | Phase 6 / research | `REFERENCE_ONLY` | No source reuse; license unclear | [note](references/vag/MQB_SNIFFER.md) |
| [vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage) | Kamiq 2024 coverage | Phase 6, 9 | `UPSTREAM_COVERAGE_CONFIRMED` | No raw data; provenance restricted | [note](references/vag/VEHICLE_COVERAGE.md) |

## Future K-Line portability evidence

All entries below are `NOT SUPPORTED` / future architecture evidence only. No motorcycle support is claimed.

| Project | Role | Phase / relevance | Evidence status | Reuse / license | Detailed note |
|---|---|---|---|---|---|
| [OBD9141](https://github.com/iwanders/OBD9141) | ISO 9141/KWP link and simulator pattern | Future only | `REFERENCE_ONLY` | MIT; destructive behavior excluded | [note](references/kline/OBD9141.md) |
| [Keyword-Protocol-2000](https://github.com/aster94/Keyword-Protocol-2000) | K-Line/KWP architecture and emulator | Future only | `REFERENCE_ONLY` | GPLv3; no source reuse | [note](references/kline/KEYWORD_PROTOCOL_2000.md) |
| [Yamaha-DataLogger](https://github.com/terrafirma2021/Yamaha-DataLogger) | ESP32-S3/K-Line pattern | Future only | `REFERENCE_ONLY` | License unconfirmed; no source reuse | [note](references/kline/YAMAHA_DATALOGGER.md) |
| [sv650overlay](https://github.com/BananaJoh/sv650overlay) | Motorcycle K-Line/power hardware evidence | Future only | `REFERENCE_ONLY` | GPLv3; no source reuse | [note](references/kline/SV650OVERLAY.md) |
| [suzuki-sds-obdii-gateway](https://github.com/sunsided/suzuki-sds-obdii-gateway) | K-Line/CAN gateway and host tests | Future only | `REFERENCE_PATTERN` | MIT OR Apache-2.0 declared; recheck before reuse | [note](references/kline/SUZUKI_SDS_OBDII_GATEWAY.md) |
| [MotoLink](https://github.com/fpoussin/MotoLink) | Historical CAN/K-Line multi-link evidence | Future only | `REFERENCE_ONLY` | GPLv3; archived; no source reuse | [note](references/kline/MOTOLINK.md) |

KYMCO / SYM 沒有足夠成熟且可歸屬的 canonical evidence；不建立其 reference note，也不推論其 protocol。

## Recommended reading sets

### Phase 2 ISO-TP

Read only:

- [SYNTHESIS.md](references/SYNTHESIS.md)
- [OBDII_ESP32.md](references/can/OBDII_ESP32.md)
- [OPENHALDEX_S3.md](references/can/OPENHALDEX_S3.md)

### VAG/Profile implementation

Read [SYNTHESIS.md](references/SYNTHESIS.md) and only relevant files under [references/vag/](references/vag/).

### Future K-Line

Only when explicitly scoped: [SYNTHESIS.md](references/SYNTHESIS.md) and relevant files under [references/kline/](references/kline/).

## Evidence labels

`CONFIRMED`、`UPSTREAM_CLAIM`、`REFERENCE_PATTERN`、`UPSTREAM_COVERAGE_CONFIRMED`、`VEHICLE_HYPOTHESIS`、`VEHICLE_CONFIRMED`、`DO_NOT_REUSE`；reuse classifications include `ARCHITECTURE_REFERENCE`、`ALGORITHM_REFERENCE`、`TEST_ARCHITECTURE_REFERENCE`、`DATA_REFERENCE`、`DATA_PIPELINE_REFERENCE`、`PROFILE_BUILDER_REFERENCE`、`RESEARCH_METHOD_REFERENCE`、`HISTORICAL_REFERENCE`、`REFERENCE_ONLY`。

`VEHICLE_CONFIRMED = none`。Local summaries are preferred; external research requires a documented Revisit trigger.
