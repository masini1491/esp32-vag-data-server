# mcd-diag-rs

Upstream: [bri3d/mcd-diag-rs](https://github.com/bri3d/mcd-diag-rs)
Reviewed: 2026-08-22
Role: Phase 6 profile/data pipeline
Evidence class: `DATA_PIPELINE_REFERENCE`, `PROFILE_BUILDER_REFERENCE`
Reuse status: BSD-3-Clause reference；不分發 proprietary data。
License / provenance: BSD-3-Clause confirmed from LICENSE.md。

## Confirmed evidence

VW ASAM-MCD definitions 可 flatten 成 readable JSON / compact Postcard-style records，包含 identifier、name、conversion/scaling、unit、metadata。Upstream 表示 VW37/AU37 MQB-era testing；TP2.0 experimental/unit-tested，非 real-car tested；K-Line、DoIP、CAN-FD 與 SFD/certificate login 不作現代相容性保證。

## Transferable patterns

Legitimately available definitions → PC converter/profile builder → flattened data → trimmed vehicle profile → ESP32 runtime。

## Project-specific implications

對應 `tools/mcd_converter/`、`tools/profile_builder/` 與 profile data pipeline；Kamiq 2024 compatibility 未確認。

## Do not infer / do not reuse

不得提交 proprietary VW MCD Projects、ODIS databases 或非法取得 datasets。

## Revisit trigger

Phase 6 profile builder implementation 或實際合法 data reuse 前。
