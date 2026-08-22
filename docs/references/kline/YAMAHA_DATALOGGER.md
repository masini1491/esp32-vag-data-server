# Yamaha-DataLogger

Upstream: [terrafirma2021/Yamaha-DataLogger](https://github.com/terrafirma2021/Yamaha-DataLogger)
Reviewed: 2026-08-22
Role: future ESP32-S3 + motorcycle K-Line architecture pattern
Evidence class: `ARCHITECTURE_REFERENCE`, `REFERENCE_ONLY`
Reuse status: no source reuse。
License / provenance: no confirmed repository LICENSE。

## Confirmed evidence

Upstream claims/design include ESP32-S3、Yamaha K-Line、L9637D、BLE、ELM327-style gateway、RPM/speed/error/coolant/gear、ECU emulator；source inspection shows Arduino ESP32、Serial1、Yamaha-specific framing與fixed pins。

## Transferable patterns

ESP32-S3 + K-Line PHY + normalization/gateway as demonstrated architecture pattern。

## Project-specific implications

不複製 fixed pins 或 Yamaha framing；Yamaha support remains `NOT SUPPORTED`。

## Do not infer / do not reuse

不得 generalize to all Yamaha vehicles或宣稱 motorcycle support。

## Revisit trigger

實際 motorcycle target 與 fresh license/provenance review 出現時。
