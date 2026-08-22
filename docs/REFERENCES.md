# Upstream Reference Index

本頁是索引；詳細工程摘要位於 [`docs/references/`](references/)。本 repository 不複製第三方 source code、raw database 或 proprietary data。

| Reference | Primary use | Phase | Evidence type | Code/data reuse status | Detailed note |
|---|---|---|---|---|---|
| [OpenHaldex-S3](https://github.com/meatro/OpenHaldex-S3) | CAN / VAG UDS runtime | 1, 4 | architecture / algorithm | Compatible-license reference; selective reuse only | [OPENHALDEX_S3.md](references/OPENHALDEX_S3.md) |
| [OBDII-ESP32](https://github.com/PouzarA/OBDII-ESP32) | ISO-TP / OBD-II / tests | 2, 3 | architecture / algorithm / test | MIT; selective reuse only | [OBDII_ESP32.md](references/OBDII_ESP32.md) |
| [mcd-diag-rs](https://github.com/bri3d/mcd-diag-rs) | Profile builder / data pipeline | 6 / tools | data pipeline | No proprietary DB redistribution | [MCD_DIAG_RS.md](references/MCD_DIAG_RS.md) |
| [opendbc](https://github.com/commaai/opendbc) | Passive CAN | 9 | data reference | Verify individual provenance before reuse | [OPENDBC.md](references/OPENDBC.md) |
| [PyVCDS](https://github.com/baconwaifu/PyVCDS) | Historical VCDS workflow | Research | historical / method | Reference only; no source reuse | [PYVCDS.md](references/PYVCDS.md) |
| [MQB-sniffer](https://github.com/mrfixpl/MQB-sniffer) | UDS sniff methodology | 6 / research | research method | No source reuse | [MQB_SNIFFER.md](references/MQB_SNIFFER.md) |
| [vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage) | Kamiq 2024 capability/signal coverage | 6, 9 / research | upstream coverage | No raw data reuse | [VEHICLE_COVERAGE.md](references/VEHICLE_COVERAGE.md) |

## Evidence labels

`CONFIRMED` = directly checked upstream source/metadata/documentation；`UPSTREAM_CLAIM` = upstream author claim not independently verified；`REFERENCE_PATTERN` = transferable pattern；`UPSTREAM_COVERAGE_CONFIRMED` = upstream lists vehicle/signal description but not exact mapping；`VEHICLE_HYPOTHESIS` = possible Kamiq applicability；`VEHICLE_CONFIRMED` = reserved for future real-vehicle evidence；`DO_NOT_REUSE` = prohibited by safety, license or provenance policy。
