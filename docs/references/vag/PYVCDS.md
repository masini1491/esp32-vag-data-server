# PyVCDS

Upstream: [baconwaifu/PyVCDS](https://github.com/baconwaifu/PyVCDS)  
Reviewed: 2026-08-22  
Role: historical VW diagnostic research workflow  
Evidence class: `HISTORICAL_REFERENCE`, `REFERENCE_ONLY`  
Reuse status: no source reuse。  
License / provenance: metadata/SPDX 不明，placeholder GPLv2+ wording；需重新確認。

## Confirmed evidence

Archived project，涉及較舊 VW、VWTP 2.0/TP20、KWP2000、measuring blocks、ECU enumeration/identification、transaction tracing、SocketCAN。

## Transferable patterns

vehicle/VIN context → enumerate modules → cache module list → identify ECU → read measurement。

## Project-specific implications

只作 historical research method，不作 modern Kamiq 2024 implementation source。

## Do not infer / do not reuse

不得重用 flashing、keys、security access、active/write behavior 或 Ross-Tech proprietary label data。

## Revisit trigger

只在需要比較 historical workflow 或重新考慮 source provenance 時。
