# Keyword-Protocol-2000

Upstream: [aster94/Keyword-Protocol-2000](https://github.com/aster94/Keyword-Protocol-2000)
Reviewed: 2026-08-22
Role: future K-Line/KWP architecture and test reference
Evidence class: `ARCHITECTURE_REFERENCE`, `TEST_ARCHITECTURE_REFERENCE`, `REFERENCE_ONLY`
Reuse status: GPLv3；no source reuse。
License / provenance: GPLv3 confirmed。

## Confirmed evidence

Arduino-oriented KWP/K-Line implementation、L9637/MC33660/MC33199 examples、Python ECU emulator；author reports tests on Suzuki GSX-R600 2011 and Kawasaki Versys 650 2012，並明確 Yamaha/Honda 未由作者測試。

## Transferable patterns

K-Line physical/application separation與 emulator testing。

## Project-specific implications

Motorcycle evidence is architecture reference only；不宣稱 KYMCO/SYM 或其他 motorcycle support。

## Do not infer / do not reuse

GPL source、clear DTC、upload/download、control/write behavior 不可直接引入。

## Revisit trigger

Future K-Line scope與明確 license decision 出現時。
