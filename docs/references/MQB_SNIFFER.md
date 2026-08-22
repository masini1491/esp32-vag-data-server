# MQB-sniffer

Repository: [mrfixpl/MQB-sniffer](https://github.com/mrfixpl/MQB-sniffer)

分類：`RESEARCH_METHOD_REFERENCE`、`VAG_UDS_TRACE_REFERENCE`

- `CONFIRMED`: 以 VW Golf MK7 2016、OBD splitter、OBDeleven 與 CAN sniffer 觀察 request/response；示例含 `10 03`、`19 02 ...`、`3E 00`、`22 22 D1`，以及 cluster `0x714 → 0x77E`、DSG `0x7E1 → 0x7E9`。
- `REFERENCE_PATTERN`: known diagnostic tool → select one read-only live value → capture traffic → correlate UI value/raw response → infer DID/scaling → repeat → profile only after validation。
- `VEHICLE_HYPOTHESIS`: upstream Golf MK7 observations 對 Kamiq 2024 只可作研究假設，不得直接加入 profile。
- `DO_NOT_REUSE`: repository 未提供明確 license；不複製 source code。對 SRS/Airbag 禁止 brute-force DID discovery、output test、coding/basic settings/security access。

任何未來 reverse engineering 都必須保持 read-only 與實車 evidence boundary。
