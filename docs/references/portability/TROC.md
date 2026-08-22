# Volkswagen T-Roc

Reviewed: 2026-08-22  
Role: Phase 11 same-brand VAG portability validation target  
Evidence class: `UPSTREAM_COVERAGE_CONFIRMED`, `VEHICLE_HYPOTHESIS`  
Reuse status: reference only；no raw database or production profile。

## Confirmed upstream evidence

[opendbc](https://github.com/commaai/opendbc) defines `VOLKSWAGEN_TROC_MK1` as a `VolkswagenMQBPlatformConfig` for Volkswagen T-Roc 2018–23, with separate T-Roc identification metadata. It also defines `SKODA_KAMIQ_MK1` with its own Kamiq/MQB-A0 metadata. [CanBusHack/vehicle_coverage](https://github.com/CanBusHack/vehicle_coverage) has Volkswagen T-Roc annual coverage directories including a 2024 file identified as Volkswagen T-Roc (2024), with signal-description categories including accelerator/throttle, gear, ABS, TPMS, airbag/igniter and belt-tensioner resistance.

`OBDb/Volkswagen-T-Roc` is metadata/future-revisit evidence only: its reviewed `signalsets/v3/default.json` has an empty commands array and is not current signal-mapping evidence. License: CC BY-SA 4.0.

## Architecture / validation implication

T-Roc and Kamiq support the pattern of a shared Generic Core / VAG Brand Layer with separate Vehicle Profiles, identification, routing, capability and signal mapping. Phase 11 remains same-brand portability validation.

## Do not infer

Do not infer identical CAN IDs, DIDs, scaling, ECU sets, gateway behavior, passive CAN visibility or T-Roc support. Coverage is not `VEHICLE_CONFIRMED`; no raw coverage redistribution.

## Future validation backlog

Exact T-Roc year/trim/powertrain、VIN/ECU identification、routing、DID/scaling、OBD visibility、passive CAN mapping、capabilities 與 real-vehicle access remain `Pending`。

## Revisit trigger

T-Roc becomes the immediate hardware validation target, a real trace is available, or OBDb adds actual signal mappings.
