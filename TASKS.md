# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 8 Web API/UI fixture path

Status: Ready

Goal:
- 在不依賴實體 ESP32 Wi-Fi / Web server 的前提下，建立 host-testable 的 Web-facing API response contract 與 fixture-driven UI 行為，完全依賴既有 application-facing VehicleData read model。
- 本 Stage 只處理 read-only presentation/serialization boundary 與 fixture/mock 驗證；不得發出 diagnostic TX、觸發 Scheduler、綁定實體網路 stack或宣稱 Web hardware/runtime PASS。

Canonical evidence:
- current application-facing VehicleData read model implementation / docs
- `docs/ARCHITECTURE.md`
- `docs/DEVELOPMENT.md`
- `VALIDATION.md`
- Phase 8 Cold trigger from `BACKLOG.md`

Required implementation contract:
1. API/read boundary:
   - Web-facing data must come only from application read model output;
   - no direct VehicleDataStore internals, profile internals, OBD/UDS, ReadOnlyGuard or Scheduler access;
   - repeated refresh/render must not generate vehicle traffic.

2. Fixture / host path:
   - define a deterministic fixture/mock path that can represent profile selection state, capability descriptors and normalized samples without ESP32/Wi-Fi hardware;
   - fixture must preserve existing support / availability / quality / source / timestamp semantics;
   - no fabricated zero/false/default for Pending/Unknown/Unavailable/Unsupported.

3. Serialization contract:
   - use a bounded, deterministic representation suitable for future Web API consumption;
   - preserve normalized signal identity and metadata;
   - distinguish absent/not-present fields from actual values;
   - do not expose raw CAN IDs, UDS DIDs, ECU routes or brand-internal mapping details;
   - do not make JSON a new source-of-truth domain model.

4. UI behavior:
   - host/browser fixture path may render current profile state, capabilities and samples;
   - unresolved profile / Pending capability / unavailable sample states must remain explicit;
   - UI must not imply live vehicle connectivity when using fixtures;
   - no control/write/diagnostic action surface.

5. Dependency boundary:
   - no ESP32 Wi-Fi / AsyncWebServer / Arduino-specific dependency required for this Stage;
   - no physical network binding;
   - no background polling framework;
   - no BLE or Logger implementation.

Deterministic validation minimum:
- fixture with unresolved profile;
- fixture with selected Kamiq_NW4 and four Pending capabilities;
- empty sample set;
- multiple normalized samples with value/unit/source/quality/availability/timestamp preservation;
- explicit handling of Unsupported / Unavailable / Pending / Unknown without fake values;
- deterministic serialized output for the same input;
- source/API inspection confirms no diagnostic transport, OBD/UDS, ReadOnlyGuard, Scheduler, Arduino/Wi-Fi dependency or brand raw mapping leakage;
- browser/host fixture rendering can be exercised without physical ESP32;
- all existing regressions PASS;
- `git diff --check` PASS.

Validation / completion:
- host-side tests/build PASS for the chosen fixture/API path;
- if a static/browser fixture is added, document exact reproducible local validation;
- ESP32 compile is not required solely for this host fixture slice if platform participation does not change;
- Bench / Hardware / Vehicle remain Pending; `VEHICLE_CONFIRMED = none`.

Explicit non-goals:
- real ESP32 HTTP server integration;
- Wi-Fi SoftAP/runtime validation;
- live vehicle polling;
- diagnostic TX;
- BLE;
- Logger/export;
- control commands;
- hardware/vehicle validation.

Actor ownership:
- Codex: source/tests/fixtures and Stage-required canonical docs/validation mutation.
- ChatGPT: reconciliation, Hot cleanup and next-stage sequencing.

STOP:
- if implementation requires physical ESP32 networking, direct ECU access, Scheduler/polling integration, brand-specific raw mappings, control/write actions, or a broad Web framework abstraction without a fixture consumer, STOP and report instead of expanding scope.

---
