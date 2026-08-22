# suzuki-sds-obdii-gateway

Upstream: [sunsided/suzuki-sds-obdii-gateway](https://github.com/sunsided/suzuki-sds-obdii-gateway)
Reviewed: 2026-08-22
Role: future K-Line/CAN gateway and host-test architecture pattern
Evidence class: `REFERENCE_PATTERN`, `TEST_ARCHITECTURE_REFERENCE`
Reuse status: reference only；no source reuse。
License / provenance: manifest declares MIT OR Apache-2.0；future reuse前需重查。

## Confirmed evidence

Architecture is Suzuki ECU → K-Line/SDS/ISO 14230 → ESP32-C3 → CAN/OBD-II → external client；包含 codec/session、OBD-II server、host-testable protocol separation與 polling/cache pattern。README states bench complete but bike validation absent。

## Transferable patterns

Vehicle polling → cache → client request，不由 client refresh 直接驅動 vehicle polling。

## Project-specific implications

只作 future link/cache architecture reference；不升級為 vehicle evidence。

## Do not infer / do not reuse

不得宣稱 Suzuki vehicle support、Hardware PASS 或直接複製 code。

## Revisit trigger

Future K-Line gateway design 或實車 evidence contradict local summary 時。
