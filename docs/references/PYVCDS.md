# PyVCDS

Repository: [baconwaifu/PyVCDS](https://github.com/baconwaifu/PyVCDS)

分類：`HISTORICAL_REFERENCE`、`RESEARCH_METHOD_REFERENCE`、`ARCHITECTURE_REFERENCE`

- `CONFIRMED`: archived；偏舊 VW（約 2007 年代），涉及 VWTP 2.0 / TP20、KWP2000、measuring blocks、ECU enumeration/identification、transaction tracing 與 socketCAN。
- `REFERENCE_PATTERN`: VIN/vehicle context → enumerate modules → cache module list → module identification → measurement read；可作 scan-tool black-box reverse-engineering SOP 的歷史參考。
- `DO_NOT_REUSE`: `_vw/flash.py`、`_vw/keys.py`、`_vw/sa2.py` 與任何 security access / flashing source；不作 modern Kamiq 2024 UDS implementation source。
- `CONFIRMED`: 不包含 Ross-Tech label database、不支援 CLB decrypt；本專案不得加入 Ross-Tech proprietary label data。

GitHub metadata 無標準 SPDX 判定；LICENSE file 為 GPLv2+ placeholder wording。因此本專案視為 `REFERENCE ONLY`，不直接搬 source code。
