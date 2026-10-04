# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 6a profile identity and Active Profile selection boundary

Status: Ready

Goal:
- 建立最小、host-testable 的 Profile identity / Active Vehicle Profile selection ownership boundary。
- 本 Stage 只建立 profile identity、selection state 與一個 concrete VAG Profile Set target identity；不得建立 capability/signal registry、resolver algorithm、route/DID/scaling mapping 或 diagnostic execution。

Canonical evidence:
- `evidence/inbox/phase6-readiness-2026-10-04.md`
- `evidence/inbox/phase6a-profile-identity-readiness-2026-10-04.md`
- `docs/ARCHITECTURE.md`
- `docs/VEHICLE_PROFILE.md`
- `docs/references/SYNTHESIS.md`
- `docs/references/portability/vag/KAMIQ.md`

Required implementation contract:
- Keep Generic protocol/data layers brand-independent.
- Add a minimal brand-independent Active Vehicle Profile selection state with exactly these semantic outcomes:
  - `Unknown`
  - `Ambiguous`
  - `ManualSelectionRequired`
  - `Selected`
- Profile identity must be opaque to Generic protocol/data code; do not embed route/DID/signal semantics in the generic selection type.
- Add a minimal compile-time VAG Profile Set with one concrete current project target identity for Škoda Kamiq 2024 facelift.
- The 2024 target identity may preserve the official model identity token/reference `Kamiq_NW4`, but must remain explicitly Pending / not vehicle-confirmed.
- Selection state and profile evidence/validation state are separate:
  - selecting the profile does not establish Hardware/Vehicle PASS;
  - selecting the profile does not establish any capability or signal support.
- First slice supports manual/explicit selection only.
- Unknown/out-of-set identity must not become Active.
- Transition from Selected to `Unknown`, `Ambiguous`, or `ManualSelectionRequired` clears the active profile.
- Unresolved states expose no active profile identity.
- Selected state exposes exactly one admitted profile identity.

Upstream/evidence boundary:
- Current opendbc `SKODA_KAMIQ_MK1` evidence is a useful profile-identity reference, but its current documented vehicle range is 2021–23.
- Do not copy its firmware fingerprint table into runtime source in this Stage.
- Do not treat opendbc WMI/chassis/fingerprint evidence as confirmed 2024 facelift resolver authority.
- No third-party implementation code is copied.

Deterministic host-test minimum:
- default state is unresolved and exposes no active profile;
- explicit manual selection of the admitted Kamiq 2024 profile succeeds and exposes exactly that profile identity;
- unknown/out-of-set identity selection is rejected without changing current state;
- `Unknown`, `Ambiguous`, and `ManualSelectionRequired` each clear any previously selected profile;
- selection does not alter or create VehicleData values/capabilities;
- the admitted Kamiq 2024 profile descriptor remains Pending / not vehicle-confirmed;
- source/API inspection confirms no VIN/WMI/chassis parser, firmware matcher, automatic resolver, capability registry, route/DID/scaling/passive-CAN mapping, Scheduler binding, diagnostic execution, BrandAdapter plugin framework, parser/schema/storage engine, Arduino/ESP32/FreeRTOS dependency or global mutable singleton.

Explicit non-goals:
- no automatic Profile Resolver algorithm;
- no VIN/WMI/chassis parsing;
- no firmware fingerprint matching;
- no profile inheritance engine;
- no profile file/parser/schema/storage format;
- no capability or normalized signal registry;
- no polling policy/source priority implementation;
- no OBD/UDS/VAG route definitions;
- no DID/scaling/passive-CAN mappings;
- no Scheduler integration;
- no Deep Diagnostic implementation;
- no Hardware/Vehicle PASS claim.

Actor ownership:
- Codex: source/tests and Stage-required canonical docs/validation mutation.
- ChatGPT: readiness/evidence, diff/code review, canonical reconciliation and Hot cleanup.

Validation / completion:
- project host compile/tests PASS with new Phase 6a tests and all existing regressions;
- `git diff --check` PASS;
- if ESP32-facing source participation/platform boundary does not change, no new ESP32 compile is required solely for formality; evidence scope must remain explicit;
- Bench / Hardware / Vehicle remain Pending;
- canonical docs may claim only the profile identity / Active Profile selection boundary implemented here.

STOP:
- if implementation requires capability/signal semantics, automatic vehicle detection, concrete VAG ECU routing, DID/scaling/passive CAN data, profile persistence/parser/schema, Scheduler binding, diagnostic execution or broader plugin/framework architecture, STOP and report instead of expanding scope.

---
