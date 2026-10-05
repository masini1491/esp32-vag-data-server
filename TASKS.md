# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 3C Generic OBD-II stored DTC read (Mode 0x03)

Status: Ready

Goal:
- 擴充既有 Generic OBD-II read-only host service，加入 bounded、host-testable 的 stored DTC read（Mode `0x03`）。
- 本 Stage 只處理 Mode `0x03` request/positive-response framing 與 bounded raw DTC record ownership；不得加入 Clear DTC、品牌解碼、VehicleData mapping、Scheduler/application integration 或實車支援宣稱。

Current baseline:
- `ReadOnlyGuard` 目前只提供 semantic OBD single-PID Mode `0x01` / `0x09` 與 UDS `0x22`。
- `ObdService` 目前支援 Mode `0x01` current data / supported PID bitmap 與 Mode `0x09` PID `0x02` VIN。
- Mode `0x03` 沒有 PID；不得為此放寬現有 `startObdSinglePid()` 或引入 arbitrary raw OBD request API。

Required implementation contract:
1. ReadOnlyGuard:
   - 新增一個專用 semantic stored-DTC read entry point，由 guard 自己建立單-byte request `{0x03}`。
   - 不接受 caller 提供 arbitrary payload。
   - 現有 Mode `0x01` / `0x09` single-PID allowlist semantics保持不變。
   - Mode `0x04` Clear DTC、其他未授權 OBD mode仍必須 fail closed / PolicyDenied。

2. ObdService:
   - 新增一個明確的 stored-DTC request API，例如 `requestStoredDtcs()`；實際 symbol可依現有 naming style決定，但不得變成 generic raw-mode API。
   - 與現有 service共用 one-outstanding-request / timeout / transport failure lifecycle。
   - 正確 positive service為 `0x43`。
   - 不相關 service response保持 UnexpectedResponse，不得被誤收。
   - 正確 `0x43` payload必須以完整 two-byte DTC records組成；奇數長度的 DTC payload fail closed為 InvalidResponse。
   - 空 DTC payload可表示 zero stored DTCs。
   - 儲存必須 fixed/bounded；不得 dynamic allocation。
   - 本 Stage保留每筆 DTC的 two-byte raw code，或以等價 bounded representation保存；不要加入品牌-specific interpretation。
   - `0x0000` padding / no-code record若被接受，必須有明確、deterministic semantics與測試，不得產生假 DTC。
   - 超過 bounded capacity必須 fail closed，不得截斷後假裝完整成功。

3. Safety / ownership:
   - 不實作 Mode `0x04`。
   - 不實作 DTC clearing、freeze-frame clearing、coding、adaptation、actuation或任何 write/control operation。
   - 不把 DTC轉成 `VehicleData`。
   - 不做 VAG/Kamiq-specific DTC text/database mapping。
   - 不做 application-facing polling / Scheduler binding。
   - 不建立 generic arbitrary OBD request executor。

Deterministic host-test minimum:
- guard stored-DTC API只送出 exactly `0x03`；
- Mode `0x04` 與現有未授權 mode仍被拒絕；
- one-outstanding-request / Busy semantics；
- correct `0x43` zero-DTC response；
- one and multiple complete two-byte DTC records；
- unrelated service response保持 UnexpectedResponse；
- malformed odd-length DTC payload -> InvalidResponse；
- bounded-capacity overflow -> fail closed；
- deterministic `0x0000` padding/no-code semantics；
- timeout與現有 transport failures；
- 現有 OBD / UDS / VehicleData / Scheduler / Phase 6 regressions全部 PASS；
- source/API inspection確認沒有 arbitrary raw request、Mode `0x04`、VehicleData mutation、VAG/Kamiq semantics、Scheduler binding、Arduino/ESP32/FreeRTOS dependency或 global mutable singleton。

Validation / completion:
- host compile/tests PASS；
- `git diff --check` PASS；
- 若 ESP32-facing source participation / platform boundary未改變，不需為形式重新跑 ESP32 compile；必須誠實維持既有 evidence scope；
- canonical docs/validation只可宣稱 Generic OBD-II stored DTC read host slice；
- Bench / Hardware / Vehicle維持 Pending。

Explicit non-goals:
- Mode `0x04` Clear DTC；
- freeze frame / permanent / pending DTC broader modes；
- UDS `0x19`；
- SAE/VAG description database；
- profile/capability mapping；
- application read model；
- BLE / Web / Logger；
- physical vehicle validation。

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: result reconciliation, Hot cleanup and next-stage promotion decision.

STOP:
- if implementing Mode `0x03` requires an arbitrary raw diagnostic API, broader DTC command family, vehicle-specific data, write/control semantics or application framework expansion, STOP and report instead of broadening scope.

---
