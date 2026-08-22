# Toyota RAV4

Reviewed: 2026-08-22  
Role: Phase 12 cross-brand Generic Core / Toyota portability validation target  
Evidence class: `UPSTREAM_COVERAGE_CONFIRMED`, `DATA_REFERENCE`, `VEHICLE_HYPOTHESIS`  
Reuse status: reference only；no Toyota code or raw signal database。

## Confirmed upstream evidence

[opendbc](https://github.com/commaai/opendbc) represents RAV4 across multiple generation, powertrain and security/profile configurations: 2016–18 New MC-style and TNGA-K variants、2019–21 TSS2、2022 TSS2/RADAR_ACC、2023–25 TSS2/RADAR_ACC/ANGLE_CONTROL，以及 RAV4 Prime ToyotaSecOC variants。Its Toyota diagnostic evidence includes multiple styles such as KWP and UDS routes; examples include hybrid-control variations around `0x7e2` KWP and `0x7d2` UDS, depending on platform.

[OBDb/Toyota-RAV4](https://github.com/OBDb/Toyota-RAV4) has a non-empty CC BY-SA 4.0 `signalsets/v3/default.json` with model/year-filtered definitions including service/mode `0x22`/`0x21` examples, fuel, wheel speed, vehicle speed, tire, ABS/brake/yaw measurements.

## Architecture / validation implication

Future Toyota Brand Layer must use Profile Resolver for generation/powertrain/security-specific profiles; it must not become one Toyota profile or one RAV4 profile. Phase 12 remains cross-brand portability validation.

## Do not infer

Do not infer that every Toyota ECU uses UDS, that ECU addressing is identical, that powertrain is irrelevant, or that upstream definitions establish RAV4 vehicle access. Do not copy active-control logic, raw signal databases or substantial CC BY-SA data. No RAV4 support is claimed.

## Future validation backlog

Exact RAV4 model year、powertrain、VIN/ECU identification、diagnostic routing、capabilities、passive CAN mapping、license/data attribution and real-vehicle access remain `Pending`。

## Revisit trigger

An exact RAV4 year/powertrain becomes the hardware target, real traces are collected, or actual mapping reuse is considered after fresh license review.
