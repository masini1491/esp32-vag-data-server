# OBDII-ESP32

Upstream: [PouzarA/OBDII-ESP32](https://github.com/PouzarA/OBDII-ESP32)  
Reviewed: 2026-08-22  
Role: Phase 2 ISO-TP / Phase 3 OBD-II / host tests  
Evidence class: `ALGORITHM_REFERENCE`, `TEST_ARCHITECTURE_REFERENCE`  
Reuse status: MIT；selective reference only，勿複製 TWAI-coupled upper API。  
License / provenance: MIT confirmed from repository LICENSE。

## Confirmed evidence

Arduino/ESP32、ESP-IDF TWAI、ISO-TP / ISO 15765-2、OBD-II、FreeRTOS、PC `unit_tests_pc`、mocks、tests、CMake host structure；涵蓋 SF/FF/CF/FC、sequence、timeouts、buffer bounds、CAN failures、BUS_OFF 與 logging patterns。

## Transferable patterns

Deterministic host tests、frame validation、timeout/status handling 與 mock hardware boundary。

## Project-specific implications

本專案依 `CanHal`、`CanFrame`、`Clock`；Architecture Freeze v0.4 不允許 TWAI/CAN-specific types 洩漏至 diagnostic/application layers。

## Do not infer / do not reuse

不得重用 OBD Mode 04 Clear DTC 或 arbitrary diagnostic TX。Upstream 的 TWAI coupling 不代表本專案 API 應照搬。

## Revisit trigger

Phase 2 開始 ISO-TP implementation 或重新考慮 source reuse 時，重新確認 API 與 license。
