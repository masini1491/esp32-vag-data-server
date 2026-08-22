# vehicle_coverage

Upstream: [CanBusHack/vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage)  
Reviewed: 2026-08-22  
Role: Kamiq 2024 capability/signal coverage  
Evidence class: `UPSTREAM_COVERAGE_CONFIRMED`, `REFERENCE_ONLY`  
Reuse status: no raw list/database redistribution。  
License / provenance: no confirmed root LICENSE；coverage files contain CanBusHack copyright notice。

## Confirmed evidence

存在 `yaml_lists/skoda/kamiq/skoda__kamiq__2024.txt`，標示 Platform: Skoda Kamiq (2024)，並列出 belt tensioner Ohm、airbag/igniter、speed、wheel speed、engine/oil temperature、ACC、gear、steering、brake、seatbelt、battery 等 signal descriptions。

## Transferable patterns

Vehicle-specific coverage 可用於研究 backlog 與 capability candidates。

## Project-specific implications

這只確認 signal-description coverage；exact J234 DID、raw encoding、scaling、session、SFD/gateway、passive CAN ID/bit 與 OBD pins visibility 全部 Pending。`VEHICLE_CONFIRMED = none`。

## Do not infer / do not reuse

不得 copy entire list、raw database 或將 upstream coverage 寫成 Hardware/Vehicle PASS。

## Revisit trigger

Kamiq 實車 validation 或合法 data provenance 需要具體 mapping 時。
