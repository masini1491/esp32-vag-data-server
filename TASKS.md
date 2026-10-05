# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 7 BLE host-side contract / mock

Status: Ready

Goal:
- 建立 host-testable、read-only、bounded 的 BLE application contract / mock，完全依賴既有 application-facing VehicleData read model。
- 本 Stage 只定義 logical BLE payload semantics 與 deterministic host encoder/mock；不得選定或綁定 ESP32 BLE stack、實體 UUID、MTU、notify timing、radio/runtime或硬體行為。

Canonical evidence:
- `evidence/inbox/phase7-ble-host-contract-readiness-2026-10-05.md`
- current application-facing VehicleData read model
- `docs/ARCHITECTURE.md`
- `docs/DEVELOPMENT.md`
- `VALIDATION.md`

Required implementation contract:
1. Source-of-truth:
   - BLE-facing data只能來自既有 application read model / copied snapshot；
   - 不直接讀 VehicleDataStore internals；
   - 不存取 OBD/UDS/ReadOnlyGuard；
   - 不觸發 Scheduler；
   - client read不得產生 ECU traffic。

2. Logical payloads:
   - versioned snapshot metadata record；
   - bounded capability record by index；
   - bounded sample record by index；
   - preserve normalized signal identity、value type/value presence、unit、source、quality、availability、CapabilitySupport與 exact 64-bit timestamp；
   - absent/no-value不得以0/false/empty假值代替；
   - 不暴露 raw CAN IDs、UDS DIDs、ECU routes或品牌 mapping internals。

3. Bounded encoding:
   - caller-provided fixed buffer；
   - insufficient capacity需回傳明確 required capacity；
   - no silent truncation；
   - no dynamic allocation solely for this slice；
   - identical input -> deterministic identical bytes；
   - malformed/inconsistent input -> explicit failure；
   - payload contract must be versioned。

4. Host mock / decoder:
   - 提供足以驗證 metadata、capability、numeric/boolean/text samples、no-value states與 timestamp round-trip/interpretation 的 host-side mock/test harness；
   - mock不等於 BLE radio evidence。

5. Deferred physical BLE decisions:
   - 不選 ESP32 BLE library / NimBLE stack；
   - 不凍結 service/characteristic UUID；
   - 不假設 negotiated MTU；
   - 不實作 real ATT fragmentation/reassembly；
   - 不決定 notify/indicate scheduling、connection interval、pairing/bonding/security、radio coexistence或 power/throughput。

Deterministic host-test minimum:
- unresolved profile metadata；
- selected profile metadata；
- selected Kamiq_NW4 的四個 Pending capability records；
- empty sample set；
- numeric / boolean / text samples；
- Available、Unsupported、Unavailable、Pending、Unknown distinctions；
- exact 64-bit timestamp preservation；
- deterministic output；
- capacity boundary與 one-byte-short failure；
- malformed/inconsistent input rejection；
- repeated encoding side-effect free；
- source/API inspection確認無 diagnostic transport、OBD/UDS、ReadOnlyGuard、Scheduler、Arduino/ESP32/BLE stack dependency、brand raw mapping或 global mutable singleton；
- all existing host regressions PASS；
- `git diff --check` PASS。

Validation / completion:
- host compile/tests PASS；
- 若新增獨立 mock/decoder tooling，記錄 exact reproducible host command；
- ESP32 compile不因本 Stage為形式重跑，除非 ESP32-facing source participation/platform boundary真的改變；
- canonical docs/validation只能宣稱 host BLE application contract / mock；
- Bench / Hardware / Vehicle維持 Pending，`VEHICLE_CONFIRMED = none`。

Explicit non-goals:
- real BLE stack integration；
- ESP32 BLE compile/runtime；
- UUID/MTU/notify policy；
- smartphone interoperability；
- live polling / Scheduler integration；
- diagnostic TX；
- Logger；
- Phase 6c；
- physical validation。

Actor ownership:
- Codex: source/tests/mock and Stage-required canonical docs/validation mutation.
- ChatGPT: result reconciliation, Hot cleanup and next-stage sequencing.

STOP:
- if this slice requires real BLE hardware/stack, physical MTU behavior, Scheduler/polling, diagnostic TX, brand raw mappings, dynamic framework architecture or control/write semantics, STOP and report instead of expanding scope.

---
