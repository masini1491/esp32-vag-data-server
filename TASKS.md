# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 4C Generic UDS ReadDTCInformation reportDTCByStatusMask (0x19 / 0x02)

Status: Ready

Goal:
- 擴充既有 Generic UDS read-only host service，加入 bounded、host-testable 的 `ReadDTCInformation (0x19) / reportDTCByStatusMask (0x02)`。
- 本 Stage 只處理 semantic request construction、positive/negative response matching、bounded DTC/status record ownership與既有 timeout/NRC lifecycle；不得展開其他 `0x19` subfunction、ClearDiagnosticInformation、session/security、品牌解碼、VehicleData mapping或實車支援。

Canonical evidence:
- `evidence/inbox/phase4c-uds-read-dtc-readiness-2026-10-05.md`
- `docs/READ_ONLY_POLICY.md`
- current `src/protocol/read_only_guard.h`
- current `src/protocol/uds_service.h`
- current host tests / `VALIDATION.md`

Required implementation contract:
1. ReadOnlyGuard:
   - 新增專用 semantic UDS DTC read entry point，只接受 caller 提供的一個 `DTCStatusMask` byte。
   - guard 自己建立 exactly `19 02 <mask>`。
   - 不接受 arbitrary service/subfunction/payload。
   - 現有 `0x22 ReadDataByIdentifier` semantics保持不變。
   - 不加入 `0x14 ClearDiagnosticInformation`、其他 `0x19` subfunction、session/security/write/control操作。

2. UdsService:
   - 新增明確的 `reportDTCByStatusMask` request API；實際 symbol可依現有 naming style決定，但不得變成 generic raw-service API。
   - 與既有 UDS service共用 one-outstanding-request / Busy / timeout / transport failure lifecycle。
   - positive response必須匹配 `59 02 <DTCStatusAvailabilityMask>`。
   - 保存 `DTCStatusAvailabilityMask`。
   - 每筆 record固定保存：
     - 3 raw DTC bytes；
     - 1 raw status byte。
   - zero-record positive response合法。
   - record area長度必須是4的整數倍；不對齊 -> `InvalidResponse`。
   - fixed/bounded storage only；over-capacity fail closed，不得 silent truncate。
   - 不把 all-zero DTC bytes當 padding；只保存 raw protocol records，不做 textual/SAE/VAG decoding。
   - 不轉成 `VehicleData`。

3. Negative response / pending:
   - terminal negative response：`7F 19 <NRC>`。
   - bounded response pending：`7F 19 78`。
   - wrong negative-response service -> `UnexpectedResponse`。
   - malformed negative response -> fail closed。
   - 既有 `0x22` NRC / pending semantics不得 regression。
   - 若為共用既有 lifecycle做最小 refactor，只能依 active semantic request識別 expected service；不得演化成 arbitrary UDS executor。

Deterministic host-test minimum:
- guard送出 exactly `19 02 <mask>`；
- guard API不能指定其他 `0x19` subfunction/service；
- existing `0x22` guard/service regressions PASS；
- one-outstanding-request / Busy；
- valid zero-record response；
- one / multiple complete 4-byte records；
- availability mask與 raw DTC/status bytes正確保存；
- wrong positive service -> UnexpectedResponse；
- wrong `0x19` positive subfunction -> UnexpectedResponse；
- response短於 service/subfunction/availability -> InvalidResponse；
- record area misalignment -> InvalidResponse；
- fixed-capacity overflow -> fail closed；
- terminal `7F 19 NRC`；
- bounded `7F 19 78`；
- wrong negative-response service -> UnexpectedResponse；
- timeout / transport failure；
- 全部既有 OBD / UDS / VehicleData / Scheduler / Phase 6 regressions PASS；
- source/API inspection確認無 arbitrary raw UDS request、其他 `0x19` subfunction、`0x14`、session/security、VehicleData mutation、VAG/Kamiq semantics、Scheduler binding、Arduino/ESP32/FreeRTOS dependency或 global mutable singleton。

Validation / completion:
- host compile/tests PASS；
- `git diff --check` PASS；
- 若 ESP32-facing source participation / platform boundary未改變，不需為形式重跑 ESP32 compile；維持既有 evidence scope；
- canonical docs/validation只可宣稱 Generic UDS `0x19/0x02` host slice；
- Bench / Hardware / Vehicle維持 Pending，`VEHICLE_CONFIRMED = none`。

Explicit non-goals:
- 其他 `0x19` subfunction；
- `0x14 ClearDiagnosticInformation`；
- DTC textual/database decoding；
- VAG/Kamiq-specific DTC meaning；
- ECU route discovery；
- DiagnosticSessionControl / TesterPresent / SecurityAccess；
- application polling / Scheduler integration；
- VehicleData mapping；
- BLE / Web / Logger；
- physical validation。

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: result reconciliation, Hot cleanup and next-stage promotion decision.

STOP:
- if this slice requires arbitrary raw UDS execution, broader DTC family implementation, session/security expansion, vehicle-specific data, write/control semantics or application framework expansion, STOP and report instead of broadening scope.

---
