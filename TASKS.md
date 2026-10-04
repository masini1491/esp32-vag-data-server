# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Phase 6R canonical reference/profile semantics reconciliation

Status: Ready

Goal:
- 在 Phase 6 source implementation 前，先把已觸發的 Telltale diagnostic/data-quality evidence bounded reconcile 到 canonical reference/profile owners。
- 本 Stage 只做 docs/reference semantics reconciliation；不得修改 source/tests、不得建立 Brand/Profile implementation、不得宣稱 Kamiq 實車支援。

Canonical evidence:
- `evidence/inbox/phase6-readiness-2026-10-04.md`
- `evidence/inbox/telltale-reference-2026-10-04.md`
- current owners: `docs/REFERENCES.md`, `docs/references/SYNTHESIS.md`, `docs/VEHICLE_PROFILE.md`
- supporting local references: `docs/references/vag/MCD_DIAG_RS.md`, `docs/references/vag/MQB_SNIFFER.md`, `docs/references/vag/VEHICLE_COVERAGE.md`, `docs/references/portability/vag/KAMIQ.md`

Required reconciliation:
1. Add Telltale to `docs/REFERENCES.md` as a bounded Phase 6 / Phase 8 / Phase 10 `REFERENCE_PATTERN`.
   - Reuse classification should remain pattern/method/data-pipeline oriented.
   - Preserve GPL-3.0 / no-direct-source-reuse boundary.
2. Add one local detailed Telltale reference note under the existing reference-note hierarchy; do not copy third-party source/code.
3. Update `docs/references/SYNTHESIS.md` with only the transferable findings that materially affect this project:
   - one failed observation / `NO DATA` does not establish capability `Unsupported`;
   - multi-responder disagreement remains ambiguity until explicit route/source arbitration authority exists;
   - malformed/unsupported decode or scaling fails closed rather than emitting plausible normalized values;
   - diagnostic provenance trace is a later validation/tooling direction, not a requirement for unbounded firmware logging;
   - discovery/simulation does not expand live TX authority.
4. Update `docs/VEHICLE_PROFILE.md` only where it is the canonical owner:
   - capability support authority must remain distinct from transient runtime availability;
   - `Unsupported` must not be inferred from a single failed read / timeout / `NO DATA`;
   - multiple ECU/controller disagreement must remain unresolved/ambiguous until profile/source arbitration exists;
   - decode/scaling uncertainty must not silently produce a normal-looking VehicleData value.
5. Preserve all current Kamiq evidence boundaries:
   - `VEHICLE_CONFIRMED = none`;
   - no exact ECU route / DID / CAN ID / scaling / SFD / passive-CAN claims;
   - no Hardware/Vehicle PASS.

Explicit non-goals:
- no source/test/tooling/workflow mutation;
- no BrandAdapter/plugin framework;
- no Profile Resolver algorithm;
- no profile inheritance engine/parser/schema/storage format;
- no capability registry implementation;
- no Deep Diagnostic implementation;
- no direct GPL-3.0 code reuse;
- no expansion of read-only TX policy.

Validation / completion:
- changed files limited to the minimum canonical reference/profile docs required by this Stage;
- `git diff --check` PASS;
- canonical read-back confirms no physical-support overclaim and no source/test mutation;
- if a new detailed note is added, `docs/REFERENCES.md` and `docs/references/SYNTHESIS.md` must route to it consistently;
- report exact changed-file set and final commit SHA.

Actor ownership:
- Codex: canonical docs/reference mutation + validation + commit/push.
- ChatGPT: readiness/evidence, result reconciliation, Hot cleanup, and fresh Phase 6a readiness decision.

STOP:
- if canonical reconciliation would require inventing concrete VAG/Kamiq mappings, changing runtime/source semantics, or broadening read-only authority, STOP and report instead of expanding scope.

---
