# OpenHaldex-S3

Upstream: [meatro/OpenHaldex-S3](https://github.com/meatro/OpenHaldex-S3)  
Reviewed: 2026-08-22  
Role: ESP32-S3 CAN/TWAI runtime、VAG UDS/ISO-TP patterns  
Evidence class: `ARCHITECTURE_REFERENCE`, `ALGORITHM_REFERENCE`  
Reuse status: MIT；selective reference only。  
License / provenance: MIT confirmed。

## Confirmed evidence

ESP32-S3、Arduino Framework、ESP-IDF TWAI、FreeRTOS、queues/mutex、timeouts、response routing、NRC `0x78`、UDS `0x22`/`0x19`、ISO-TP multi-frame/Flow Control、11/29-bit routes 與 KWP/TP2.0-related implementation。

## Transferable patterns

CAN runtime integration、queue/mutex/timeout reasoning、response matching 與 error handling。

## Project-specific implications

只作 runtime pattern reference；本專案仍使用 `Board Profile → HardwareConfig → CanHal`，且 upper layers 不依賴 TWAI types。

## Do not infer / do not reuse

不得重用 Haldex active control、signal modification、actuator/write paths、fixed pins/routes 或 `0x14 ClearDiagnosticInformation`。

## Revisit trigger

ESP32 TWAI/diagnostic runtime implementation 需要比較具體 recovery/error pattern 時。
