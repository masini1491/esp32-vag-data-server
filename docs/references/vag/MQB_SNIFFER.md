# MQB-sniffer

Upstream: [mrfixpl/MQB-sniffer](https://github.com/mrfixpl/MQB-sniffer)
Reviewed: 2026-08-22
Role: read-only VAG reverse-engineering methodology
Evidence class: `RESEARCH_METHOD_REFERENCE`, `REFERENCE_ONLY`
Reuse status: no source reuse。
License / provenance: no clear reusable license confirmed。

## Confirmed evidence

Example vehicle is 2016 Golf MK7；workflow uses known diagnostic tool + OBD splitter/sniffer → select live value → capture request/response → correlate display/raw → infer candidate DID/scaling → repeat/validate。

## Transferable patterns

Read-only observation and evidence correlation methodology。

## Project-specific implications

Golf route/DID observations are not Kamiq 2024 evidence。

## Do not infer / do not reuse

不得 copy source、使用 injection examples、brute-force SRS/Airbag、output tests、coding、basic settings 或 security access。

## Revisit trigger

未來實車 read-only research SOP 需要具體方法時。
