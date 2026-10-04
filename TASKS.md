# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — EV-1 Kamiq read-only evidence acquisition protocol

Status: Ready

Goal:
- 建立 canonical docs-only 的 Kamiq read-only evidence acquisition / research protocol，作為 Phase 6c evidence unlock 與未來 Phase 10 validation 的前置規格。
- 本 Stage 只凍結採證方法、evidence schema、safety gates 與 promotion threshold；不得執行實車測試、建立 firmware TX、capture tooling、hardware selection 或任何 vehicle-support claim。

Canonical evidence:
- `evidence/inbox/kamiq-readonly-evidence-acquisition-readiness-2026-10-05.md`
- `evidence/inbox/phase6c-deep-diagnostic-readiness-2026-10-05.md`
- `docs/READ_ONLY_POLICY.md`
- `docs/references/portability/vag/KAMIQ.md`
- `docs/references/vag/MQB_SNIFFER.md`
- `docs/references/vag/TELLTALE.md`
- `VALIDATION.md`

Required canonical docs:
- Add one focused protocol/SOP document under `docs/` using the existing repository documentation style.
- Update only the minimum index/roadmap/validation owner(s) needed to route to the new SOP and clarify its non-PASS status.

Required protocol content:
1. Purpose and scope:
   - research/evidence acquisition precursor only;
   - no Bench/Hardware/Vehicle PASS;
   - no authority expansion.

2. Preferred acquisition hierarchy:
   - Path A: passive observation of a legitimate known diagnostic tool through splitter/passive observation topology;
   - Path B: project-originated active read only after exact request/route is already known/reviewed and only through existing read-only guard semantics;
   - Path B is not authorized merely by creating this SOP.

3. Explicit forbidden methods:
   - no live DID/address brute force, especially SRS/Airbag sweeps;
   - no Coding/Adaptation/Clear DTC/SecurityAccess/RoutineControl/Output Tests/Basic Settings/flashing/actuator control;
   - no copying another MQB route/DID into Kamiq runtime merely by similarity;
   - no bypass of ReadOnlyGuard;
   - no single timeout/NO DATA -> Unsupported promotion.

4. Evidence schema:
   - sanitized vehicle/session context;
   - tool/version/capture topology;
   - route/request/response/controller provenance;
   - service/DID/request bytes/response bytes where known;
   - displayed-value/raw correlation;
   - candidate decode/scaling/unit;
   - repeated observations / ambiguity notes;
   - evidence classification and unresolved items;
   - privacy sanitization, including no full VIN unless explicitly required and approved.

5. Minimum correlation standard:
   - numeric mappings should prefer at least two materially different observations explained by the same decode/scaling;
   - discrete/status mapping should prefer more than one known state when safe;
   - safety always overrides completeness;
   - unresolved multi-responder ambiguity blocks promotion.

6. First-session sequence:
   - capture-path preflight;
   - known low-risk baseline exchange;
   - one known VAG measuring value;
   - one selected Deep Diagnostic candidate only after capture path works;
   - post-session classification/review before any runtime admission.

7. Phase 6c promotion threshold:
   - selected profile;
   - normalized capability/signal identity;
   - target ECU/route;
   - exact authorized read-only service + DID;
   - raw response validation;
   - deterministic decode/scaling + unit;
   - bounded failure/status semantics;
   - provenance/evidence classification;
   - no unresolved responder ambiguity;
   - ReadOnlyGuard remains mandatory.

8. Relation to Phase 10:
   - observation through external tooling is research evidence, not end-to-end project Vehicle PASS;
   - Phase 10 minimum dataset remains VIN, vehicle.speed, vehicle.rpm, vehicle.coolantTemp, vehicle.voltage.

Explicit non-goals:
- no source/tests/tooling/workflow mutation unless strictly necessary for docs lint/index and explicitly allowed by current governance;
- no firmware or project-originated diagnostic TX;
- no physical capture;
- no hardware/board/transceiver/GPIO selection;
- no exact Kamiq route/DID/scaling claim;
- no capability promotion to Supported;
- no Phase 6c implementation;
- no Phase 10 PASS.

Validation / completion:
- changed files limited to the minimum canonical docs required by this Stage;
- `git diff --check` PASS;
- canonical read-back confirms SOP is routed from the appropriate owner/index and clearly states no physical PASS / no execution authority;
- report exact changed-file set and final commit SHA.

Actor ownership:
- Codex: canonical docs mutation + validation + commit/push.
- ChatGPT: readiness/evidence, result reconciliation, Hot cleanup and next-step decision.

STOP:
- if the SOP would require inventing vehicle mappings, choosing physical hardware, authorizing new TX semantics, or changing read-only policy, STOP and report instead of expanding scope.

---
