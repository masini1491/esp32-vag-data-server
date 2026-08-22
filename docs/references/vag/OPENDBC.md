# opendbc

Upstream: [commaai/opendbc](https://github.com/commaai/opendbc)  
Reviewed: 2026-08-22  
Role: Phase 9 passive CAN / VAG data reference  
Evidence class: `DATA_REFERENCE`, `PASSIVE_CAN_REFERENCE`, `VEHICLE_HYPOTHESIS`  
Reuse status: MIT root reference；individual data provenance/applicability 仍需確認。  
License / provenance: root MIT confirmed；data applicability not automatically established。

## Confirmed evidence

包含 `vw_mqb.dbc`、`vw_mqbevo.dbc` 與 VAG passive CAN definitions，可研究 speed、wheel speed、ACC、steering、engine、airbag/seatbelt、brake、gateway categories。

## Transferable patterns

Signal category discovery 與 passive decoder research。

## Project-specific implications

MQB signal existence 不等於 Kamiq 2024 CAN ID、bit、scaling、OBD visibility 或 vehicle compatibility。

## Do not infer / do not reuse

本 repository 不複製 DBC；未經實車 validation 不標 `VEHICLE_CONFIRMED`。

## Revisit trigger

Phase 9 passive CAN profile implementation 或個別 data reuse 前。
