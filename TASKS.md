# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Logger / export contract

Status: Ready

Goal:
- 建立 host-testable、bounded、versioned 的 normalized VehicleData export record contract，供後續 Logger / export 使用。
- 本 Stage 只處理單筆 normalized sample 的 deterministic serialization / parse-validation 與 host tests；不得實作 filesystem、SD/NVS、file rotation、queue、background writer、Scheduler integration、diagnostic TX 或實體 ESP32 storage。

Canonical evidence:
- `evidence/inbox/logger-export-contract-readiness-2026-10-05.md`
- current `src/core/vehicle_data.h`
- current application-facing VehicleData read model
- current Phase 8 Web contract
- current Phase 7 BLE host contract
- `VALIDATION.md`
- `docs/ARCHITECTURE.md`
- `docs/DEVELOPMENT.md`

Required implementation contract:
1. Source-of-truth:
   - export input must be existing `NormalizedVehicleSample`;
   - preserve existing SignalId, value/value-presence, unit, source/provenance, quality, availability and exact 64-bit timestamp semantics;
   - do not define a second signal identity/value/quality/availability system;
   - do not infer or synthesize timestamps, freshness, TTL or support state.

2. Export record:
   - versioned record format;
   - one logical record per normalized sample;
   - deterministic representation for identical input;
   - caller-provided bounded buffer;
   - explicit required-capacity result;
   - insufficient capacity fails without silent truncation;
   - malformed or semantically inconsistent input fails explicitly;
   - absent/no-value states must remain distinct from numeric zero, false or empty text.

3. Encoding:
   - representation may be compact textual or binary, but must be deterministic and host-testable;
   - preserve exact 64-bit timestamp without precision loss;
   - numeric values must be finite;
   - text/signal/unit boundaries must respect existing bounded types;
   - format must have an explicit schema/version discriminator;
   - do not reuse Web JSON or BLE bytes as the canonical logger record merely for convenience unless the implementation proves that representation independently satisfies Logger semantics. Logger is a consumer contract, not a second domain owner.

4. Parser / validation harness:
   - provide host-side parser/decoder or equivalent validation harness that can prove semantic round-trip/interpretation;
   - this harness is validation tooling only and does not become a runtime storage engine.

5. Explicitly deferred storage/runtime decisions:
   - no filesystem path or file naming;
   - no CSV/JSONL/file-extension freeze unless strictly required by the bounded record contract;
   - no SD card / SPIFFS / LittleFS / NVS binding;
   - no buffering queue/ring buffer;
   - no rotation/retention policy;
   - no background flush/thread/task;
   - no Scheduler integration;
   - no persistence durability/atomicity claim;
   - no telemetry upload/cloud path.

Deterministic host-test minimum:
- numeric sample;
- boolean sample;
- text sample;
- Unsupported / Unavailable / Pending / Unknown no-value samples;
- exact 64-bit timestamp preservation;
- source / quality / availability preservation;
- deterministic identical output;
- explicit schema/version handling;
- capacity sizing + exact-capacity success + one-byte-short failure with no silent truncation;
- malformed/truncated/version-invalid input rejection;
- non-finite numeric rejection;
- repeated encode/decode side-effect free;
- source/API inspection confirms no filesystem, SD/NVS, Scheduler, OBD/UDS, ReadOnlyGuard, Arduino/ESP32 dependency, brand raw mapping or global mutable singleton;
- all existing host regressions PASS;
- `git diff --check` PASS.

Validation / completion:
- host compile/tests PASS;
- if an independent parser/decoder test utility is added, record exact reproducible host command;
- if ESP32-facing source participation/platform boundary remains unchanged, no formal ESP32 compile required;
- canonical docs/validation may claim only host-side Logger/export record contract;
- Bench / Hardware / Vehicle remain Pending; `VEHICLE_CONFIRMED = none`.

Explicit non-goals:
- file I/O;
- SD/NVS/SPIFFS/LittleFS;
- retention/rotation;
- background logging;
- Scheduler/polling;
- diagnostic TX;
- BLE/Web changes;
- Phase 6c;
- physical storage/runtime validation.

Actor ownership:
- Codex: source/tests/parser-or-mock and Stage-required canonical docs/validation mutation.
- ChatGPT: result reconciliation, Hot cleanup and next-stage decision.

STOP:
- if this slice requires physical storage, filesystem semantics, Scheduler/polling, diagnostic TX, brand-specific raw mappings, dynamic logging framework architecture or control/write behavior, STOP and report instead of expanding scope.

---
