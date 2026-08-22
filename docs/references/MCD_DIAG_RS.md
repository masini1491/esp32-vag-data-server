# mcd-diag-rs

Repository: [bri3d/mcd-diag-rs](https://github.com/bri3d/mcd-diag-rs)

分類：`DATA_PIPELINE_REFERENCE`、`PROFILE_BUILDER_REFERENCE`

- `UPSTREAM_CLAIM`: Volkswagen ASAM-MCD diagnostic data 可 flatten 為 readable / compact JSON / Postcard-style representation。
- `REFERENCE_PATTERN`: legitimately available definitions → PC profile builder → flattened signal definitions → trimmed vehicle profile → ESP32；runtime 不直接解析大型 MCD / ODX database。
- `CONFIRMED`: 可抽象欄位包括 diagnostic identifier、name、conversion/scaling、unit、metadata；對應 `tools/mcd_converter/`、`tools/profile_builder/`、`data/profiles/`、`data/signals/`。
- `DO_NOT_REUSE`: proprietary VW/ODIS/MCD Projects data 或來源不合法的 diagnostic database。

License metadata 記錄為 BSD-3-Clause；實際重用前仍須確認 provenance。Kamiq 2024 exact compatibility 與 2022+ SFD/certificate restrictions 均 `Pending`。
