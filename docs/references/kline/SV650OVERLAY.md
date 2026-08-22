# sv650overlay

Upstream: [BananaJoh/sv650overlay](https://github.com/BananaJoh/sv650overlay)  
Reviewed: 2026-08-22  
Role: future motorcycle hardware/link evidence  
Evidence class: `REFERENCE_PATTERN`, `REFERENCE_ONLY`  
Reuse status: GPLv3；no source reuse。  
License / provenance: GPLv3 confirmed。

## Confirmed evidence

Suzuki SV650AK7 → diagnostic connector → K-Line/SDS → L9637D → ESP32 UART → Bluetooth/Android；upstream describes ESP32 DevKitC、DC/DC buck、GND、switched +12V 與 K-Line。

## Transferable patterns

Vehicle/adapter-supplied power may be relevant to future link hardware and needs voltage conversion/protection。

## Project-specific implications

Evidence is Suzuki-specific；不推論所有 motorcycle connectors 都有 +12V 或相同 pinout。

## Do not infer / do not reuse

不得 copy GPL source或將此 hardware pattern 寫成 project hardware selection。

## Revisit trigger

Future vehicle adapter hardware assessment 時。
