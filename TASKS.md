# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Application-facing VehicleData read model / snapshot API

Status: Ready

Goal:
- 建立 transport-neutral、host-testable、read-only 的 application-facing VehicleData projection，作為 BLE / Web / Logger 共用 consumer boundary。
- 本 Stage 只讀既有 Active Profile、Capability Registry 與 VehicleData Store/Cache；不得發出診斷 TX、觸發 Scheduler、綁定 BLE/Web/Arduino library或建立另一套 domain identity/model。

Canonical evidence:
- `evidence/inbox/application-read-model-readiness-2026-10-05.md`
- `docs/ARCHITECTURE.md`
- current `src/profiles/active_vehicle_profile.h`
- current `src/profiles/signal_registry.h`
- current `src/core/vehicle_data_store.h`
- current `src/core/vehicle_data.h`

Required implementation contract:
1. Read model responsibility:
   - expose current `ProfileSelectionState`;
   - expose active `ProfileIdentity` only when selected;
   - expose bounded capability descriptor metadata associated with the selected profile, when available;
   - expose bounded VehicleData Store snapshot;
   - preserve existing source-of-truth semantics without inventing fallback values.

2. Profile semantics:
   - unresolved states (`Unknown`, `Ambiguous`, `ManualSelectionRequired`) expose no active identity;
   - selected state exposes the selected identity;
   - do not auto-select or guess a profile.

3. Capability semantics:
   - use existing `CapabilitySupport` / `NormalizedSignalDescriptor` types or a minimal copy/view wrapper only when ownership safety requires it;
   - preserve support state exactly;
   - selected `Kamiq_NW4` may expose exactly its existing four descriptors, all still `Pending`;
   - no selected profile / no associated registry -> no fabricated capability list and no fabricated `Unsupported`.

4. VehicleData snapshot:
   - read only through existing `VehicleDataStore::snapshot()` semantics;
   - preserve SignalId, value, unit, source, quality, availability and timestamp exactly;
   - insufficient caller capacity must fail explicitly; no silent truncation / partial-success claim;
   - do not add TTL/staleness inference or mutate sample state.

5. Side-effect boundary:
   - no OBD / UDS / ReadOnlyGuard call;
   - no Scheduler request;
   - no Store mutation;
   - no profile/capability mutation;
   - no route/DID discovery;
   - repeated client reads must not generate vehicle traffic.

API / architecture constraints:
- fixed/bounded host-testable C++;
- no dynamic allocation solely for this slice;
- no JSON as the canonical domain contract;
- no BLE/Web/Wi-Fi/Arduino dependency;
- no callback/event framework;
- no second SignalId/ProfileIdentity/CapabilitySupport/VehicleData identity system.

Deterministic host-test minimum:
- unresolved profile states -> no active identity;
- selected admitted profile -> correct active identity;
- selected Kamiq_NW4 -> exactly four existing capability descriptors, all Pending;
- unknown/no-registry path -> empty/not-present capability view, never fabricated Unsupported;
- empty Store -> empty snapshot;
- populated Store -> multiple samples preserved exactly;
- insufficient capacity -> explicit failure, no silent truncation;
- repeated reads deterministic and side-effect free;
- Store/profile/registry state unchanged after reads;
- source/API inspection confirms no diagnostic transport, OBD/UDS, ReadOnlyGuard, Scheduler, BLE/Web/Arduino dependency or global mutable singleton;
- all existing regressions PASS.

Validation / completion:
- host compile/tests PASS;
- `git diff --check` PASS;
- if ESP32-facing source participation/platform boundary remains unchanged, no formal ESP32 recompile required; keep existing evidence scope explicit;
- canonical docs/validation may claim only host-side application read projection;
- Bench / Hardware / Vehicle remain Pending; `VEHICLE_CONFIRMED = none`.

Explicit non-goals:
- BLE characteristic/payload implementation;
- HTTP/JSON/Web UI;
- Logger/export format;
- polling/Scheduler integration;
- DTC presentation/API;
- profile resolver/detection;
- TTL/aging;
- capability learning/mutation;
- VAG/Kamiq raw mapping;
- physical validation.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: result reconciliation, Hot cleanup and next-stage promotion decision.

STOP:
- if implementation requires diagnostic TX, Scheduler/polling behavior, transport-specific client code, new domain identity systems, dynamic framework architecture, vehicle-specific mapping or hardware dependencies, STOP and report instead of broadening scope.

---
