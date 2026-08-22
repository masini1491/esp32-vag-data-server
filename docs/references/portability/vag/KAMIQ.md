# Škoda Kamiq

Reviewed: 2026-08-22
Role: Primary vehicle validation target
Evidence class: `UPSTREAM_COVERAGE_CONFIRMED`, `VEHICLE_HYPOTHESIS`
Reuse status: `REFERENCE_ONLY` / cross-source synthesis

## Confirmed upstream evidence

[opendbc](https://github.com/commaai/opendbc) represents `SKODA_KAMIQ_MK1` through `VolkswagenMQBPlatformConfig` with its own platform/chassis identification and vehicle metadata. This supports `Generic Core → VAG Brand Layer → VAG Profile Set → Kamiq Vehicle Profile`, but does not establish 2024 facelift compatibility, exact CAN IDs, ECU routes, DIDs, scaling, OBD visibility or SFD behavior.

The existing canonical [vehicle_coverage note](../../vag/VEHICLE_COVERAGE.md) records a CanBusHack Kamiq 2024 coverage entry with signal-description categories including speed, engine, wheel speed, steering, gear/selector, ACC, seatbelt, airbag, pretensioner and electrical measurements. This is `UPSTREAM_COVERAGE_CONFIRMED` only; raw lists are not copied here.

[mcd-diag-rs](../../vag/MCD_DIAG_RS.md) supplies a future PC-side flatten/profile-builder pattern. [MQB-sniffer](../../vag/MQB_SNIFFER.md) supplies read-only trace methodology; Golf-specific addresses, DIDs, routes and values are not Kamiq evidence.

## Architecture / validation implication

Kamiq is the project’s first real-vehicle target and should establish the first Vehicle PASS for:

```text
Classic CAN / TWAI → ISO-TP → Generic OBD-II / UDS
→ VAG Brand Layer → Kamiq Vehicle Profile → normalized VehicleData
```

Validation should cover physical CAN connectivity、actual bitrate、ISO-TP behavior、read-only routing、ECU identification、Profile Resolver、VehicleData mapping、ReadOnlyGuard 與 applicable OBD/gateway/passive CAN visibility。T-Roc only becomes meaningful after this first target is stable.

## Do not infer

`VEHICLE_CONFIRMED = none`。Do not claim Kamiq support, Hardware PASS or Vehicle PASS. Do not infer exact ECU/DID/CAN mappings, diagnostic session, scaling, SFD/gateway access or OBD pin visibility. A readable pretensioner resistance value does not prove original component、undeployed state、no prior repair or absence of a simulator/resistor。

All diagnostic work remains read-only: no Clear DTC、coding、adaptation、SecurityAccess、RoutineControl、output test、flashing or actuator control。

## Future validation backlog

All items remain `Pending` until physical vehicle evidence exists:

- exact 2024 facelift identity/profile evidence and differences from MQB-A0 evidence
- OBD Gateway accessible ECU list、required diagnostic routes、VIN/ECU identification、firmware fingerprints
- actual CAN bitrate and OBD pins 6/14 passive visibility
- J234 measuring-value accessibility、pretensioner resistance DID/scaling、SFD/gateway restrictions
- actual sources for VIN、vehicle speed、cluster/displayed speed、RPM、coolant/oil temperature、voltage、gear、steering angle、ACC active/set speed and individual wheel speeds

Minimum first Vehicle PASS dataset: VIN、`vehicle.speed`、engine RPM、coolant temperature、vehicle/system voltage。Oil temperature、gear、wheel speeds、steering angle、ACC and Deep Diagnostic values are optional/high-value follow-ups; pretensioner/airbag/extended DTC reads remain on-demand and must not become high-rate polling.

## Revisit trigger

Revisit local VAG notes only for a concrete Kamiq implementation question. Add profile data only after exact vehicle identity and physical read-only evidence are available; until then, use `Pending` / `Unknown` and do not repeatedly rediscover upstream.
